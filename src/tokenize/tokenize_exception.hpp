#pragma once

#include "../util/text/utf8_iterator.hpp"

#include <fmt/format.h>
#include <exception>

namespace karmac {
    class TokenizeException final : public std::exception {
    private:
        Utf8Iterator _iterator;
        LineOffset _line_offset;
        std::string _message;
    public:
        template<typename... T>
        TokenizeException(const LineOffset& line_offset, const fmt::format_string<T...> fmt, T&&... args)
            : _line_offset(line_offset), _message(fmt::format(fmt, std::forward<T&&>(args)...)) {}



        [[nodiscard]] inline const LineOffset& get_line_offset() const noexcept {
            return _line_offset;
        }

        [[nodiscard]] inline const std::string& get_message() const noexcept {
            return _message;
        }
    };
}