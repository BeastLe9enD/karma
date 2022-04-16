#include "tokenizer.hpp"
#include "token/identifier_token.hpp"
#include "token/literal_token.hpp"
#include "token/string_literal_token.hpp"
#include "util/atom.hpp"
#include "util/number_literal.hpp"
#include "util/string_literal.hpp"
#include "../util/text/character.hpp"
#include <unordered_map>

namespace karmac {
    static std::unordered_map<std::string_view, TokenType> _keywords = {
        { "fn", TokenType::Fn },
        { "if", TokenType::If },
        { "else", TokenType::Else },
        { "for", TokenType::For },
        { "while", TokenType::While },
        { "break", TokenType::Break },
        { "continue", TokenType::Continue },
        { "return", TokenType::Return }
    };

    void Tokenizer::skip_whitespace() {
        while(character::is_whitespace(*_iterator)) {
            ++_iterator;
        }
    }

    void Tokenizer::parse_line_comment() {
        ++_iterator;

        while(_iterator.has_chars()) {
            if(*_iterator == static_cast<uint64_t>('\n')) {
                break;
            }
            ++_iterator;
        }
    }

    void Tokenizer::parse_multiline_comment() {
        ++_iterator;

        size_t num_tokens = 1;

        while(_iterator.has_chars()) {
            const auto current = *_iterator;

            switch(current) {
                case static_cast<uint64_t>('*'):
                    if(_iterator[1] == static_cast<uint64_t>('/')) {
                        ++_iterator;
                        if(--num_tokens == 0) {
                            return;
                        }
                    }
                    break;
                case static_cast<uint64_t>('/'):
                    if(_iterator[1] == static_cast<uint64_t>('*')) {
                        ++num_tokens;
                        ++_iterator;
                    }
                    break;
                default:
                    break;
            }

            ++_iterator;
        }

        throw std::runtime_error("Expected */"); //TODO:
    }

    void Tokenizer::parse_string_literal() {
        ++_iterator;

        const auto line_offset = _iterator.get_line_offset();
        auto literal = tokenize::string_literal::parse(_iterator);

        _tokens.push_back(new StringLiteralToken(std::move(literal), line_offset));
    }

    void Tokenizer::parse_identifier() {
        std::string identifier;

        const auto line_offset = _iterator.get_line_offset();

        char buffer[7];
        while(character::is_identifier(*_iterator)) {
            utf8::from_unicode(*_iterator, buffer);
            identifier += buffer;

            ++_iterator;
        }

        const auto keyword_iter = _keywords.find(identifier);
        if(keyword_iter == _keywords.end()) {
            _tokens.push_back(new IdentifierToken(std::move(identifier), line_offset));
        } else {
            _tokens.push_back(new SimpleToken(keyword_iter->second, line_offset));
        }
    }

    bool Tokenizer::try_parse_number() {
        const auto line_offset = _iterator.get_line_offset();

        auto unicode = *_iterator;
        if(unicode == static_cast<uint64_t>('0')) {
            if(_iterator.has_chars()) {
                unicode = *++_iterator;

                switch(unicode) {
                    case static_cast<uint64_t>('b'):
                    case static_cast<uint64_t>('B'): {
                        ++_iterator;
                        const auto literal = tokenize::number_literal::parse_bin(_iterator);
                    }
                        break;
                    case static_cast<uint64_t>('f'):
                    case static_cast<uint64_t>('F'): {
                        ++_iterator;
                        const auto literal = tokenize::number_literal::parse_hex(_iterator);
                    }
                        break;
                    case static_cast<uint64_t>('o'):
                    case static_cast<uint64_t>('O'): {
                        ++_iterator;
                        const auto literal = tokenize::number_literal::parse_oct(_iterator);
                    }
                        break;
                    default:
                        karmac_unimplemented();
                        break;
                }
            } else {
                _tokens.push_back(new I32LiteralToken(0, line_offset));
                return true;
            }
        }

        return false;
    }

    bool Tokenizer::try_parse_atom() {
        auto result = true;
        auto unicode = *_iterator;

        switch(unicode) {
            case static_cast<uint64_t>('!'):
                tokenize::atom::branch_1_or_2_len_char<'=', TokenType::Not, TokenType::NotEquals>(_iterator, _tokens);
                break;
            case static_cast<uint64_t>('"'):
                parse_string_literal();
                break;
            case static_cast<uint64_t>('%'):
                tokenize::atom::branch_1_or_2_len_char<'=', TokenType::Mod, TokenType::ModAssign>(_iterator, _tokens);
                break;
            case static_cast<uint64_t>('&'):
                tokenize::atom::branch_1_or_2_len_2_char<'&', '=', TokenType::And, TokenType::Conjunction, TokenType::AndAssign>(_iterator, _tokens);
                break;
            case static_cast<uint64_t>('('):
                tokenize::atom::open_bracket<TokenType::LeftBracket, TokenType::RightBracket>(_iterator, _tokens, _pending_tokens);
                break;
            case static_cast<uint64_t>(')'):
                tokenize::atom::close_bracket<TokenType::LeftBracket, TokenType::RightBracket>(_iterator, _tokens, _pending_tokens);
                break;
            case static_cast<uint64_t>('*'):
                tokenize::atom::branch_1_or_2_len_char<'=', TokenType::Mul, TokenType::MulAssign>(_iterator, _tokens);
                break;
            case static_cast<uint64_t>('+'):
                tokenize::atom::branch_1_or_2_len_2_char<'+', '=', TokenType::Add, TokenType::Increment, TokenType::AddAssign>(_iterator, _tokens);
                break;
            case static_cast<uint64_t>(','):
                _tokens.push_back(new SimpleToken(TokenType::Comma, _iterator));
                break;
            case static_cast<uint64_t>('-'):
                tokenize::atom::branch_1_or_2_len_3_char<'-', '=', '>', TokenType::Sub, TokenType::Decrement, TokenType::SubAssign, TokenType::Arrow>(_iterator, _tokens);
                break;
            case static_cast<uint64_t>('.'):
                tokenize::atom::branch_1_or_2_len_char<'.', TokenType::Dot, TokenType::DoubleDot>(_iterator, _tokens);
                break;
            case static_cast<uint64_t>('/'):
                if(_iterator.has_chars()) {
                    const auto line_offset = _iterator.get_line_offset();

                    unicode = *++_iterator;

                    switch(unicode) {
                        case static_cast<uint64_t>('*'):
                            parse_multiline_comment();
                            break;
                        case static_cast<uint64_t>('/'):
                            parse_line_comment();
                            break;
                        case static_cast<uint64_t>('='):
                            _tokens.push_back(new SimpleToken(TokenType::DivAssign, line_offset));
                            break;
                        default:
                            --_iterator;
                            _tokens.push_back(new SimpleToken(TokenType::Div, line_offset));
                            break;
                    }
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Div, _iterator));
                }
                break;
            case static_cast<uint64_t>(':'):
                tokenize::atom::branch_1_or_2_len_char<':', TokenType::Colon, TokenType::DoubleColon>(_iterator, _tokens);
                break;
            case static_cast<uint64_t>(';'):
                _tokens.push_back(new SimpleToken(TokenType::Semicolon, _iterator));
                break;
            case static_cast<uint64_t>('<'):
                tokenize::atom::branch_1_or_2_len_2_1_char<'<', '=', '=', TokenType::Less, TokenType::LeftShift, TokenType::LeftShiftAssign, TokenType::LessEquals>(_iterator, _tokens);
                break;
            case static_cast<uint64_t>('='):
                tokenize::atom::branch_1_or_2_len_char<'=', TokenType::Assign, TokenType::Equals>(_iterator, _tokens);
                break;
            case static_cast<uint64_t>('>'):
                tokenize::atom::branch_1_or_2_len_2_1_char<'>', '=', '=', TokenType::Greater, TokenType::RightShift, TokenType::RightShiftAssign, TokenType::GreaterEquals>(_iterator, _tokens);
                break;
            case static_cast<uint64_t>('?'):
                _tokens.push_back(new SimpleToken(TokenType::QuestionMark, _iterator));
                break;
            case static_cast<uint64_t>('['):
                tokenize::atom::open_bracket<TokenType::LeftSquareBracket, TokenType::RightSquareBracket>(_iterator, _tokens, _pending_tokens);
                break;
            case static_cast<uint64_t>(']'):
                tokenize::atom::close_bracket<TokenType::LeftSquareBracket, TokenType::RightSquareBracket>(_iterator, _tokens, _pending_tokens);
                break;
            case static_cast<uint64_t>('^'):
                tokenize::atom::branch_1_or_2_len_char<'=', TokenType::Xor, TokenType::XorAssign>(_iterator, _tokens);
                break;
            case static_cast<uint64_t>('{'):
                tokenize::atom::open_bracket<TokenType::LeftCurlyBracket, TokenType::RightCurlyBracket>(_iterator, _tokens, _pending_tokens);
                break;
            case static_cast<uint64_t>('|'):
                tokenize::atom::branch_1_or_2_len_2_char<'|', '=', TokenType::Or, TokenType::Disjunction, TokenType::OrAssign>(_iterator, _tokens);
                break;
            case static_cast<uint64_t>('}'):
                tokenize::atom::close_bracket<TokenType::LeftCurlyBracket, TokenType::RightCurlyBracket>(_iterator, _tokens, _pending_tokens);
                break;
            default:
                result = false;
                break;
        }

        return result;
    }

    Tokenizer::Tokenizer(const std::string_view& source) : _iterator(source.data()) {
        while(_iterator.has_chars()) {
            skip_whitespace();

            auto unicode = *_iterator;

            if(character::is_identifier_start(unicode)) {
                parse_identifier();
                continue;
            }

            auto result = try_parse_atom();
            if(!result && character::is_number_start(unicode)) {
                result = try_parse_number();
            }

            if(!result) {
                throw std::runtime_error("Invalid token"); //TODO:
            }

            ++_iterator;
        }
    }

    Tokenizer::~Tokenizer() {
        for(const auto* token : _tokens) {
            delete token;
        }
        _tokens.clear();
    }
}