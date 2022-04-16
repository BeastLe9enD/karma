#pragma once

#include "../../util/text/text_iterator.hpp"
#include "../tokenize_exception.hpp"
#include <string>

namespace karmac::tokenize::string_literal {
    [[nodiscard]] inline std::string parse(TextIterator& iterator) {
        std::string literal;
        char buffer[7];

        while (iterator.has_chars()) {
            auto current = *iterator;
            switch (current) {
                case static_cast<uint64_t>('"'):
                    ++iterator;
                    return literal;
                case static_cast<uint64_t>('\\'): {
                    if (!iterator.has_chars()) {
                        throw std::runtime_error("Invalid token: \\");
                    }

                    current = *++iterator;
                    switch (current) {
                        case static_cast<uint64_t>('"'):
                            literal += '"';
                            break;
                        case static_cast<uint64_t>('\''):
                            literal += '\'';
                            break;
                        case static_cast<uint64_t>('0'):
                            literal += '\0';
                            break;
                        case static_cast<uint64_t>('b'):
                            literal += '\b';
                            break;
                        case static_cast<uint64_t>('f'):
                            literal += '\f';
                            break;
                        case static_cast<uint64_t>('n'):
                            literal += '\n';
                            break;
                        case static_cast<uint64_t>('r'):
                            literal += '\r';
                            break;
                        case static_cast<uint64_t>('t'):
                            literal += '\t';
                            break;
                        case static_cast<uint64_t>('u'):
                            karmac_unimplemented();
                            break;
                        case static_cast<uint64_t>('v'):
                            literal += '\v';
                            break;
                        case static_cast<uint64_t>('\\'):
                            literal += '\\';
                            break;
                        default:
                            throw std::runtime_error("Invalid token: TODO");
                    }
                }
                    break;
                default:
                    utf8::from_unicode(current, buffer);
                    literal += buffer;
                    break;
            }

            ++iterator;

        }

        throw std::runtime_error("Invalid token, expected \"");
    }
}