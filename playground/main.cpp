#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

constexpr std::size_t BUFFER_SIZE = 1024 * 1024;

class StreamParser {
   public:
    StreamParser(std::istream& inputStream)
        : inputStream(inputStream),
          buffers{{}},
          currentBufferIndex(0),
          currentBufferFilledSize(0),
          nextBufferIndex(1),
          nextBufferFilledSize(0),
          stop(false) {
        workerThread = std::thread(&StreamParser::worker, this);
        startLoadingNextBuffer.store(true);
        cv.notify_one();
    }

    ~StreamParser() {
        stop = true;
        cv.notify_one();
        workerThread.join();
    }

    // return a reference to current buffer and start a thread which
    // fills the next buffer
    std::array<char, BUFFER_SIZE>& getFilledBuffer(std::size_t& size) {
        // wait in case the next buffer is not filled yet
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !startLoadingNextBuffer.load(); });

        size = currentBufferFilledSize;
        startLoadingNextBuffer.store(true);
        cv.notify_one();
        return buffers[currentBufferIndex];
    }

   private:
    // we fill the nextbufferindex with data
    void fillBuffer() {
        std::size_t filledSize = 0;
        char* buffer           = buffers[nextBufferIndex].data();

        while (filledSize < BUFFER_SIZE) {
            inputStream.read(buffer + filledSize, BUFFER_SIZE - filledSize);
            filledSize += inputStream.gcount();
            if (inputStream.eof()) {
                break;
            }
        }

        nextBufferFilledSize = filledSize;
    }

    void worker() {
        while (!stop) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return startLoadingNextBuffer.load() || stop; });

            fillBuffer();

            startLoadingNextBuffer.store(false);
            std::swap(currentBufferIndex, nextBufferIndex);
            currentBufferFilledSize = nextBufferFilledSize;
            cv.notify_one();
        }
    }

    std::istream& inputStream;
    std::array<char, BUFFER_SIZE> buffers[2];

    std::size_t currentBufferIndex;
    std::size_t currentBufferFilledSize;

    std::size_t nextBufferIndex;
    std::size_t nextBufferFilledSize;

    std::atomic<bool> startLoadingNextBuffer;
    std::atomic<bool> stop;
    std::thread workerThread;
    std::condition_variable cv;
    std::mutex mtx;
};

int main() {
    std::ifstream inputFile("lichess_db_standard_rated_2016-03.pgn", std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }

    // get file size
    inputFile.seekg(0, std::ios::end);
    std::size_t fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    StreamParser parser(inputFile);

    auto t0 = std::chrono::high_resolution_clock::now();

    std::size_t filledSize;
    const std::array<char, BUFFER_SIZE>* buffer = &parser.getFilledBuffer(filledSize);

    std::uint64_t total = 0;

    while (filledSize > 0) {
        for (std::size_t i = 0; i < filledSize; ++i) {
            if ((*buffer)[i] == '\n') total++;
            // std::cout << (*buffer)[i];
        }
        buffer = &parser.getFilledBuffer(filledSize);
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    std::cout << "Total lines: " << total << "\n";

    std::cout << "MB/s: "
              << (fileSize / 1024.0 / 1024.0) /
                     std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0).count()
              << "\n";

    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0).count() << "s\n";

    return 0;
}
