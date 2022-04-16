#pragma once

#include "token.hpp"

namespace karmac {
    class SimpleToken final : public Token {
    private:
        TokenType _type;

    public:
        SimpleToken(TokenType type, LineOffset line_offset) noexcept : _type(type), Token(line_offset) {}
        SimpleToken(TokenType type, const TextIterator& iterator) noexcept : _type(type), Token(iterator) {}

        [[nodiscard]] TokenType get_type() const noexcept final { return _type; }
        [[nodiscard]] std::string_view to_string() const noexcept final { return token_type::to_string(_type); }
    };
}