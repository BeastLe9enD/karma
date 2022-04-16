#pragma once

#include "token.hpp"
#include <string>

namespace karmac {
    template<typename T, TokenType Type>
    class LiteralToken : public Token {
    private:
        T _value;
        std::string _value_str;

    public:
        LiteralToken(T value, LineOffset line_offset) noexcept : _value(value), _value_str(std::to_string(value)), Token(line_offset) {}
        LiteralToken(T value, const TextIterator& iterator) noexcept : _value(value), _value_str(std::to_string(value)), Token(iterator) {}

        [[nodiscard]] TokenType get_type() const noexcept final { return Type; }

        [[nodiscard]] std::string_view to_string() const noexcept final {
            return _value_str;
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