#pragma once

#include "token/token.hpp"
#include <stack>
#include <vector>

namespace karmac {
    class TextIterator;
    class Tokenizer final {
    private:
        std::stack<TokenType> _pending_tokens;
        std::vector<Token*> _tokens;

        TextIterator _iterator;

        void skip_whitespace();
        void parse_line_comment();
        void parse_multiline_comment();
        void parse_string_literal();

        void parse_identifier();
        [[nodiscard]] bool try_parse_number();
        [[nodiscard]] bool try_parse_atom();
    public:
        explicit Tokenizer(const std::string_view& source);
        ~Tokenizer();

        [[nodiscard]] inline const std::vector<Token*>& get_tokens() const noexcept {
            return _tokens;
        }
    };
}