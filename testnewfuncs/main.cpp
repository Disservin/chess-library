#include <fstream>
#include "chess.hpp"

// UCI parameters to be used by the engines.
// Adapt to your own needs.
constexpr unsigned ThreadSize = 2;
constexpr unsigned HashSize = 1024;
constexpr unsigned StartCount = 1;
constexpr unsigned PositionCount = 10000;
constexpr unsigned MoveTime = 120000;
constexpr unsigned MaxNodes = 0;
const std::string SyzygyPath("S:\\SYZYGY");

// Helper function: convert the mating pv line to SAN notation
static std::string convert_to_san(const std::string& fen, const std::string& pv)
{
	std::string SAN;
	chess::Board board(fen);
	std::istringstream is(pv);

	while (is)
	{
		std::string smove;
		is >> smove;
		if (!smove.empty())
		{
			chess::Movelist list;
			chess::movegen::legalmoves(list, board);
			auto found_move = std::ranges::find_if(list, [&](const chess::Move& m) {
				return chess::uci::moveToUci(m, false) == smove;
				});

			// If the move was not found, the pv is corrupt
			if (found_move == list.end())
				return "";

			SAN += chess::uci::moveToSan(board, *found_move) + ' ';
			board.makeMove(*found_move);
		}
	}

	if (!SAN.empty())
	{
		SAN.pop_back();  // Delete the last space

		// The last character of the SAN string must indicate a mate.
		if (SAN.back() != '#')
			return "";
	}

	return SAN;
}

void test_perft()
{
	// chess::Board board("5B2/6p1/8/4RP1k/4pK1N/6P1/4qP2/8 b");  // EP mate
	// chess::Board board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");  // Kiwi Pete
	// chess::Board board("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");  // Position 3
	// chess::Board board("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -");  // Position 5
	// chess::Board board("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");  // Position 6
	chess::Board board(chess::constants::STARTPOS);
	// chess::Board board("r3k2r/p1pp1pb1/bn2pnp1/q2PN3/Pp2P3/5Q1p/1PP1BPPP/R1BNK2R b KQkq a3 0 3");

	std::cout << board << std::endl;

	for (unsigned depth = 1; depth <= 8; ++depth)
	{
		chess::PerftResults result;
		chess::perft<chess::PerftResults::ALL>(board, depth, result);
		std::cout
			<< "Depth:    " << depth
			<< "\nNodes:    " << result.nodes
			<< "\nCaptures: " << result.captures
			<< "\nE.Ps:     " << result.eps
			<< "\nCastles:  " << result.castles
			<< "\nPromo:    " << result.promotions
			<< "\nChecks:   " << result.checks
			<< "\nDirect:   " << result.direct_checks
			<< "\nDiscov:   " << result.discovery_checks
			<< "\nDouble:   " << result.double_checks
			<< "\nMates :   " << result.mates
			<< "\nTime:     " << result.time << " s"
			<< "\nSpeed:    " << result.speed / 1000000.0f << " Mnps\n"
			<< std::endl;
	}
}

void test_mate()
{
	// Download file from:
	// https://github.com/vondele/matetrack/blob/master/matetrack.epd

	std::ifstream epd("matetrack.epd");
	if (!epd)
		return;

	auto current_date = []() {
		auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::tm tm;
		localtime_s(&tm, &now);
		std::ostringstream os;
		os << std::put_time(&tm, "%F_%H-%M-%S");
		return os.str();
		};

	std::ofstream csv("matelog " + current_date() + ".csv");
	if (!csv)
		return;

	csv << "Hash " << HashSize << " MB;Threads " << ThreadSize << std::endl
		<< "Index;FEN;Mate in;Time [ms];Engine;PV" << std::endl;

	// Add as many UCI engines as you like.
	chess::EnginePool pool;
	pool.run("Stockfish.exe");  // Contains Huntsman-2023

	// NNUE engines
	pool.run("Crystal.exe");
	pool.run("Fluorine-avx512.exe");
	pool.run("ShashChessSantiago-x86-64-avx512.exe");

	// Send initial commands
	pool << "uci";

	std::ostringstream ss;
	ss << "setoption name Threads value " << ThreadSize;
	pool << ss.str();

	ss.str("");
	ss << "setoption name Hash value " << HashSize;
	pool << ss.str();

	if (!SyzygyPath.empty())
	{
		ss.str("");
		ss << "setoption name SyzygyPath value " << SyzygyPath;
		pool << ss.str();
	}

	pool.wait_for_readyok();

	for (unsigned int posIdx = 1; posIdx <= PositionCount && !epd.eof(); ++posIdx)
	{
		// Read the next line from the epd file
		std::string epdLine;
		std::getline(epd, epdLine);

		// Skip the first StartCount positions
		if (posIdx < StartCount)
			continue;

		// Get the FEN.
		auto pos = epdLine.find("bm");
		if (pos == std::string::npos)
			continue;

		std::string fen = epdLine.substr(0, pos - 1);
		std::cout << "\nPosition #" << posIdx << ": " << fen;
		std::string positionFen("position fen " + fen);
		pool << positionFen;

		// Get the "mate in"
		int mateDepth = std::strtol(epdLine.c_str() + pos + 4, 0, 10);
		std::ostringstream ss;
		ss << "go mate " << std::abs(mateDepth);
		if (MaxNodes)
			ss << " nodes " << MaxNodes;
		else if (MoveTime)
			ss << " movetime " << MoveTime;

		std::string goMate = ss.str();
		std::cout << "\nMate in " << mateDepth << std::endl;
		pool << goMate;

		// Read the outputs until mate has been found
		bool stop = false;
		uint64_t elapsedTime = 0;
		size_t countEnginesFinished = 0;
		chess::EngineData last_mate;

		// Begin search mate loop
		while (!stop)
		{
			pool.wait_for_data();
			pool.lock();
			if (pool.get_data().info.find("bestmove") == 0)
			{
				++countEnginesFinished;

				if (!last_mate.info.empty())
				{
					int foundDepth = 0;
					std::string time;
					std::string pv;
					std::istringstream is(last_mate.info);

					// Parse the info string
					while (is)
					{
						std::string token;
						is >> token;
						if (token == "time")
							is >> time;
						else if (token == "mate")
							is >> foundDepth;
						else if (token == "pv")
						{
							pv = last_mate.info.c_str() + last_mate.info.find_last_of("pv") + 2;
							break;
						}
					}

					if (mateDepth == foundDepth)
					{
						std::string SAN = convert_to_san(fen, pv);
						if (!SAN.empty())
						{
							std::cout
								<< "Engine: " << last_mate.engineName
								<< "\nTime  : " << time << " ms"
								<< "\nPV    : " << SAN << std::endl;

							csv << posIdx << ';'
								<< fen << ';'
								<< mateDepth << ';'
								<< time << ';'
								<< last_mate.engineName << ';'
								<< SAN << std::endl;

							stop = true;
						}
					}
				}
			}
			else
			{
				const auto& cd = pool.get_data();
				if (cd.info.find("info") == 0 && cd.info.find("score mate") != std::string::npos)
					last_mate = cd;
			}
			pool.pop_data();
			pool.unlock();

			if (!stop && countEnginesFinished == pool.size())
			{
				std::cout << "No mate found within limits!" << std::endl;

				csv << posIdx << ';'
					<< fen << ';'
					<< mateDepth << ';'
					<< MoveTime << ";;" << std::endl;

				stop = true;
			}
		}

		// Stop all engines
		pool << "stop";

		// Clear all hash tables
		pool << "ucinewgame";

		// Wait for engines to be ready
		pool.wait_for_readyok();

		pool.clear();
	}
}

int main()
{
	// Choose one of the functions to test
	test_perft();
	// test_mate();
	return 0;
}
