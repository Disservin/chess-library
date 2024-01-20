#pragma once

#include <array>
#include <iostream>
#include <istream>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace chess::pgn {

/// @brief Visitor interface for parsing PGN files
/// the order of the calls is as follows:
class Visitor {
   public:
    virtual ~Visitor(){};

    /// @brief When true, the current PGN will be skipped and only
    /// endPgn will be called, this will also reset the skip flag to false.
    /// Has to be called after startPgn.
    /// @param skip
    void skipPgn(bool skip) { skip_ = skip; }
    bool skip() { return skip_; }

    /// @brief Called when a new PGN starts
    virtual void startPgn() = 0;

    /// @brief Called for each header
    /// @param key
    /// @param value
    virtual void header(std::string_view key, std::string_view value) = 0;

    /// @brief Called before the first move of a game
    virtual void startMoves() = 0;

    /// @brief Called for each move of a game
    /// @param move
    /// @param comment
    virtual void move(std::string_view move, std::string_view comment) = 0;

    /// @brief Called when a game ends
    virtual void endPgn() = 0;

   private:
    bool skip_ = false;
};

class StreamParser {
   public:
    StreamParser(std::istream &stream) : stream_buffer(stream) {}

    void readGames(Visitor &vis) {
        visitor = &vis;

        stream_buffer.loop([this](char c) { processNextByte(c); });

        if (!pgn_end && has_body) {
            pgn_end = true;

            callVisitorMoveFunction();

            visitor->endPgn();
            visitor->skipPgn(false);
        }
    }

   private:
    class LineBuffer {
       public:
        bool empty() const noexcept { return index_ == 0; }

        void clear() noexcept { index_ = 0; }

        std::string_view get() const noexcept { return std::string_view(buffer_.data(), index_); }

        void operator+=(char c) {
            if (index_ < N) {
                buffer_[index_++] = c;
            } else {
                throw std::runtime_error("LineBuffer overflow");
            }
        }

       private:
        // PGN lines are limited to 255 characters
        static constexpr int N      = 255;
        std::array<char, N> buffer_ = {};
        std::size_t index_          = 0;
    };

    class StreamBuffer {
       private:
        static constexpr std::size_t N = 1024;
        using BufferType               = std::array<char, N * N>;

       public:
        StreamBuffer(std::istream &stream) : stream_(stream) {}

        template <typename FUNC>
        void loop(FUNC f) {
            const auto ret = fill();
            if (!ret.has_value() || !*ret) {
                return;
            }

            while (true) {
                if (buffer_index_ == bytes_read_) {
                    const auto ret = fill();

                    if (!ret.has_value() || !*ret) {
                        return;
                    }
                }

                f(buffer_[buffer_index_++]);
            }
        }

        /// @brief Assume that the current character is already the opening_delim
        /// @param open_delim
        /// @param close_delim
        /// @return
        bool readUntilMatchingDelimiter(char open_delim, char close_delim) {
            int stack = 1;

            while (true) {
                const auto ret = getNextByte();

                if (!ret.has_value()) {
                    return false;
                }

                if (*ret == open_delim) {
                    stack++;
                } else if (*ret == close_delim) {
                    if (stack == 0) {
                        // Mismatched closing delimiter
                        return false;
                    } else {
                        stack--;
                        if (stack == 0) {
                            // Matching closing delimiter found
                            return true;
                        }
                    }
                }
            }

            // If we reach this point, there are unmatched opening delimiters
            return false;
        }

       private:
        std::optional<char> getNextByte() {
            if (buffer_index_ == bytes_read_) {
                const auto ret = fill();
                return ret.has_value() && *ret ? std::optional<char>(buffer_[buffer_index_++]) : std::nullopt;
            }

            return buffer_[buffer_index_++];
        }

        std::optional<bool> fill() {
            if (!stream_.good()) return std::nullopt;

            buffer_index_ = 0;

            stream_.read(buffer_.data(), N * N);
            bytes_read_ = stream_.gcount();

            return std::optional<bool>(bytes_read_ > 0);
        }

        std::istream &stream_;
        BufferType buffer_;
        std::streamsize bytes_read_   = 0;
        std::streamsize buffer_index_ = 0;
    };

    void reset_trackers() {
        header.first.clear();
        header.second.clear();

        move.clear();
        comment.clear();

        reading_move    = false;
        reading_comment = false;

        line_start = true;

        has_head = false;
        has_body = false;

        in_header = false;
        in_body   = false;

        // Header
        reading_key   = false;
        reading_value = false;
    }

    bool isLetter(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }

    void callVisitorMoveFunction() {
        if (!move.empty()) {
            if (!visitor->skip()) visitor->move(move.get(), comment.get());

            move.clear();
            comment.clear();
        }
    }

    void processNextByte(const char c) {
        // save the last three characters across different buffers
        cbuf[2] = cbuf[1];
        cbuf[1] = cbuf[0];
        cbuf[0] = c;

        // skip carriage return
        if (c == '\r') {
            return;
        }

        // PGN Header
        if (line_start && c == '[') {
            if (pgn_end) {
                pgn_end = false;
                visitor->skipPgn(false);
                visitor->startPgn();
            }

            has_head = true;

            in_header = true;
            in_body   = false;

            reading_key = true;

            line_start = false;
            return;
        }

        // PGN Moves Start
        if (line_start && has_head && !in_header && !in_body) {
            reading_move    = false;
            reading_comment = false;

            has_body = true;

            in_header = false;
            in_body   = true;

            line_start = false;

            if (!visitor->skip()) visitor->startMoves();
            return;
        }

        // PGN End
        if (line_start && in_body && c == '\n') {
            // buffer_index = i + 1;
            pgn_end = true;

            visitor->endPgn();
            visitor->skipPgn(false);

            reset_trackers();
            return;
        }

        // set line_start to true, since the next char will be first on
        // a new line
        if (c == '\n') {
            line_start = true;
        }

        // make sure that the line_start is turned off again
        if (line_start && c != '\n') {
            line_start = false;
        }

        if (in_header) {
            if (c == '"') {
                reading_value = !reading_value;
            } else if (reading_key && c == ' ') {
                reading_key = false;
            } else if (reading_key) {
                header.first += c;
            } else if (reading_value) {
                header.second += c;
            } else if (c == '\n') {
                reading_key   = false;
                reading_value = false;
                in_header     = false;

                if (!visitor->skip()) visitor->header(header.first.get(), header.second.get());

                header.first.clear();
                header.second.clear();
            }
        }
        // Pgn are build up in the following way.
        // {move_number} {move} {comment} {move} {comment} {move_number} ...
        // So we need to skip the move_number then start reading the move, then save the comment
        // then read the second move in the group. After that a move_number will follow again.
        else if (in_body) {
            // whitespace while reading a move means that we have finished reading the move
            if (c == '\n') {
                reading_move    = false;
                reading_comment = false;

                callVisitorMoveFunction();
            } else if (reading_move && c == ' ') {
                reading_move = false;
            } else if (reading_move) {
                move += c;
            } else if (!reading_comment && c == '{') {
                reading_comment = true;
            } else if (reading_comment && c == '}') {
                reading_comment = false;

                callVisitorMoveFunction();
            }
            // we are in empty space, when we encounter now a file or a piece, or a castling
            // move, we try to parse the move
            else if (!reading_move && !reading_comment) {
                // skip variations
                if (c == '(') {
                    stream_buffer.readUntilMatchingDelimiter('(', ')');
                    return;
                }

                // O-O(-O) castling moves are caught by isLetter(c), and we need to distinguish
                // 0-0(-0) castling moves from results like 1-0 and 0-1.
                if (isLetter(c) || (c == '0' && cbuf[1] == '-' && cbuf[2] == '0')) {
                    callVisitorMoveFunction();

                    reading_move = true;

                    if (c == '0') {
                        move += '0';
                        move += '-';
                        move += '0';
                    } else {
                        move += c;
                    }
                } else {
                    // no new move detected
                    return;
                }
            } else if (reading_comment) {
                comment += c;
            }
        }
    }

    StreamBuffer stream_buffer;

    Visitor *visitor = nullptr;

    // one time allocations
    std::pair<LineBuffer, LineBuffer> header = {LineBuffer{}, LineBuffer{}};

    // std::string move;
    LineBuffer move    = {};
    LineBuffer comment = {};

    // buffer for the last two characters, cbuf[0] is the current character
    std::array<char, 3> cbuf = {'\0', '\0', '\0'};

    // State

    bool reading_move    = false;
    bool reading_comment = false;

    // True when at the start of a line
    bool line_start = true;

    bool in_header = false;
    bool in_body   = false;

    bool has_head = false;
    bool has_body = false;

    // Header
    bool reading_key   = false;
    bool reading_value = false;

    bool pgn_end = true;
};
}  // namespace chess::pgn
