#pragma once

namespace karmac {
    struct LineOffset {
    public:
        size_t line;
        size_t offset;

    public:
        LineOffset() noexcept : line(0), offset(0) {}
        LineOffset(size_t line, size_t offset) noexcept
            : line(line), offset(offset) {}

        [[nodiscard]] inline bool operator ==(const LineOffset& other) const noexcept {
            return line == other.line && offset == other.offset;
        }

        [[nodiscard]] inline bool operator !=(const LineOffset& other) const noexcept {
            return line != other.line || offset != other.offset;
        }
    };
}