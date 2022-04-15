#pragma once

#include "token_type.hpp"
#include "../util/text/line_offset.hpp"
#include "../util/text/utf8_iterator.hpp"
#include <string>

namespace karmac {
    class Token {
    protected:
        LineOffset _line_offset;
    public:
        Token(LineOffset line_offset) noexcept : _line_offset(line_offset) {}
        explicit Token(const Utf8Iterator& iterator) noexcept : _line_offset(iterator.get_line_offset()) {}

        [[nodiscard]] virtual TokenType get_type() const noexcept = 0;
        [[nodiscard]] virtual std::string_view to_string() const noexcept = 0;

        [[nodiscard]] inline LineOffset get_line_offset() const noexcept { return _line_offset; }
    };

    class SimpleToken final : public Token {
    private:
        TokenType _type;

    public:
        SimpleToken(TokenType type, LineOffset line_offset) noexcept : _type(type), Token(line_offset) {}
        SimpleToken(TokenType type, const Utf8Iterator& iterator) noexcept : _type(type), Token(iterator) {}

        [[nodiscard]] TokenType get_type() const noexcept final { return _type; }
        [[nodiscard]] std::string_view to_string() const noexcept final { return token_type::to_string(_type); }
    };

    class IdentifierToken final : public Token {
    private:
        std::string _identifier;

    public:
        IdentifierToken(std::string identifier, LineOffset line_offset) noexcept
            : _identifier(std::move(identifier)), Token(line_offset) {}
        IdentifierToken(std::string identifier, const Utf8Iterator& iterator) noexcept
            : _identifier(std::move(identifier)), Token(iterator) {}

        [[nodiscard]] TokenType get_type() const noexcept final { return TokenType::Identifier; }
        [[nodiscard]] std::string_view to_string() const noexcept final { return _identifier; }
    };

    template<typename T, TokenType Type>
    class LiteralToken : public Token {
    private:
        T _value;
        std::string _value_str;

    public:
        LiteralToken(T value, LineOffset line_offset) noexcept : _value(value), _value_str(std::to_string(value)), Token(line_offset) {}
        LiteralToken(T value, const Utf8Iterator& iterator) noexcept : _value(value), _value_str(std::to_string(value)), Token(iterator) {}

        [[nodiscard]] TokenType get_type() const noexcept final { return Type; }

        [[nodiscard]] std::string_view to_string() const noexcept final {
            return _value_str;
        }
    };

    class StringLiteralToken : public Token {
    private:
        std::string _value;

    public:
        StringLiteralToken(std::string value, LineOffset line_offset) noexcept : _value(std::move(value)), Token(line_offset) {}
        StringLiteralToken(std::string value, const Utf8Iterator& iterator) noexcept : _value(std::move(value)), Token(iterator) {}

        [[nodiscard]] TokenType get_type() const noexcept final { return TokenType::StringLiteral; }

        [[nodiscard]] std::string_view to_string() const noexcept final {
            return _value;
        }
    };

    using U8LiteralToken = LiteralToken<uint8_t, TokenType::U8Literal>;
    using I8LiteralToken = LiteralToken<int8_t, TokenType::I8Literal>;
    using U16LiteralToken = LiteralToken<uint16_t, TokenType::U16Literal>;
    using I16LiteralToken = LiteralToken<int16_t, TokenType::I16Literal>;
    using U32LiteralToken = LiteralToken<uint32_t, TokenType::U32Literal>;
    using I32LiteralToken = LiteralToken<int32_t, TokenType::I32Literal>;
    using U64LiteralToken = LiteralToken<uint64_t, TokenType::U64Literal>;
    using I64LiteralToken = LiteralToken<int64_t, TokenType::I64Literal>;
    using USizeLiteralToken = LiteralToken<size_t, TokenType::USizeLiteral>;
    using ISizeLiteralToken = LiteralToken<ptrdiff_t, TokenType::ISizeLiteral>;
    using F32LiteralToken = LiteralToken<float, TokenType::F32Literal>;
    using F64LiteralToken = LiteralToken<double, TokenType::F64Literal>;
}
