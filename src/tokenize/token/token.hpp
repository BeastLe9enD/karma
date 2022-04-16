#pragma once

#include "token_type.hpp"
#include "../../util/text/text_iterator.hpp"

namespace karmac {
    class Token {
    protected:
        LineOffset _line_offset;
    public:
        Token(LineOffset line_offset) noexcept : _line_offset(line_offset) {}
        explicit Token(const TextIterator& iterator) noexcept : _line_offset(iterator.get_line_offset()) {}
        virtual ~Token() {}

        [[nodiscard]] virtual TokenType get_type() const noexcept = 0;
        [[nodiscard]] virtual std::string_view to_string() const noexcept = 0;

        [[nodiscard]] inline LineOffset get_line_offset() const noexcept { return _line_offset; }
    };
}