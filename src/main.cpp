#include "chess.hpp"

using namespace Chess;
using namespace Testing;

int main() {
    PerftTesting perft;
    perft.RunPerftTest();
    return 0;
}