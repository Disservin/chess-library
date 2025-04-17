#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("Coords") {
    TEST_CASE("File operator==") {
        File f = File::FILE_A;
        CHECK(f == File::FILE_A);
    }

    TEST_CASE("File operator!=") {
        File f = File::FILE_A;
        CHECK(f != File::FILE_B);
    }

    TEST_CASE("File operator>=") {
        File f = File::FILE_A;
        CHECK(f >= File::FILE_A);
    }

    TEST_CASE("File operator<=") {
        File f = File::FILE_A;
        CHECK(f <= File::FILE_A);
    }

    TEST_CASE("File operator>") {
        File f = File::FILE_A;
        CHECK(f < File::FILE_B);
    }

    TEST_CASE("File operator<") {
        File f = File::FILE_B;
        CHECK(f > File::FILE_A);
    }

    TEST_CASE("File operator int") {
        File f = File::FILE_A;
        CHECK(static_cast<int>(f) == 0);
    }

    TEST_CASE("File operator std::string") {
        File f = File::FILE_A;
        CHECK(static_cast<std::string>(f) == "a");

        f = File::FILE_B;
        CHECK(static_cast<std::string>(f) == "b");

        f = File::FILE_C;
        CHECK(static_cast<std::string>(f) == "c");

        f = File::FILE_D;
        CHECK(static_cast<std::string>(f) == "d");

        f = File::FILE_E;
        CHECK(static_cast<std::string>(f) == "e");

        f = File::FILE_F;
        CHECK(static_cast<std::string>(f) == "f");

        f = File::FILE_G;
        CHECK(static_cast<std::string>(f) == "g");

        f = File::FILE_H;
        CHECK(static_cast<std::string>(f) == "h");
    }
}

TEST_SUITE("Rank") {
    TEST_CASE("Rank operator==") {
        Rank r = Rank::RANK_1;
        CHECK(r == Rank::RANK_1);
    }

    TEST_CASE("Rank operator!=") {
        Rank r = Rank::RANK_1;
        CHECK(r != Rank::RANK_2);
    }

    TEST_CASE("Rank operator>=") {
        Rank r = Rank::RANK_1;
        CHECK(r >= Rank::RANK_1);
    }

    TEST_CASE("Rank operator<=") {
        Rank r = Rank::RANK_1;
        CHECK(r <= Rank::RANK_1);
    }

    TEST_CASE("Rank operator int") {
        Rank r = Rank::RANK_1;
        CHECK(static_cast<int>(r) == 0);
    }

    TEST_CASE("Rank operator std::string") {
        Rank r = Rank::RANK_1;
        CHECK(static_cast<std::string>(r) == "1");

        r = Rank::RANK_2;
        CHECK(static_cast<std::string>(r) == "2");

        r = Rank::RANK_3;
        CHECK(static_cast<std::string>(r) == "3");

        r = Rank::RANK_4;
        CHECK(static_cast<std::string>(r) == "4");

        r = Rank::RANK_5;
        CHECK(static_cast<std::string>(r) == "5");

        r = Rank::RANK_6;
        CHECK(static_cast<std::string>(r) == "6");

        r = Rank::RANK_7;
        CHECK(static_cast<std::string>(r) == "7");

        r = Rank::RANK_8;
        CHECK(static_cast<std::string>(r) == "8");
    }
}

TEST_SUITE("Square") {
    TEST_CASE("Square operator==") {
        Square s = Square::SQ_A1;
        CHECK(s == Square::SQ_A1);
    }

    TEST_CASE("Square operator!=") {
        Square s = Square::SQ_A1;
        CHECK(s != Square::SQ_A2);
    }

    TEST_CASE("Square operator>=") {
        Square s = Square::SQ_A1;
        CHECK(s >= Square::SQ_A1);
    }

    TEST_CASE("Square operator<=") {
        Square s = Square::SQ_A1;
        CHECK(s <= Square::SQ_A1);
    }

    TEST_CASE("Square operator>") {
        Square s = Square::SQ_A2;
        CHECK(s > Square::SQ_A1);
    }

    TEST_CASE("Square operator<") {
        Square s = Square::SQ_A1;
        CHECK(s < Square::SQ_A2);
    }

    TEST_CASE("Square operator std::string") {
        Square s = Square::SQ_A1;
        CHECK(static_cast<std::string>(s) == "a1");

        s = Square::SQ_A2;
        CHECK(static_cast<std::string>(s) == "a2");

        s = Square::SQ_A3;
        CHECK(static_cast<std::string>(s) == "a3");

        s = Square::SQ_A4;
        CHECK(static_cast<std::string>(s) == "a4");

        s = Square::SQ_A5;
        CHECK(static_cast<std::string>(s) == "a5");

        s = Square::SQ_A6;
        CHECK(static_cast<std::string>(s) == "a6");

        s = Square::SQ_A7;
        CHECK(static_cast<std::string>(s) == "a7");

        s = Square::SQ_A8;
        CHECK(static_cast<std::string>(s) == "a8");

        s = Square::SQ_B1;
        CHECK(static_cast<std::string>(s) == "b1");

        s = Square::SQ_B2;
        CHECK(static_cast<std::string>(s) == "b2");

        s = Square::SQ_B3;
        CHECK(static_cast<std::string>(s) == "b3");

        s = Square::SQ_B4;
        CHECK(static_cast<std::string>(s) == "b4");

        s = Square::SQ_B5;
        CHECK(static_cast<std::string>(s) == "b5");

        s = Square::SQ_B6;
        CHECK(static_cast<std::string>(s) == "b6");

        s = Square::SQ_B7;
        CHECK(static_cast<std::string>(s) == "b7");

        s = Square::SQ_B8;
        CHECK(static_cast<std::string>(s) == "b8");

        s = Square::SQ_C1;
        CHECK(static_cast<std::string>(s) == "c1");

        s = Square::SQ_C2;
        CHECK(static_cast<std::string>(s) == "c2");

        s = Square::SQ_C3;
        CHECK(static_cast<std::string>(s) == "c3");
    }

    TEST_CASE("Square file()") {
        Square s = Square::SQ_A1;
        CHECK(s.file() == File::FILE_A);

        s = Square::SQ_B1;
        CHECK(s.file() == File::FILE_B);

        s = Square::SQ_C1;
        CHECK(s.file() == File::FILE_C);
    }

    TEST_CASE("Square rank()") {
        Square s = Square::SQ_A1;
        CHECK(s.rank() == Rank::RANK_1);

        s = Square::SQ_A2;
        CHECK(s.rank() == Rank::RANK_2);

        s = Square::SQ_A3;
        CHECK(s.rank() == Rank::RANK_3);
    }

    TEST_CASE("Square ()") {
        Square s = Square(File::FILE_A, Rank::RANK_1);
        CHECK(s == Square::SQ_A1);

        s = Square(File::FILE_B, Rank::RANK_1);
        CHECK(s == Square::SQ_B1);

        s = Square(File::FILE_C, Rank::RANK_1);
        CHECK(s == Square::SQ_C1);
    }

    TEST_CASE("Square ()") {
        Square s = Square("a1");
        CHECK(s == Square::SQ_A1);

        s = Square("b1");
        CHECK(s == Square::SQ_B1);

        s = Square("c1");
        CHECK(s == Square::SQ_C1);
    }

    TEST_CASE("is_light") {
        Square s = Square::SQ_A1;
        CHECK(!s.is_light());

        s = Square::SQ_B1;
        CHECK(s.is_light());

        s = Square::SQ_C1;
        CHECK(!s.is_light());
    }

    TEST_CASE("is_dark") {
        Square s = Square::SQ_A1;
        CHECK(s.is_dark());

        s = Square::SQ_B1;
        CHECK(!s.is_dark());

        s = Square::SQ_C1;
        CHECK(s.is_dark());
    }

    TEST_CASE("is_valid") {
        Square s = Square::SQ_A1;
        CHECK(s.is_valid());

        s = Square::SQ_B1;
        CHECK(s.is_valid());

        s = Square::SQ_C1;
        CHECK(s.is_valid());

        s = Square::NO_SQ;
        CHECK(!s.is_valid());
    }

    TEST_CASE("is_valid file rank") {
        CHECK(Square::is_valid(Rank::RANK_1, File::FILE_A));
        CHECK(Square::is_valid(Rank::RANK_1, File::FILE_B));
        CHECK(!Square::is_valid(Rank::RANK_1, File::NO_FILE));
    }

    TEST_CASE("Square distance") {
        CHECK(Square::distance(Square::SQ_A1, Square::SQ_A1) == 0);
        CHECK(Square::distance(Square::SQ_A1, Square::SQ_A2) == 1);
        CHECK(Square::distance(Square::SQ_A1, Square::SQ_B1) == 1);
        CHECK(Square::distance(Square::SQ_A1, Square::SQ_B2) == 1);
    }

    TEST_CASE("Square value_distance") {
        CHECK(Square::value_distance(Square::SQ_A1, Square::SQ_A1) == 0);
        CHECK(Square::value_distance(Square::SQ_A1, Square::SQ_A2) == 8);
        CHECK(Square::value_distance(Square::SQ_A1, Square::SQ_B1) == 1);
        CHECK(Square::value_distance(Square::SQ_A1, Square::SQ_B2) == 9);
    }

    TEST_CASE("Square same_color") {
        CHECK(Square::same_color(Square::SQ_A1, Square::SQ_A1));
        CHECK(!Square::same_color(Square::SQ_A1, Square::SQ_A2));
        CHECK(!Square::same_color(Square::SQ_A1, Square::SQ_B1));
        CHECK(Square::same_color(Square::SQ_A1, Square::SQ_B2));
    }

    TEST_CASE("Square back_rank") {
        CHECK(Square::back_rank(Square::SQ_A1, Color::WHITE));
        CHECK(!Square::back_rank(Square::SQ_A1, Color::BLACK));
        CHECK(!Square::back_rank(Square::SQ_A8, Color::WHITE));
        CHECK(Square::back_rank(Square::SQ_A8, Color::BLACK));
    }

    TEST_CASE("Square flip") {
        CHECK(Square(Square::SQ_A1).flip() == Square::SQ_A8);
        CHECK(Square(Square::SQ_A2).flip() == Square::SQ_A7);
        CHECK(Square(Square::SQ_A3).flip() == Square::SQ_A6);
    }

    TEST_CASE("Square relative_square") {
        CHECK(Square(Square::SQ_A1).relative_square(Color::WHITE) == Square::SQ_A1);
        CHECK(Square(Square::SQ_A1).relative_square(Color::BLACK) == Square::SQ_A8);
        CHECK(Square(Square::SQ_A2).relative_square(Color::WHITE) == Square::SQ_A2);
        CHECK(Square(Square::SQ_A2).relative_square(Color::BLACK) == Square::SQ_A7);
    }

    TEST_CASE("Square ep_square") {
        CHECK(Square(Square::SQ_A3).ep_square() == Square::SQ_A4);
        CHECK(Square(Square::SQ_A4).ep_square() == Square::SQ_A3);
        CHECK(Square(Square::SQ_A5).ep_square() == Square::SQ_A6);
        CHECK(Square(Square::SQ_A6).ep_square() == Square::SQ_A5);
    }

    TEST_CASE("Square castling_king_square") {
        CHECK(Square::castling_king_square(true, Color::WHITE) == Square::SQ_G1);
        CHECK(Square::castling_king_square(false, Color::WHITE) == Square::SQ_C1);
        CHECK(Square::castling_king_square(true, Color::BLACK) == Square::SQ_G8);
        CHECK(Square::castling_king_square(false, Color::BLACK) == Square::SQ_C8);
    }

    TEST_CASE("Square max") { CHECK(Square::max() == 64); }
}