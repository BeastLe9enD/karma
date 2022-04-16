#pragma once

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
}