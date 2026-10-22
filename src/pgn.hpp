#pragma once

#include <array>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <istream>
#include <optional>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#if defined(__SSE2__) && (defined(__GNUC__) || defined(__clang__))
#    include <emmintrin.h>
#endif

namespace chess::pgn {

namespace detail {

/**
 * @brief Private class
 */
class StringBuffer {
   public:
    bool empty() const noexcept { return index_ == 0; }

    void clear() noexcept { index_ = 0; }

    std::string_view get() const noexcept { return std::string_view(buffer_.data(), index_); }

    bool add(char c) {
        if (index_ >= N) {
            return false;
        }

        buffer_[index_] = c;

        ++index_;

        return true;
    }

    bool add(const char* data, std::size_t size) {
        if (size > N - index_) return false;
        std::memcpy(buffer_.data() + index_, data, size);
        index_ += size;
        return true;
    }

   private:
    // PGN String Tokens are limited to 255 characters
    static constexpr int N = 255;

    std::array<char, N> buffer_ = {};

    std::size_t index_ = 0;
};

/**
 * @brief Private class
 * @tparam BUFFER_SIZE
 */
template <std::size_t BUFFER_SIZE>
class StreamBuffer {
   private:
    static constexpr std::size_t N = BUFFER_SIZE;
    using BufferType               = std::array<char, N * N>;

   public:
    StreamBuffer(std::istream& stream) : stream_(stream) {}

    // Get the current character, skip carriage returns
    std::optional<char> some() {
        while (true) {
            if (buffer_index_ < bytes_read_) {
                const auto c = buffer_[buffer_index_];

                if (c == '\r') {
                    ++buffer_index_;
                    continue;
                }

                return c;
            }

            if (!fill()) {
                return std::nullopt;
            }
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

    std::string_view comment() const noexcept {
        if (comment_borrowed_) return {buffer_.data() + comment_offset_, comment_length_};
        return comment_storage_;
    }

    void clearComment() noexcept {
        if (comment_borrowed_) {
            comment_borrowed_ = false;
        } else {
            comment_storage_.clear();
        }
    }

    // Borrow contiguous comments until the callback. Copy only when a comment
    // spans refills, contains carriage returns, or follows another comment.
    void readComment() {
        while (true) {
            if (buffer_index_ >= bytes_read_ && !fill()) return;
            const auto begin = buffer_index_;
            buffer_index_ = delimiterIndex<TokenKind::Comment>();
            const auto size = static_cast<std::size_t>(buffer_index_ - begin);
            if (size != 0) {
                if (!comment_borrowed_ && comment_storage_.empty()) {
                    comment_offset_ = static_cast<std::size_t>(begin);
                    comment_length_ = size;
                    comment_borrowed_ = true;
                } else {
                    preserveComment();
                    comment_storage_.append(buffer_.data() + begin, size);
                }
            }
            if (buffer_index_ < bytes_read_) {
                const auto delimiter = buffer_[buffer_index_];
                // Reuse the consumed delimiter to terminate a borrowed span.
                buffer_[buffer_index_++] = '\0';
                if (delimiter == '}') return;
            }
        }
    }

    enum class TokenKind { Move, HeaderKey, HeaderValue, Comment };

    template <TokenKind kind>
    bool readTokenUntil(StringBuffer& token) {
        while (true) {
            if (buffer_index_ >= bytes_read_ && !fill()) return true;
            const auto begin = buffer_index_;
            buffer_index_ = delimiterIndex<kind>();
            if (!token.add(buffer_.data() + begin, static_cast<std::size_t>(buffer_index_ - begin))) return false;
            if (buffer_index_ < bytes_read_) {
                if (buffer_[buffer_index_] != '\r') return true;
                ++buffer_index_;
            }
        }
    }

    bool readMoveToken(StringBuffer& token) {
        return readTokenUntil<TokenKind::Move>(token);
    }

    bool fill() {
        preserveComment();
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
    template <TokenKind kind>
    std::streamsize delimiterIndex() const {
        auto index = buffer_index_;
#if defined(__SSE2__) && (defined(__GNUC__) || defined(__clang__))
        if constexpr (kind == TokenKind::Comment || kind == TokenKind::HeaderValue) {
            while (bytes_read_ - index >= 16) {
                const auto bytes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(buffer_.data() + index));
                auto delimiters = _mm_cmpeq_epi8(bytes, _mm_set1_epi8('\r'));
                if constexpr (kind == TokenKind::Comment) {
                    delimiters = _mm_or_si128(delimiters, _mm_cmpeq_epi8(bytes, _mm_set1_epi8('}')));
                } else {
                    delimiters = _mm_or_si128(delimiters, _mm_cmpeq_epi8(bytes, _mm_set1_epi8('\n')));
                    delimiters = _mm_or_si128(delimiters, _mm_cmpeq_epi8(bytes, _mm_set1_epi8('"')));
                    delimiters = _mm_or_si128(delimiters, _mm_cmpeq_epi8(bytes, _mm_set1_epi8('\\')));
                }
                const auto mask = static_cast<unsigned>(_mm_movemask_epi8(delimiters));
                if (mask != 0) return index + __builtin_ctz(mask);
                index += 16;
            }
        }
#endif
        while (index < bytes_read_) {
            const auto c = buffer_[index];
            if (c == '\r') break;
            if constexpr (kind == TokenKind::Comment) {
                if (c == '}') break;
            } else {
                if (c == '\n') break;
                if constexpr (kind == TokenKind::HeaderValue) {
                    if (c == '"' || c == '\\') break;
                } else {
                    if (c == ' ' || c == '\t') break;
                    if constexpr (kind == TokenKind::Move) {
                        if (c == '(' || c == ')') break;
                    }
                }
            }
            ++index;
        }
        return index;
    }

    void preserveComment() {
        if (comment_borrowed_) {
            comment_storage_.assign(buffer_.data() + comment_offset_, comment_length_);
            comment_borrowed_ = false;
        }
    }

    std::string comment_storage_;
    std::size_t comment_offset_ = 0;
    std::size_t comment_length_ = 0;
    bool comment_borrowed_ = false;

    std::istream& stream_;
    BufferType buffer_;
    std::streamsize bytes_read_   = 0;
    std::streamsize buffer_index_ = 0;
};

}  // namespace detail

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

    /** @brief Called when a parenthesized side variation begins. */
    virtual void startVariation() {}

    /** @brief Called when a parenthesized side variation ends. */
    virtual void endVariation() {}

    /**
     * @brief Called when a game ends
     */
    virtual void endPgn() = 0;

   private:
    bool skip_ = false;
};

class StreamParserError {
   public:
    enum Code {
        None,
        ExceededMaxStringLength,
        InvalidHeaderMissingClosingBracket,
        InvalidHeaderMissingClosingQuote,
        NotEnoughData
    };

    StreamParserError() : code_(None) {}

    StreamParserError(Code code) : code_(code) {}

    Code code() const { return code_; }

    bool hasError() const { return code_ != None; }

    std::string message() const {
        switch (code_) {
            case None:
                return "No error";
            case InvalidHeaderMissingClosingBracket:
                return "Invalid header: missing closing bracket";
            case InvalidHeaderMissingClosingQuote:
                return "Invalid header: missing closing quote";
            case NotEnoughData:
                return "Not enough data";
            default:
                assert(false);
                return "Unknown error";
        }
    }

    bool operator==(Code code) const { return code_ == code; }
    bool operator!=(Code code) const { return code_ != code; }
    bool operator==(const StreamParserError& other) const { return code_ == other.code_; }
    bool operator!=(const StreamParserError& other) const { return code_ != other.code_; }

    operator bool() const { return code_ != None; }

   private:
    Code code_;
};

template <std::size_t BUFFER_SIZE =
#if defined(__APPLE__) || defined(__MACH__)
              256
#elif defined(__unix__) || defined(__unix) || defined(unix)
              1024
#else
              256
#endif
          >
class StreamParser {
    using TokenKind = typename detail::StreamBuffer<BUFFER_SIZE>::TokenKind;

   public:
    StreamParser(std::istream& stream) : stream_buffer(stream) {}

    /**
     * @brief Enable or disable parsing parenthesized side variations.
     * Variations are disabled by default to preserve mainline-only behavior.
     */
    void setParseVariations(bool enabled) { parse_variations = enabled; }

    bool readNextGame(Visitor& vis) {
        visitor = &vis;
        if (parser_error != StreamParserError::None) return false;
        bool game_started = !pgn_end;

        while (true) {
            auto c = stream_buffer.some();
            if (!c.has_value()) {
                if (!pgn_end) {
                    onEnd();
                    return true;
                }
                return false;
            }

            if (in_header) {
                visitor->skipPgn(false);
                if (*c == '[') {
                    visitor->startPgn();
                    pgn_end = false;
                    game_started = true;
                    processHeader();
                }
            } else if (in_body) {
                processBody();
            }

            if (parser_error != StreamParserError::None) return false;
            if (!dont_advance_after_body) stream_buffer.advance();
            dont_advance_after_body = false;
            if (game_started && pgn_end) return true;
        }
    }

    const StreamParserError& error() const { return parser_error; }

    StreamParserError readGames(Visitor& vis) {
        visitor = &vis;

        if (!stream_buffer.fill()) {
            return StreamParserError::NotEnoughData;
        }

        while (auto c = stream_buffer.some()) {
            if (in_header) {
                visitor->skipPgn(false);

                if (*c == '[') {
                    visitor->startPgn();
                    pgn_end = false;

                    processHeader();

                    if (parser_error != StreamParserError::None) {
                        return parser_error;
                    }
                }

            } else if (in_body) {
                processBody();

                if (parser_error != StreamParserError::None) {
                    return parser_error;
                }
            }

            if (!dont_advance_after_body) stream_buffer.advance();
            dont_advance_after_body = false;
        }

        if (!pgn_end) {
            onEnd();
        }

        return parser_error;
    }

   private:
    void reset_trackers() {
        header.first.clear();
        header.second.clear();

        move.clear();
        stream_buffer.clearComment();
        variation_depth = 0;

        in_header = true;
        in_body   = false;
    }

    void callVisitorMoveFunction() {
        if (!move.empty()) {
            if (!visitor->skip()) visitor->move(move.get(), stream_buffer.comment());

            move.clear();
            stream_buffer.clearComment();
        }
    }

    void processHeader() {
        bool backslash = false;

        while (auto c = stream_buffer.some()) {
            switch (*c) {
                // tag start
                case '[':
                    stream_buffer.advance();

                    if (!stream_buffer.template readTokenUntil<TokenKind::HeaderKey>(header.first)) {
                        parser_error = StreamParserError::ExceededMaxStringLength;
                        return;
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
                            if (stream_buffer.current().value_or('\0') != ']') {
                                parser_error = StreamParserError::InvalidHeaderMissingClosingBracket;
                                return;
                            }

                            stream_buffer.advance();

                            break;
                        } else if (*k == '\n') {
                            // we missed the closing quote and read until the newline character
                            // this is an invalid pgn, let's throw an error
                            parser_error = StreamParserError::InvalidHeaderMissingClosingQuote;
                            return;
                        } else {
                            backslash = false;

                            if (*k == '"') {
                                if (!header.second.add(*k)) {
                                    parser_error = StreamParserError::ExceededMaxStringLength;
                                    return;
                                }
                                stream_buffer.advance();
                            } else if (!stream_buffer.template readTokenUntil<TokenKind::HeaderValue>(header.second)) {
                                parser_error = StreamParserError::ExceededMaxStringLength;
                                return;
                            }
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

                    return;
                default:
                    // this should normally not happen
                    // lets just go into the body, will this always be save?
                    in_header = false;
                    in_body   = true;

                    if (!visitor->skip()) visitor->startMoves();

                    return;
            }
        }
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
            } else if (*c == '-' || *c == '*' || *c == '/') {
                is_termination_symbol = true;
                stream_buffer.advance();
            } else if (*c == '{') {
                has_comment = true;

                // reading comment
                stream_buffer.advance();

                stream_buffer.readComment();

                // the game has no moves, but a comment followed by a game termination
                if (!visitor->skip()) {
                    visitor->move("", stream_buffer.comment());

                    has_comment = false;
                    stream_buffer.clearComment();
                }
            } else {
                break;
            }
        }

        // we need to reparse the termination symbol
        if (!visitor->skip() && has_comment && !is_termination_symbol) {
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

            // Side variation delimiters can be adjacent to move tokens.
            if (*cd == '(' || *cd == ')') {
                if (*cd == '(') {
                    if (parse_variations) {
                        callVisitorMoveFunction();
                        ++variation_depth;
                        if (!visitor->skip()) visitor->startVariation();
                    } else {
                        stream_buffer.skipUntil('(', ')');
                        continue;
                    }
                } else {
                    callVisitorMoveFunction();
                    if (variation_depth > 0) {
                        --variation_depth;
                        if (!visitor->skip()) visitor->endVariation();
                    }
                }
                stream_buffer.advance();
                continue;
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
            if (*curr == '*' && variation_depth == 0) {
                onEnd();
                stream_buffer.advance();

                break;
            }

            const auto peek = stream_buffer.peek();

            if (*curr == '1' && variation_depth == 0) {
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
            if (*curr == '0' && variation_depth == 0 && stream_buffer.peek() == '-') {
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
                    if (!move.add('0') || !move.add('-')) {
                        parser_error = StreamParserError::ExceededMaxStringLength;
                        return;
                    }

                    if (parseMove()) {
                        stream_buffer.advance();
                        break;
                    }
                }
            }
        }
    }

    bool parseMove() {
        if (!stream_buffer.readMoveToken(move)) {
            parser_error = StreamParserError::ExceededMaxStringLength;
            return true;
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

                    stream_buffer.readComment();

                    break;
                }
                case '(': {
                    if (parse_variations) {
                        callVisitorMoveFunction();
                        ++variation_depth;
                        if (!visitor->skip()) visitor->startVariation();
                        stream_buffer.advance();
                    } else {
                        stream_buffer.skipUntil('(', ')');
                    }
                    break;
                }
                case ')': {
                    callVisitorMoveFunction();
                    if (variation_depth > 0) {
                        --variation_depth;
                        if (!visitor->skip()) visitor->endVariation();
                    }
                    stream_buffer.advance();
                    break;
                }
                case '$': {
                    while (auto c = stream_buffer.some()) {
                        if (is_space(*c) || *c == '(' || *c == ')') {
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

    detail::StreamBuffer<BUFFER_SIZE> stream_buffer;

    Visitor* visitor = nullptr;

    // one time allocations
    std::pair<detail::StringBuffer, detail::StringBuffer> header = {detail::StringBuffer{}, detail::StringBuffer{}};

    detail::StringBuffer move = {};

    // State

    StreamParserError parser_error = StreamParserError::None;

    bool in_header = true;
    bool in_body   = false;

    bool pgn_end = true;

    bool dont_advance_after_body = false;
    std::size_t variation_depth = 0;
    bool parse_variations = false;
};

enum class GameEventType { Move, VariationStart, VariationEnd };

struct GameEvent {
    GameEventType type;
    std::string san;
    std::string comment;
};

/// An owning PGN game. Events retain move order and variation nesting.
struct Game {
    std::vector<std::pair<std::string, std::string>> headers;
    std::vector<GameEvent> events;
};

namespace detail {
class GameCollector final : public Visitor {
   public:
    void startPgn() override { game = {}; }
    void header(std::string_view key, std::string_view value) override {
        game.headers.emplace_back(key, value);
    }
    void startMoves() override {}
    void move(std::string_view san, std::string_view comment) override {
        game.events.push_back({GameEventType::Move, std::string(san), std::string(comment)});
    }
    void startVariation() override { game.events.push_back({GameEventType::VariationStart, {}, {}}); }
    void endVariation() override { game.events.push_back({GameEventType::VariationEnd, {}, {}}); }
    void endPgn() override { ready = true; }

    Game game;
    bool ready = false;
};
}  // namespace detail

/// A single-pass range that parses one owning game at a time.
template <std::size_t BUFFER_SIZE =
#if defined(__APPLE__) || defined(__MACH__)
              256
#elif defined(__unix__) || defined(__unix) || defined(unix)
              1024
#else
              256
#endif
          >
class GameRange {
   public:
    explicit GameRange(std::istream& stream) : parser_(stream) {}

    /// Enable or disable variation events. Must be called before iteration.
    void setParseVariations(bool enabled) { parser_.setParseVariations(enabled); }

    class iterator {
       public:
        using iterator_category = std::input_iterator_tag;
        using value_type = Game;
        using difference_type = std::ptrdiff_t;
        using pointer = const Game*;
        using reference = const Game&;

        iterator() = default;
        explicit iterator(GameRange* range) : range_(range) { advance(); }
        reference operator*() const { return range_->collector_.game; }
        pointer operator->() const { return &range_->collector_.game; }
        iterator& operator++() { advance(); return *this; }
        void operator++(int) { advance(); }
        friend bool operator==(const iterator& lhs, const iterator& rhs) {
            return (lhs.end_ && rhs.end_) || (lhs.range_ == rhs.range_ && lhs.end_ == rhs.end_);
        }
        friend bool operator!=(const iterator& lhs, const iterator& rhs) { return !(lhs == rhs); }

       private:
        void advance() {
            if (!range_ || range_->finished_) { end_ = true; return; }
            range_->collector_.ready = false;
            end_ = !range_->parser_.readNextGame(range_->collector_) || !range_->collector_.ready;
            if (end_) range_->finished_ = true;
        }
        GameRange* range_ = nullptr;
        bool end_ = true;
    };

    iterator begin() { return iterator(this); }
    iterator end() const { return iterator(); }
    const StreamParserError& error() const { return parser_.error(); }

   private:
    StreamParser<BUFFER_SIZE> parser_;
    detail::GameCollector collector_;
    bool finished_ = false;
};

template <std::size_t BUFFER_SIZE =
#if defined(__APPLE__) || defined(__MACH__)
              256
#elif defined(__unix__) || defined(__unix) || defined(unix)
              1024
#else
              256
#endif
          >
GameRange<BUFFER_SIZE> games(std::istream& stream) {
    return GameRange<BUFFER_SIZE>(stream);
}
}  // namespace chess::pgn
