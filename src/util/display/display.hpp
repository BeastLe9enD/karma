#pragma once

#include "../text/line_offset.hpp"
#include <fmt/format.h>

namespace karmac::display {
    namespace detail {
        inline void error_at_line(LineOffset line_offset, const std::string& message);
    }

    template<typename... T>
    inline void error_at_line(LineOffset line_offset, const fmt::format_string<T...> fmt, T&&... args) {
        detail::error_at_line(line_offset, fmt::format(fmt, std::forward<T&&>(args)...));
    }
}