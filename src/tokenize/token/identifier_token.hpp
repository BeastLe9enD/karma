#pragma once

#include "token.hpp"

namespace karmac {
    class IdentifierToken final : public Token {
    private:
        std::string _identifier;

    public:
        IdentifierToken(std::string identifier, LineOffset line_offset) noexcept
                : _identifier(std::move(identifier)), Token(line_offset) {}
        IdentifierToken(std::string identifier, const TextIterator& iterator) noexcept
                : _identifier(std::move(identifier)), Token(iterator) {}

        [[nodiscard]] TokenType get_type() const noexcept final { return TokenType::Identifier; }
        [[nodiscard]] std::string_view to_string() const noexcept final { return _identifier; }
    };
}