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

        Lexer lexer(stream_buffer);

        while (true) {
            auto token = lexer.next();

            if (token.type() == Lexer::Token::Type::None) {
                return;
            }
        }
    }

   private:
    class LineBuffer {
       public:
        LineBuffer() = default;
        LineBuffer(std::string_view str) : index_(str.size()) {
            if (str.size() > N) {
                throw std::runtime_error("LineBuffer overflow");
            }

            std::copy(str.begin(), str.end(), buffer_.begin());
        }

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
        static constexpr std::size_t N = 1024;
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

        std::optional<bool> fill() {
            if (!stream_.good()) return std::nullopt;

            buffer_index_ = 0;

            stream_.read(buffer_.data(), N * N);
            bytes_read_ = stream_.gcount();

            return std::optional<bool>(bytes_read_ > 0);
        }

        std::optional<char> get() {
            if (buffer_index_ == bytes_read_) {
                const auto ret = fill();
                return ret.has_value() && *ret ? std::optional<char>(buffer_[buffer_index_++]) : std::nullopt;
            }

            return buffer_[buffer_index_++];
        }

        std::optional<char> peek() {
            if (buffer_index_ == bytes_read_) {
                // we cant fill the new buffer, so we just peek the stream
                const auto c = stream_.peek();
                return c == std::char_traits<char>::eof() ? std::nullopt : std::optional<char>(c);
            }

            return buffer_[buffer_index_ + 1];
        }

        std::optional<char> current() {
            if (buffer_index_ == bytes_read_) {
                const auto ret = fill();
                return ret.has_value() && *ret ? std::optional<char>(buffer_[buffer_index_]) : std::nullopt;
            }

            return buffer_[buffer_index_];
        }

       private:
        std::istream &stream_;
        BufferType buffer_;
        std::streamsize bytes_read_   = 0;
        std::streamsize buffer_index_ = 0;
    };

    class Lexer {
       public:
        Lexer(StreamBuffer &stream_buffer) : stream_buffer_(stream_buffer) {}

        class Token {
           public:
            enum class Type {
                None,
                LBracket,
                RBracket,
                LParen,
                RParen,
                LCurl,
                RCurl,
                Dot,
                Asterisk,
                Dollar,
                Number,
                Symbol,   // tag name, move
                String,   // value
                Comment,  // move comment
                // Identifier,  // tag name, move, value, move comment
                Unknown,
            };

            Token() = default;

            Token(Type type, LineBuffer value) : type_(type), value_(value) {}

            Type type() const noexcept { return type_; }

            LineBuffer &value() noexcept { return value_; }

            std::string toString() const noexcept {
                switch (type_) {
                    case Type::None:
                        return "None";
                    case Type::LBracket:
                        return "LBracket";
                    case Type::RBracket:
                        return "RBracket";
                    case Type::LParen:
                        return "LParen";
                    case Type::RParen:
                        return "RParen";
                    case Type::LCurl:
                        return "LCurl";
                    case Type::RCurl:
                        return "RCurl";
                    case Type::Dot:
                        return "Dot";
                    case Type::Asterisk:
                        return "Asterisk";
                    case Type::Dollar:
                        return "Dollar";
                    case Type::Number:
                        return "Number";
                    case Type::Symbol:
                        return "Symbol";
                    case Type::String:
                        return "String";
                    case Type::Comment:
                        return "Comment";
                    case Type::Unknown:
                        return "Unknown";
                }

                return "Unknown";
            }

           private:
            Type type_ = Type::None;
            LineBuffer value_;
        };

        Token next() noexcept {
            while (true) {
                const auto curr = stream_buffer_.current();

                if (!curr.has_value()) {
                    return Token(Token::Type::None, LineBuffer());
                }

                if (is_space(*curr)) {
                    stream_buffer_.get();
                } else {
                    break;
                }
            }

            const auto c = stream_buffer_.current();

            if (!c.has_value()) {
                return Token(Token::Type::None, LineBuffer());
            }

            switch (*c) {
                case '[':
                    stream_buffer_.get();
                    last_token_type_ = Token::Type::LBracket;
                    return Token(last_token_type_, LineBuffer("["));
                case ']':
                    stream_buffer_.get();
                    last_token_type_ = Token::Type::RBracket;
                    return Token(last_token_type_, LineBuffer("]"));
                case '(':
                    stream_buffer_.get();
                    last_token_type_ = Token::Type::LParen;
                    return Token(last_token_type_, LineBuffer("("));
                case ')':
                    stream_buffer_.get();
                    last_token_type_ = Token::Type::RParen;
                    return Token(last_token_type_, LineBuffer(")"));
                case '.':
                    stream_buffer_.get();
                    last_token_type_ = Token::Type::Dot;
                    return Token(last_token_type_, LineBuffer("."));
                case '*':
                    stream_buffer_.get();
                    last_token_type_ = Token::Type::Asterisk;
                    return Token(last_token_type_, LineBuffer("*"));
                case '$':
                    stream_buffer_.get();
                    last_token_type_ = Token::Type::Dollar;
                    return nag();
                case '{':
                    stream_buffer_.get();
                    last_token_type_ = Token::Type::LCurl;
                    return Token(last_token_type_, LineBuffer("{"));
                case '}':
                    stream_buffer_.get();
                    last_token_type_ = Token::Type::RCurl;
                    return Token(last_token_type_, LineBuffer("}"));
                case '"':
                    stream_buffer_.get();
                    last_token_type_ = Token::Type::String;
                    return string();
                default:
                    if (last_token_type_ == Token::Type::LCurl) {
                        return comment();
                    } else if (last_token_type_ == Token::Type::LParen) {
                        return variation();
                    } else if (last_token_type_ == Token::Type::LBracket) {
                        last_token_type_ = Token::Type::Symbol;
                        return symbol();
                    } else if (is_digit(*c) || is_letter(*c)) {
                        last_token_type_ = Token::Type::Symbol;
                        return symbol();
                    } else {
                        stream_buffer_.get();
                        last_token_type_ = Token::Type::Unknown;
                        return Token(last_token_type_, LineBuffer());
                    }
            }
        }

       private:
        Token variation() noexcept {
            Token token(Token::Type::Symbol, LineBuffer());

            while (true) {
                const auto c = stream_buffer_.current();

                if (!c.has_value()) {
                    return token;
                }

                if (*c != ')') {
                    stream_buffer_.get();
                } else {
                    return token;
                }
            }

            return token;
        }

        Token comment() noexcept {
            Token token(Token::Type::Comment, LineBuffer());

            while (true) {
                const auto c = stream_buffer_.current();

                if (!c.has_value()) {
                    return token;
                }

                if (*c != '}') {
                    token.value() += *c;
                    stream_buffer_.get();
                } else {
                    return token;
                }
            }

            return token;
        }

        Token nag() noexcept {
            Token token(Token::Type::Dollar, LineBuffer());

            while (true) {
                const auto c = stream_buffer_.current();

                if (!c.has_value()) {
                    return token;
                }

                if (is_digit(*c)) {
                    token.value() += *c;
                    stream_buffer_.get();
                } else {
                    return token;
                }
            }

            return token;
        }

        Token string() noexcept {
            Token token(Token::Type::String, LineBuffer());

            while (true) {
                const auto c = stream_buffer_.current();

                if (!c.has_value()) {
                    return token;
                }

                stream_buffer_.get();

                if (*c != '"') {
                    token.value() += *c;
                } else {
                    return token;
                }
            }

            return token;
        }

        Token symbol() noexcept {
            Token token(Token::Type::Symbol, LineBuffer());

            while (true) {
                const auto c = stream_buffer_.current();

                if (!c.has_value()) {
                    return token;
                }

                if (!is_space(*c)) {
                    token.value() += *c;
                    stream_buffer_.get();
                } else {
                    return token;
                }
            }

            return token;
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

        bool is_digit(char c) noexcept { return c >= '0' && c <= '9'; }

        bool is_letter(char c) noexcept { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }

        StreamBuffer &stream_buffer_;

        Token::Type last_token_type_ = Token::Type::None;
    };

    StreamBuffer stream_buffer;

    Visitor *visitor = nullptr;
};
}  // namespace chess::pgn
