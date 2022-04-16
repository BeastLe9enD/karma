#pragma once

#include <cstdint>

namespace karmac::utf8 {
    [[nodiscard]] size_t num_chars(const char* p);
    [[nodiscard]] size_t num_chars(uint64_t unicode);

    [[nodiscard]] uint64_t to_unicode(const char* p, size_t& len);
    [[nodiscard]] inline uint64_t to_unicode(const char* p) {
        size_t len;
        return to_unicode(p, len);
    }

    void from_unicode(uint64_t unicode, char* buffer, size_t& len);
    inline void from_unicode(uint64_t unicode, char* buffer) {
        size_t len;
        from_unicode(unicode, buffer, len);
    }
}