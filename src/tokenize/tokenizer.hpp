#pragma once

#include "token.hpp"
#include <stack>
#include <vector>

namespace karmac {
    class Utf8Iterator;
    class Tokenizer final {
    private:
        std::stack<uint64_t> _pending_tokens;
        std::vector<Token*> _tokens;

        void parse_identifier(uint64_t unicode, Utf8Iterator& iterator) noexcept;
        void parse_number(uint64_t unicode, Utf8Iterator& iterator);
        void parse_string(Utf8Iterator& iterator);
        void parse_line_comment(Utf8Iterator& iterator);
        void parse_multiline_comment(Utf8Iterator& iterator);
        void parse_operator(uint64_t unicode, Utf8Iterator& iterator);
    public:
        Tokenizer(const std::string_view& source);

        [[nodiscard]] inline const std::vector<Token*>& get_tokens() const noexcept {
            return _tokens;
        }
    };
}