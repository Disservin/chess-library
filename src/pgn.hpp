#pragma once

#include <array>
#include <cassert>
#include <iostream>
#include <istream>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace chess::pgn {

/**
 * @brief Visitor interface for parsing PGN files
 */
class Visitor {
   public:
    virtual ~Visitor() {};

    /**
     * @brief When true, the current PGN will be skipped and only
     * endPgn will be called, this will also reset the skip flag to false.
     * Has to be called after startPgn.
     * @param skip
     */
    void skipPgn(bool skip) { skip_ = skip; }
    bool skip() { return skip_; }

    /**
     * @brief Called when a new PGN starts
     */
    virtual void startPgn() = 0;

    /**
     * @brief Called for each header
     * @param key
     * @param value
     */
    virtual void header(std::string_view key, std::string_view value) = 0;

    /**
     * @brief Called before the first move of a game
     */
    virtual void startMoves() = 0;

    /**
     * @brief Called for each move of a game
     * @param move
     * @param comment
     */
    virtual void move(std::string_view move, std::string_view comment) = 0;

    /**
     * @brief Called when a game ends
     */
    virtual void endPgn() = 0;

   private:
    bool skip_ = false;
};

class StreamParserException : public std::exception {
   public:
    explicit StreamParserException(const char *message) : msg_(message) {}

    explicit StreamParserException(const std::string &message) : msg_(message) {}

    virtual ~StreamParserException() noexcept {}

    virtual const char *what() const noexcept { return msg_.c_str(); }

   protected:
    std::string msg_;
};

template <std::size_t BUFFER_SIZE =
#if defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__) || defined(__MACH__)
#    if defined(__APPLE__) || defined(__MACH__)
              256
#    else
              1024
#    endif
#else
              256
#endif
          >
class StreamParser {
   public:
    // Exception Class

    StreamParser(std::istream &stream) : stream_buffer(stream) {}

    void readGames(Visitor &vis) {
        visitor = &vis;

        if (!stream_buffer.fill()) {
            return;
        }

        while (auto c = stream_buffer.some()) {
            if (in_header) {
                visitor->skipPgn(false);

                if (*c == '[') {
                    visitor->startPgn();
                    pgn_end = false;

                    processHeader();
                }

            } else if (in_body) {
                processBody();
            }

            if (!dont_advance_after_body) stream_buffer.advance();
            dont_advance_after_body = false;
        }

        if (!pgn_end) {
            onEnd();
        }
    }

   private:
    class LineBuffer {
       public:
        bool empty() const noexcept { return index_ == 0; }

        void clear() noexcept { index_ = 0; }

        std::string_view get() const noexcept { return std::string_view(buffer_.data(), index_); }

        void operator+=(char c) {
            assert(index_ < N);
            buffer_[index_] = c;
            ++index_;
        }

        void remove_suffix(std::size_t n) {
#ifndef CHESS_NO_EXCEPTIONS
            if (n > index_) {
                throw StreamParserException("LineBuffer underflow");
            }
#endif

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
        static constexpr std::size_t N = BUFFER_SIZE;
        using BufferType               = std::array<char, N * N>;

       public:
        StreamBuffer(std::istream &stream) : stream_(stream) {}

        // Get the current character, skip carriage returns
        std::optional<char> some() {
            if (buffer_index_ < bytes_read_) {
                const auto c = buffer_[buffer_index_];

                if (c == '\r') {
                    ++buffer_index_;
                    return some();
                }

                return c;
            } else {
                if (!fill()) {
                    return std::nullopt;
                }

                return some();
            }
        }

        // Assume that the current character is already the opening_delim
        bool skipUntil(char open_delim, char close_delim) {
            int stack = 0;

            while (true) {
                const auto ret = some();
                advance();

                if (!ret.has_value()) {
                    return false;
                }

                if (*ret == open_delim) {
                    ++stack;
                } else if (*ret == close_delim) {
                    if (stack == 0) {
                        // Mismatched closing delimiter
                        return false;
                    } else {
                        --stack;
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

        bool fill() {
            if (!stream_.good()) return false;

            buffer_index_ = 0;

            stream_.read(buffer_.data(), N * N);
            bytes_read_ = stream_.gcount();

            return bytes_read_ > 0;
        }

        void advance() {
            if (buffer_index_ >= bytes_read_) {
                fill();
            }

            ++buffer_index_;
        }

        char peek() {
            if (buffer_index_ + 1 >= bytes_read_) {
                return stream_.peek();
            }

            return buffer_[buffer_index_ + 1];
        }

        std::optional<char> current() {
            if (buffer_index_ >= bytes_read_) {
                return fill() ? std::optional<char>(buffer_[buffer_index_]) : std::nullopt;
            }

            return buffer_[buffer_index_];
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

        in_header = true;
        in_body   = false;
    }

    void callVisitorMoveFunction() {
        if (!move.empty()) {
            if (!visitor->skip()) visitor->move(move.get(), comment.get());

            move.clear();
            comment.clear();
        }
    }

    void processHeader() {
        bool backslash = false;

        while (auto c = stream_buffer.some()) {
            switch (*c) {
                // tag start
                case '[':
                    stream_buffer.advance();

                    while (auto k = stream_buffer.some()) {
                        if (is_space(*k)) {
                            break;
                        } else {
                            header.first += *k;
                            stream_buffer.advance();
                        }
                    }

                    stream_buffer.advance();
                    break;
                case '"':
                    stream_buffer.advance();

                    while (auto k = stream_buffer.some()) {
                        if (*k == '\\') {
                            backslash = true;
                            // don't add backslash to header, is this really correct?
                            stream_buffer.advance();
                        } else if (*k == '"' && !backslash) {
                            stream_buffer.advance();

                            // we should be now at ]
                            assert(stream_buffer.current().value() == ']');
                            stream_buffer.advance();

                            break;
                        }
#ifndef CHESS_NO_EXCEPTIONS
                        else if (*k == '\n') {
                            // we missed the closing quote and read until the newline character
                            // this is an invalid pgn, let's throw an error
                            throw StreamParserException("Invalid PGN, missing closing quote in header");
                        }
#endif
                        else {
                            backslash = false;
                            header.second += *k;
                            stream_buffer.advance();
                        }
                    }

                    // manually skip carriage return, otherwise we would be in the body
                    // ideally we should completely skip all carriage returns and newlines to avoid this
                    if (stream_buffer.current() == '\r') {
                        stream_buffer.advance();
                    }

                    if (!visitor->skip()) visitor->header(header.first.get(), header.second.get());

                    header.first.clear();
                    header.second.clear();

                    stream_buffer.advance();
                    break;
                case '\n':
                    in_header = false;
                    in_body   = true;

                    if (!visitor->skip()) visitor->startMoves();

                    goto exit_loop;
                default:
                    // this should normally not happen
                    // lets just go into the body, will this always be save?
                    in_header = false;
                    in_body   = true;

                    if (!visitor->skip()) visitor->startMoves();

                    goto exit_loop;
            }
        }

    exit_loop:;
    }

    void processBody() {
        auto is_termination_symbol = false;
        auto has_comment           = false;

    start:
        /*
        Skip first move number or game termination
        Also skip - * / to fix games
        which directly start with a game termination
        this https://github.com/Disservin/chess-library/issues/68
        */

        while (auto c = stream_buffer.some()) {
            if (*c == ' ' || is_digit(*c)) {
                stream_buffer.advance();
            } else if (*c == '-' || *c == '*' || c == '/') {
                is_termination_symbol = true;
                stream_buffer.advance();
            } else if (*c == '{') {
                has_comment = true;

                // reading comment
                stream_buffer.advance();

                while (auto k = stream_buffer.some()) {
                    stream_buffer.advance();

                    if (*k == '}') {
                        break;
                    }

                    comment += *k;
                }

                // the game has no moves, but a comment followed by a game termination
                if (!visitor->skip()) {
                    visitor->move("", comment.get());

                    comment.clear();
                }
            } else {
                break;
            }
        }

        // we need to reparse the termination symbol
        if (has_comment && !is_termination_symbol) {
            goto start;
        }

        // game had no moves, so we can skip it and call endPgn
        if (is_termination_symbol) {
            onEnd();
            return;
        }

        while (auto c = stream_buffer.some()) {
            if (is_space(*c)) {
                stream_buffer.advance();
                continue;
            }

            break;
        }

        while (auto cd = stream_buffer.some()) {
            // Pgn are build up in the following way.
            // {move_number} {move} {comment} {move} {comment} {move_number} ...
            // So we need to skip the move_number then start reading the move, then save the comment
            // then read the second move in the group. After that a move_number will follow again.

            // [ is unexpected here, it probably is a new pgn and the current one is finished
            if (*cd == '[') {
                onEnd();
                dont_advance_after_body = true;
                // break;
                break;
            }

            // skip move number digits
            while (auto c = stream_buffer.some()) {
                if (is_space(*c) || is_digit(*c)) {
                    stream_buffer.advance();
                } else {
                    break;
                }
            }

            // skip dots
            while (auto c = stream_buffer.some()) {
                if (*c == '.') {
                    stream_buffer.advance();
                } else {
                    break;
                }
            }

            // skip spaces
            while (auto c = stream_buffer.some()) {
                if (is_space(*c)) {
                    stream_buffer.advance();
                } else {
                    break;
                }
            }

            // parse move
            if (parseMove()) {
                break;
            }

            // skip spaces
            while (auto c = stream_buffer.some()) {
                if (is_space(*c)) {
                    stream_buffer.advance();
                } else {
                    break;
                }
            }

            // game termination
            auto curr = stream_buffer.current();

            if (!curr.has_value()) {
                onEnd();
                break;
            }

            // game termination
            if (*curr == '*') {
                onEnd();
                stream_buffer.advance();

                break;
            }

            const auto peek = stream_buffer.peek();

            if (*curr == '1') {
                if (peek == '-') {
                    stream_buffer.advance();
                    stream_buffer.advance();

                    onEnd();
                    break;
                } else if (peek == '/') {
                    for (size_t i = 0; i <= 6; ++i) {
                        stream_buffer.advance();
                    }

                    onEnd();
                    break;
                }
            }

            // might be 0-1 (game termination) or 0-0-0/0-0 (castling)
            if (*curr == '0' && stream_buffer.peek() == '-') {
                stream_buffer.advance();
                stream_buffer.advance();

                const auto c = stream_buffer.current();
                if (!c.has_value()) {
                    onEnd();

                    break;
                }

                // game termination
                if (*c == '1') {
                    onEnd();
                    stream_buffer.advance();

                    break;
                }
                // castling
                else {
                    move += '0';
                    move += '-';

                    if (parseMove()) {
                        stream_buffer.advance();
                        break;
                    }
                }
            }
        }
    }

    bool parseMove() {
        // reading move
        while (auto c = stream_buffer.some()) {
            if (is_space(*c)) {
                break;
            }

            move += *c;

            stream_buffer.advance();
        }

        return parseMoveAppendix();
    }

    bool parseMoveAppendix() {
        while (true) {
            auto curr = stream_buffer.current();

            if (!curr.has_value()) {
                onEnd();
                return true;
            }

            switch (*curr) {
                case '{': {
                    // reading comment
                    stream_buffer.advance();

                    while (auto c = stream_buffer.some()) {
                        stream_buffer.advance();

                        if (*c == '}') {
                            break;
                        }

                        comment += *c;
                    }

                    break;
                }
                case '(': {
                    stream_buffer.skipUntil('(', ')');
                    break;
                }
                case '$': {
                    while (auto c = stream_buffer.some()) {
                        if (is_space(*c)) {
                            break;
                        }

                        stream_buffer.advance();
                    }

                    break;
                }
                case ' ': {
                    while (auto c = stream_buffer.some()) {
                        if (!is_space(*c)) {
                            break;
                        }

                        stream_buffer.advance();
                    }

                    break;
                }
                default:
                    callVisitorMoveFunction();
                    return false;
            }
        }
    }

    void onEnd() {
        callVisitorMoveFunction();
        visitor->endPgn();
        visitor->skipPgn(false);

        reset_trackers();

        pgn_end = true;
    }

    bool is_space(const char c) noexcept {
        switch (c) {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                return true;
            default:
                return false;
        }
    }

    bool is_digit(const char c) noexcept {
        switch (c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return true;
            default:
                return false;
        }
    }

    StreamBuffer stream_buffer;

    Visitor *visitor = nullptr;

    // one time allocations
    std::pair<LineBuffer, LineBuffer> header = {LineBuffer{}, LineBuffer{}};

    LineBuffer move    = {};
    LineBuffer comment = {};

    // State

    bool in_header = true;
    bool in_body   = false;

    bool pgn_end = true;

    bool dont_advance_after_body = false;
};
}  // namespace chess::pgn
