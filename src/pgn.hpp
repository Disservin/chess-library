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

        const auto ret = stream_buffer.fill();
        if (!ret.has_value() || !*ret) {
            return;
        }

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

        void remove_suffix(std::size_t n) {
            if (n > index_) {
                throw std::runtime_error("LineBuffer underflow");
            }

            index_ -= n;
        }

       private:
        // PGN lines are limited to 255 characters
        static constexpr int N      = 255;
        std::array<char, N> buffer_ = {};
        std::size_t index_          = 0;
    };

    class StreamBuffer {
       private:
        static constexpr std::size_t N = 512;
        using BufferType               = std::array<char, N * N>;

       public:
        StreamBuffer(std::istream &stream) : stream_(stream) {}

        template <typename FUNC>
        void loop(FUNC f) {
            while (true) {
                if (buffer_index_ >= bytes_read_) {
                    const auto ret = fill();

                    if (!ret.has_value() || !*ret) {
                        return;
                    }
                }

                const auto c = buffer_[buffer_index_];

                // skip carriage return
                if (c == '\r') {
                    buffer_index_++;
                    continue;
                }

                if constexpr (std::is_same_v<decltype(f(c)), bool>) {
                    const auto res = f(c);

                    if (res) {
                        buffer_index_++;
                        return;
                    }
                } else {
                    f(c);
                }

                buffer_index_++;
            }
        }

        /// @brief Assume that the current character is already the opening_delim
        /// @param open_delim
        /// @param close_delim
        /// @return
        bool readUntilMatchingDelimiter(char open_delim, char close_delim) {
            int stack = 0;

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

        std::optional<bool> fill() {
            if (!stream_.good()) return std::nullopt;

            buffer_index_ = 0;

            stream_.read(buffer_.data(), N * N);
            bytes_read_ = stream_.gcount();

            return std::optional<bool>(bytes_read_ > 0);
        }

        std::optional<char> getNextByte() {
            if (buffer_index_ == bytes_read_) {
                const auto ret = fill();
                return ret.has_value() && *ret ? std::optional<char>(buffer_[buffer_index_++]) : std::nullopt;
            }

            return buffer_[buffer_index_++];
        }

        void moveBack() {
            if (buffer_index_ > 0) {
                buffer_index_--;
            }
        }

       private:
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

        line_start = true;

        has_head = false;
        has_body = false;

        in_header = false;
        in_body   = false;
    }

    bool isLetter(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }

    void callVisitorMoveFunction() {
        if (!move.empty()) {
            if (!visitor->skip()) visitor->move(move.get(), comment.get());

            move.clear();
            comment.clear();
        }
    }

    void processHeader() {
        stream_buffer.loop([this](char c) {
            // end of key
            if (c == ' ') {
                // skip whitespace and "
                stream_buffer.getNextByte();
                stream_buffer.getNextByte();

                // read until end of line
                stream_buffer.loop([this](char c) {
                    if (c == '\n') {
                        in_header  = false;
                        line_start = true;

                        // remove last "]
                        header.second.remove_suffix(2);

                        // call visitor
                        if (!visitor->skip()) visitor->header(header.first.get(), header.second.get());

                        header.first.clear();
                        header.second.clear();

                        return true;
                    }

                    // add to value
                    header.second += c;
                    return false;
                });

                stream_buffer.moveBack();

                return true;
            }

            // add to key
            header.first += c;

            return false;
        });
    }

    void processBody() {
        stream_buffer.loop([this](char c) {
            // make sure that the line_start is turned off again
            if (line_start && c != '\n') {
                line_start = false;
            }

            // Pgn are build up in the following way.
            // {move_number} {move} {comment} {move} {comment} {move_number} ...
            // So we need to skip the move_number then start reading the move, then save the comment
            // then read the second move in the group. After that a move_number will follow again.
            switch (c) {
                case '\n':
                    if (line_start) {
                        pgn_end = true;

                        visitor->endPgn();
                        visitor->skipPgn(false);

                        reset_trackers();
                        return true;
                    }

                    line_start = true;

                    break;
                /*
                The second kind starts with a left brace character and continues to the next right brace
                character.
                Brace comments do not nest; a left brace character appearing in a brace comment loses its
                special meaning and is ignored. A semicolon appearing inside of a brace comment loses its
                special meaning and is ignored. Braces appearing inside of a semicolon comments lose their
                special meaning and are ignored.
                */
                case '{':
                    // skip brace
                    stream_buffer.getNextByte();
                    stream_buffer.loop([this](char c) {
                        if (c == '}') {
                            callVisitorMoveFunction();

                            return true;
                        }

                        comment += c;

                        return false;
                    });

                    stream_buffer.moveBack();

                    break;
                default:
                    cbuf[2] = cbuf[1];
                    cbuf[1] = cbuf[0];
                    cbuf[0] = c;

                    // skip variations
                    if (c == '(') {
                        stream_buffer.readUntilMatchingDelimiter('(', ')');
                        return false;
                    }

                    // we are in empty space, when we encounter now a file or a piece, or a castling
                    // move, we try to parse the move.
                    // When we encounter a 0, we first start reading once we are sure it qualifie
                    // as a castling move, by checking the previous moves
                    // O-O(-O) castling moves are caught by isLetter(c), and we need to distinguish
                    // 0-0(-0) castling moves from results like 1-0 and 0-1.

                    if (isLetter(c) || (c == '0' && cbuf[1] == '-' && cbuf[2] == '0')) {
                        if (c == '0') {
                            move += '0';
                            move += '-';
                            move += '0';
                        } else {
                            move += c;
                        }

                        // loop over remaining move string

                        stream_buffer.getNextByte();
                        stream_buffer.loop([this](char c) {
                            // whitespace while reading a move means that we have finished reading the move
                            if (c == ' ' || c == '\n') {
                                callVisitorMoveFunction();
                                return true;
                            }

                            move += c;

                            return false;
                        });

                        stream_buffer.moveBack();
                    }

                    break;
            }

            return false;
        });
    }

    void processNextByte(char c) {
        // PGN Header
        if (line_start && c == '[') {
            if (pgn_end) {
                pgn_end = false;
                visitor->skipPgn(false);
                visitor->startPgn();
            }

            line_start = false;

            has_head = true;

            in_header = true;
            in_body   = false;

            if (!stream_buffer.getNextByte().has_value()) return;
            processHeader();

            // processHeader() will move the buffer_index to the next character
            // so we need to undo this
            stream_buffer.moveBack();
        }
        // PGN Moves Start
        else if (line_start && has_head && !in_header && !in_body) {
            line_start = false;

            has_body = true;

            in_header = false;
            in_body   = true;

            if (!visitor->skip()) visitor->startMoves();
        } else if (in_body) {
            processBody();

            // processBody() will move the buffer_index to the next character
            // so we need to undo this
            stream_buffer.moveBack();
        }
    }

    StreamBuffer stream_buffer;

    Visitor *visitor = nullptr;

    // one time allocations
    std::pair<LineBuffer, LineBuffer> header = {LineBuffer{}, LineBuffer{}};

    LineBuffer move    = {};
    LineBuffer comment = {};

    // buffer for the last two characters, cbuf[0] is the current character
    std::array<char, 3> cbuf = {'\0', '\0', '\0'};

    // State

    // True when at the start of a line
    bool line_start = true;

    bool in_header = false;
    bool in_body   = false;

    bool has_head = false;
    bool has_body = false;

    bool pgn_end = true;
};
}  // namespace chess::pgn
