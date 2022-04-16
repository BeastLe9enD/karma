#pragma once

#include "../token/simple_token.hpp"
#include "../tokenize_exception.hpp"
#include <stack>
#include <vector>

namespace karmac::tokenize::atom {
    template<char Char, TokenType FirstType, TokenType SecondType>
    static void branch_1_or_2_len_char(TextIterator& iterator, std::vector<Token*>& tokens) {
        if(iterator.has_chars() && iterator[1] == static_cast<uint64_t>(Char)) {
            tokens.push_back(new SimpleToken(SecondType, iterator));
            ++iterator;
        } else {
            tokens.push_back(new SimpleToken(FirstType, iterator));
        }
    }

    template<char FirstChar, char SecondChar, TokenType FirstType, TokenType SecondType, TokenType ThirdType>
    static void branch_1_or_2_len_2_char(TextIterator& iterator, std::vector<Token*>& tokens) {
        if(iterator.has_chars()) {
            const auto line_offset = iterator.get_line_offset();

            auto unicode = *++iterator;

            switch(unicode) {
                case static_cast<uint64_t>(FirstChar):
                    tokens.push_back(new SimpleToken(SecondType, line_offset));
                    break;
                case static_cast<uint64_t>(SecondChar):
                    tokens.push_back(new SimpleToken(ThirdType, line_offset));
                    break;
                default:
                    --iterator;
                    tokens.push_back(new SimpleToken(FirstType, line_offset));
                    break;
            }

        } else {
            tokens.push_back(new SimpleToken(FirstType, iterator));
        }
    }

    template<char FirstChar, char SecondChar, char ThirdChar, TokenType FirstType, TokenType SecondType, TokenType ThirdType, TokenType FourthType>
    static void branch_1_or_2_len_3_char(TextIterator& iterator, std::vector<Token*>& tokens) {
        if(iterator.has_chars()) {
            const auto line_offset = iterator.get_line_offset();

            auto unicode = *++iterator;

            switch(unicode) {
                case static_cast<uint64_t>(FirstChar):
                    tokens.push_back(new SimpleToken(SecondType, line_offset));
                    break;
                case static_cast<uint64_t>(SecondChar):
                    tokens.push_back(new SimpleToken(ThirdType, line_offset));
                    break;
                case static_cast<uint64_t>(ThirdChar):
                    tokens.push_back(new SimpleToken(FourthType, line_offset));
                    break;
                default:
                    --iterator;
                    tokens.push_back(new SimpleToken(FirstType, line_offset));
                    break;
            }

        } else {
            tokens.push_back(new SimpleToken(FirstType, iterator));
        }
    }

    template<char FirstChar, char SecondChar, char ThirdChar, TokenType FirstType, TokenType SecondType, TokenType ThirdType, TokenType FourthType>
    static void branch_1_or_2_len_2_1_char(TextIterator& iterator, std::vector<Token*>& tokens) {
        if(iterator.has_chars()) {
            const auto line_offset = iterator.get_line_offset();

            auto unicode = *++iterator;

            switch(unicode) {
                case static_cast<uint64_t>(FirstChar):
                    if(iterator.has_chars() && iterator[1] == static_cast<uint64_t>(SecondChar)) {
                        tokens.push_back(new SimpleToken(ThirdType, line_offset));
                        ++iterator;
                    } else {
                        tokens.push_back(new SimpleToken(SecondType, line_offset));
                    }
                    break;
                case static_cast<uint64_t>(ThirdChar):
                    tokens.push_back(new SimpleToken(FourthType, line_offset));
                    break;
                default:
                    --iterator;
                    tokens.push_back(new SimpleToken(FirstType, line_offset));
                    break;
            }

        } else {
            tokens.push_back(new SimpleToken(FirstType, iterator));
        }
    }

    template<TokenType OpeningType, TokenType ClosingType>
    static void open_bracket(TextIterator& iterator, std::vector<Token*>& tokens, std::stack<TokenType>& pending_tokens) {
        pending_tokens.push(ClosingType);
        tokens.push_back(new SimpleToken(OpeningType, iterator));
    }

    template<TokenType OpeningType, TokenType ClosingType>
    static void close_bracket(TextIterator& iterator, std::vector<Token*>& tokens, std::stack<TokenType>& pending_tokens) {
        if(pending_tokens.empty()) {
            throw std::runtime_error("TODO"); //TODO:
        }

        const auto last_token = pending_tokens.top();
        pending_tokens.pop();

        if(last_token != ClosingType) {
            throw std::runtime_error("TODO"); //TODO:
        }

        tokens.push_back(new SimpleToken(ClosingType, iterator));
    }
}