#include "utf8.hpp"

#include <stdexcept>

namespace karmac::utf8 {
    //Based on https://gist.github.com/rechardchen/3321830
    size_t num_chars(const char* p) {
        const auto ch = *p;

        if((ch & 0x80) == 0) {
            return 1;
        }
        if((ch & 0xe0) == 0xc0) {
            return 2;
        }
        if((ch & 0xf0) == 0xe0) {
            return 3;
        }
        if((ch & 0xf8) == 0xf0) {
            return 4;
        }
        if((ch & 0xfc) == 0xf8) {
            return 5;
        }
        if((ch & 0xfe) == 0xfc) {
            return 6;
        }

        throw std::runtime_error("Invalid utf8!");
    }

    size_t num_chars(uint64_t unicode) {
        if(unicode >= 0 && unicode <= 0x7f) {
            return 1;
        }
        if(unicode >= 0x80 && unicode <= 0x7ff) {
            return 2;
        }
        if(unicode >= 0x800 && unicode <= 0xffff) {
            return 3;
        }
        if(unicode >= 0x10000 && unicode <= 0x1fffff) {
            return 4;
        }
        if(unicode >= 0x200000 && unicode <= 0x3ffffff) {
            return 5;
        }
        if(unicode >= 0x4000000 && unicode <= 0x7fffffff) {
            return 6;
        }

        throw std::runtime_error("Invalid unicode!");
    }

    uint64_t to_unicode(const char* p, size_t& len) {
        len = num_chars(p);

        uint64_t unicode;
        switch(len) {
            case 1:
                unicode = static_cast<uint64_t>(*p);
                break;
            case 2:
                unicode = static_cast<uint64_t>(*p++ & 0x1f) << 6;
                unicode |= static_cast<uint64_t>(*p & 0x3f);
                break;
            case 3:
                unicode = static_cast<uint64_t>(*p++ & 0xf) << 12;
                unicode |= static_cast<uint64_t>(*p++ & 0x3f) << 6;
                unicode |= static_cast<uint64_t>(*p & 0x3f);
                break;
            case 4:
                unicode = static_cast<uint64_t>(*p++ & 0x7) << 18;
                unicode |= static_cast<uint64_t>(*p++ & 0x3f) << 12;
                unicode |= static_cast<uint64_t>(*p++ & 0x3f) << 6;
                unicode |= static_cast<uint64_t>(*p & 0x3f);
                break;
            case 5:
                unicode = static_cast<uint64_t>(*p++ & 0x3) << 24;
                unicode |= static_cast<uint64_t>(*p++ & 0x3f) << 18;
                unicode |= static_cast<uint64_t>(*p++ & 0x3f) << 12;
                unicode |= static_cast<uint64_t>(*p++ & 0x3f) << 6;
                unicode |= static_cast<uint64_t>(*p & 0x3f);
                break;
            case 6:
                unicode = static_cast<uint64_t>(*p++ & 0x1) << 30;
                unicode |= static_cast<uint64_t>(*p++ & 0x3f) << 24;
                unicode |= static_cast<uint64_t>(*p++ & 0x3f) << 18;
                unicode |= static_cast<uint64_t>(*p++ & 0x3f) << 12;
                unicode |= static_cast<uint64_t>(*p++ & 0x3f) << 6;
                unicode |= static_cast<uint64_t>(*p & 0x3f);
                break;
        }

        return unicode;
    }

    void from_unicode(uint64_t unicode, char* buffer, size_t& len) {
        len = num_chars(unicode);
        buffer[len] = '\0';

        switch(len) {
            case 1:
                buffer[0] = static_cast<char>(unicode);
                break;
            case 2:
                buffer[0] = static_cast<char>(0xc0) | static_cast<char>(unicode >> 6);
                buffer[1] = static_cast<char>(0x80) | static_cast<char>(unicode & 0x3f);
                break;
            case 3:
                buffer[0] = static_cast<char>(0xe0) | static_cast<char>(unicode >> 12);
                buffer[1] = static_cast<char>(0x80) | static_cast<char>((unicode >> 6) & 0x3f);
                buffer[2] = static_cast<char>(0x80) | static_cast<char>(unicode & 0x3f);
                break;
            case 4:
                buffer[0] = static_cast<char>(0xf0) | static_cast<char>(unicode >> 18);
                buffer[1] = static_cast<char>(0x80) | static_cast<char>((unicode >> 12) & 0x3f);
                buffer[2] = static_cast<char>(0x80) | static_cast<char>((unicode >> 6) & 0x3f);
                buffer[3] = static_cast<char>(0x80) | static_cast<char>(unicode & 0x3f);
                break;
            case 5:
                buffer[0] = static_cast<char>(0xf8) | static_cast<char>(unicode >> 24);
                buffer[1] = static_cast<char>(0x80) | static_cast<char>((unicode >> 18) & 0x3f);
                buffer[2] = static_cast<char>(0x80) | static_cast<char>((unicode >> 12) & 0x3f);
                buffer[3] = static_cast<char>(0x80) | static_cast<char>((unicode >> 6) & 0x3f);
                buffer[4] = static_cast<char>(0x80) | static_cast<char>(unicode & 0x3f);
                break;
            case 6:
                buffer[0] = static_cast<char>(0xfc) | static_cast<char>(unicode >> 30);
                buffer[1] = static_cast<char>(0x80) | static_cast<char>((unicode >> 24) & 0x3f);
                buffer[2] = static_cast<char>(0x80) | static_cast<char>((unicode >> 18) & 0x3f);
                buffer[3] = static_cast<char>(0x80) | static_cast<char>((unicode >> 12) & 0x3f);
                buffer[4] = static_cast<char>(0x80) | static_cast<char>((unicode >> 6) & 0x3f);
                buffer[5] = static_cast<char>(0x80) | static_cast<char>(unicode & 0x3f);
                break;
        }
    }
}