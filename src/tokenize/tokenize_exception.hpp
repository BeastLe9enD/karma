#pragma once

#include "../util/text/text_iterator.hpp"

#include <fmt/format.h>
#include <exception>
#include <utility>

namespace karmac {
    class TokenizeException final : public std::exception {
    private:
        TextIterator _iterator;
        LineOffset _line_offset;
        std::string _message;
    public:
        template<typename... T>
        TokenizeException(TextIterator iterator, const fmt::format_string<T...> fmt, T&&... args)
                : _iterator(std::move(iterator)), _line_offset(_iterator.get_line_offset()), _message(fmt::format(fmt, std::forward<T&&>(args)...)) {}

        template<typename... T>
        TokenizeException(TextIterator iterator, const LineOffset& line_offset, const fmt::format_string<T...> fmt, T&&... args)
            : _iterator(std::move(iterator)), _line_offset(line_offset), _message(fmt::format(fmt, std::forward<T&&>(args)...)) {}

        [[nodiscard]] std::string show() const;

        [[nodiscard]] inline const LineOffset& get_line_offset() const noexcept {
            return _line_offset;
        }

        [[nodiscard]] inline const std::string& get_message() const noexcept {
            return _message;
        }
    };
}