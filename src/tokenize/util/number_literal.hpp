#pragma once

#include "../tokenize_exception.hpp"
#include "../../util/text/character.hpp"
#include "../../util/text/text_iterator.hpp"
#include <string>

namespace karmac::tokenize::number_literal {
    [[nodiscard]] inline uint64_t parse_bin(TextIterator& iterator) {
        auto unicode = *iterator;

        uint64_t literal = 0;

        while(character::is_bin_digit(unicode) || unicode == static_cast<uint64_t>('_')) {
            if(unicode != static_cast<uint64_t>('_')) { //TODO: overflow check
                literal = (literal << 1) | (unicode - static_cast<uint64_t>('0'));
            }

            unicode = *++iterator;
        }

        return literal;
    }

    [[nodiscard]] inline uint64_t parse_oct(TextIterator& iterator) {
        auto unicode = *iterator;

        uint64_t literal = 0;

        while(character::is_oct_digit(unicode)) {
            if(unicode != static_cast<uint64_t>('_')) { //TODO: overflow check
                literal = (literal << 3) | (unicode - static_cast<uint64_t>('7'));
            }

            unicode = *++iterator;
        }

        return literal;
    }

    [[nodiscard]] inline uint64_t parse_hex(TextIterator& iterator) {
        auto unicode = *iterator;

        uint64_t literal = 0;

        while(character::is_hex_digit(unicode)) {
            if(unicode != static_cast<uint64_t>('_')) { //TODO: overflow check
                uint64_t digit = 0;

                if(unicode >= static_cast<uint64_t>('0') && unicode <= static_cast<uint64_t>('9')) {
                    digit = unicode - static_cast<uint64_t>('0');
                } else if(unicode >= static_cast<uint64_t>('a') && unicode <= static_cast<uint64_t>('f')) {
                    digit = unicode - static_cast<uint64_t>('a') + 10;
                } else if(unicode >= static_cast<uint64_t>('A') && unicode <= static_cast<uint64_t>('F')) {
                    digit = unicode - static_cast<uint64_t>('A') + 10;
                }

                literal = (literal << 4) | digit;
            }

            unicode = *++iterator;
        }

        return literal;
    }

    [[nodiscard]] inline bool parse_type(TextIterator& iterator, TokenType& type) {
        auto unicode = *iterator;

        switch(unicode) {
            case static_cast<uint64_t>('f'):
                if(iterator.has_chars(2)) {
                    if(iterator[1] == static_cast<uint64_t>('3') && iterator[2] == static_cast<uint64_t>('2')) {
                        type = TokenType::F32Literal;
                        iterator += 3;
                        return true;
                    } else if(iterator[1] == static_cast<uint64_t>('6') && iterator[2] == static_cast<uint64_t>('4')) {
                        type = TokenType::F64Literal;
                        iterator += 3;
                        return true;
                    }
                }
                break;
            case static_cast<uint64_t>('i'): {
                const auto rem_chars = iterator.get_remaining_chars(4);
                if(rem_chars == 4) {
                    if(iterator[1] == static_cast<uint64_t>('s')
                        && iterator[2] == static_cast<uint64_t>('i')
                        && iterator[3] == static_cast<uint64_t>('z')
                        && iterator[4] == static_cast<uint64_t>('e')) {
                        type = TokenType::ISizeLiteral;
                        iterator += 5;
                        return true;
                    }
                }
                if(rem_chars >= 2) {
                    if(iterator[1] == static_cast<uint64_t>('1') && iterator[2] == static_cast<uint64_t>('6')) {
                        type = TokenType::I16Literal;
                        iterator += 3;
                        return true;
                    }
                    if(iterator[1] == static_cast<uint64_t>('3') && iterator[2] == static_cast<uint64_t>('2')) {
                        type = TokenType::I32Literal;
                        iterator += 3;
                        return true;
                    }
                    if(iterator[1] == static_cast<uint64_t>('6') && iterator[2] == static_cast<uint64_t>('4')) {
                        type = TokenType::I64Literal;
                        iterator += 3;
                        return true;
                    }
                }
                if(rem_chars >= 1) {
                    if(iterator[1] == static_cast<uint64_t>('8')) {
                        type = TokenType::I8Literal;
                        iterator += 2;
                        return true;
                    }
                }
            }
                break;
            case static_cast<uint64_t>('u'): {
                const auto rem_chars = iterator.get_remaining_chars(4);
                if(rem_chars == 4) {
                    if(iterator[1] == static_cast<uint64_t>('s')
                       && iterator[2] == static_cast<uint64_t>('i')
                       && iterator[3] == static_cast<uint64_t>('z')
                       && iterator[4] == static_cast<uint64_t>('e')) {
                        type = TokenType::USizeLiteral;
                        iterator += 5;
                        return true;
                    }
                }
                if(rem_chars >= 2) {
                    if(iterator[1] == static_cast<uint64_t>('1') && iterator[2] == static_cast<uint64_t>('6')) {
                        type = TokenType::U16Literal;
                        iterator += 3;
                        return true;
                    }
                    if(iterator[1] == static_cast<uint64_t>('3') && iterator[2] == static_cast<uint64_t>('2')) {
                        type = TokenType::U32Literal;
                        iterator += 3;
                        return true;
                    }
                    if(iterator[1] == static_cast<uint64_t>('6') && iterator[2] == static_cast<uint64_t>('4')) {
                        type = TokenType::U64Literal;
                        iterator += 3;
                        return true;
                    }
                }
                if(rem_chars >= 1) {
                    if(iterator[1] == static_cast<uint64_t>('8')) {
                        type = TokenType::U8Literal;
                        iterator += 2;
                        return true;
                    }
                }
            }
                break;
            default:
                return false;
                break;
        }
    }
}