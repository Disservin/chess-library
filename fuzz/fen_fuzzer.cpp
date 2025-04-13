#include "../include/chess.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size == 0) return 0;

    std::string_view fen(reinterpret_cast<const char*>(data), size);

    chess::Board b;

    b.setFen(fen);
    b.setEpd(fen);

    return 0;
}