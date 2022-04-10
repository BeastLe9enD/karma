#pragma once

#include <cstdlib>
#include <string>
#include <string_view>

namespace karmac {
    enum class TokenType {
        //Brackets
        LeftBracket,
        RightBracket,
        LeftSquareBracket,
        RightSquareBracket,
        LeftCurlyBracket,
        RightCurlyBracket,

        //Operators
        Dot,
        DoubleDot,
        Comma,
        Colon,
        DoubleColon,
        Semicolon,
        Assign,
        Not,
        Arrow,
        QuestionMark,
        Less,
        LessEquals,
        Greater,
        GreaterEquals,
        Equals,
        NotEquals,
        Conjunction,
        Disjunction,
        And,
        AndAssign,
        Or,
        OrAssign,
        Xor,
        XorAssign,
        LeftShift,
        LeftShiftAssign,
        RightShift,
        RightShiftAssign,
        Increment,
        Decrement,
        Add,
        AddAssign,
        Sub,
        SubAssign,
        Mul,
        MulAssign,
        Div,
        DivAssign,
        Mod,
        ModAssign,

        //Keywords
        Identifier,
        Fn,
        If,
        Else,
        For,
        While,
        Break,
        Continue,
        Return,

        //Literals
        U8Literal,
        I8Literal,
        U16Literal,
        I16Literal,
        U32Literal,
        I32Literal,
        U64Literal,
        I64Literal,
        USizeLiteral,
        ISizeLiteral,
        F32Literal,
        F64Literal,
        StringLiteral
    };

    namespace token_type {
        [[nodiscard]] std::string_view get_name(TokenType type) noexcept;
        [[nodiscard]] std::string_view to_string(TokenType type) noexcept;
    }

    class Token {
    protected:
        size_t _line;
        size_t _line_offset;
    public:
        Token(size_t line, size_t line_offset) noexcept : _line(line), _line_offset(line_offset) {}

        [[nodiscard]] virtual TokenType get_type() const noexcept = 0;
        [[nodiscard]] virtual std::string_view to_string() const noexcept = 0;

        [[nodiscard]] size_t get_line() const noexcept { return _line; }
        [[nodiscard]] size_t get_line_offset() const noexcept { return _line_offset; }
    };

    class SimpleToken final : public Token {
    private:
        TokenType _type;
        size_t _line;
        size_t _line_offset;

    public:
        SimpleToken(TokenType type, size_t line, size_t line_offset) noexcept : _type(type), Token(line, line_offset) {}

        [[nodiscard]] TokenType get_type() const noexcept final { return _type; }
        [[nodiscard]] std::string_view to_string() const noexcept final { return token_type::to_string(_type); }
    };

    class IdentifierToken final : public Token {
    private:
        std::string _identifier;

    public:
        IdentifierToken(std::string identifier, size_t line, size_t line_offset) noexcept
            : _identifier(std::move(identifier)), Token(line, line_offset) {}

        [[nodiscard]] TokenType get_type() const noexcept final { return TokenType::Identifier; }
        [[nodiscard]] std::string_view to_string() const noexcept final { return _identifier; }
    };

    template<typename T, TokenType Type>
    class LiteralToken : public Token {
    private:
        T _value;
        std::string _value_str;

    public:
        LiteralToken(T value, size_t line, size_t line_offset) noexcept : _value(value), _value_str(std::to_string(value)), Token(line, line_offset) {}

        [[nodiscard]] TokenType get_type() const noexcept final { return Type; }

        [[nodiscard]] std::string_view to_string() const noexcept final {
            return _value_str;
        }
    };

    class StringLiteralToken : public Token {
    private:
        std::string _value;

    public:
        StringLiteralToken(std::string value, size_t line, size_t line_offset) noexcept : _value(std::move(value)), Token(line, line_offset) {}

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
