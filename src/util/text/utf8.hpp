#pragma once

#include <cstdint>

namespace karmac::utf8 {
    [[nodiscard]] size_t num_chars(const char* p);
    [[nodiscard]] size_t num_chars(uint64_t unicode);

    [[nodiscard]] uint64_t to_unicode(const char* p);
    void from_unicode(uint64_t unicode, char* buffer);
}