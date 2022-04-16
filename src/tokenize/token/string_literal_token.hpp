#pragma once

#include "token.hpp"

namespace karmac {
    class StringLiteralToken : public Token {
    private:
        std::string _value;

    public:
        StringLiteralToken(std::string value, LineOffset line_offset) noexcept : _value(std::move(value)), Token(line_offset) {}
        StringLiteralToken(std::string value, const TextIterator& iterator) noexcept : _value(std::move(value)), Token(iterator) {}

        [[nodiscard]] TokenType get_type() const noexcept final { return TokenType::StringLiteral; }
        [[nodiscard]] std::string_view to_string() const noexcept final { return _value; }
    };
}