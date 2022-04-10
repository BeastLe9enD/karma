#pragma once

#include "utf8_iterator.hpp"
#include <cstdint>

namespace karmac::character {
    [[nodiscard]] inline bool is_whitespace(uint64_t ch) noexcept {
        return ch == static_cast<uint64_t>('\t')
            || ch == static_cast<uint64_t>('\b')
            || ch == static_cast<uint64_t>('\r')
            || ch == static_cast<uint64_t>('\n')
            || ch == static_cast<uint64_t>(' ');
    }

    [[nodiscard]] inline bool is_bin_digit(uint64_t ch) noexcept {
        return ch == static_cast<uint64_t>('0') || ch == static_cast<uint64_t>('1');
    }

    [[nodiscard]] inline bool is_oct_digit(uint64_t ch) noexcept {
        return ch >= static_cast<uint64_t>('0') && ch <= static_cast<uint64_t>('7');
    }

    [[nodiscard]] inline bool is_dec_digit(uint64_t ch) noexcept {
        return ch >= static_cast<uint64_t>('0') && ch <= static_cast<uint64_t>('9');
    }

    [[nodiscard]] inline bool is_dec_digit_non_zero(uint64_t ch) noexcept {
        return ch >= static_cast<uint64_t>('1') && ch <= static_cast<uint64_t>('9');
    }

    [[nodiscard]] inline bool is_hex_digit(uint64_t ch) noexcept {
        return is_dec_digit(ch)
            || (ch >= static_cast<uint64_t>('a') && ch <= static_cast<uint64_t>('f'))
            || (ch >= static_cast<uint64_t>('A') && ch <= static_cast<uint64_t>('F'));
    }

    [[nodiscard]] inline bool is_number_start(uint64_t ch) noexcept {
        return is_dec_digit(ch)
            /*|| ch == static_cast<uint64_t>('+')
            || ch == static_cast<uint64_t>('-')
            || ch == static_cast<uint64_t>('.')*/; //TODO: reimpl
    }

    [[nodiscard]] inline bool is_letter(uint64_t ch) noexcept {
        return (ch >= static_cast<uint64_t>('a') && ch <= static_cast<uint64_t>('z'))
            || (ch >= static_cast<uint64_t>('A') && ch <= static_cast<uint64_t>('Z'));
    }

    [[nodiscard]] inline bool is_identifier_start(uint64_t ch) noexcept {
        return is_letter(ch) || ch == static_cast<uint64_t>('_');
    }

    [[nodiscard]] inline bool is_identifier(uint64_t ch) noexcept {
        return is_identifier_start(ch) || is_dec_digit(ch);
    }

    inline void skip_whitespace(Utf8Iterator& iterator) noexcept {
        while(is_whitespace(*iterator)) {
            ++iterator;
        }
    }
}