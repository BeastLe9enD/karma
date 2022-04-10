#include "tokenizer.hpp"
#include "../util/text/utf8_iterator.hpp"
#include "../util/text/character.hpp"
#include <optional>
#include <stdexcept>
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

    void Tokenizer::parse_identifier(uint64_t& unicode, Utf8Iterator& iterator) noexcept {
        std::string identifier;

        char buffer[7];
        while(character::is_identifier(*iterator)) {
            utf8::from_unicode(*iterator, buffer);
            identifier += buffer;

            ++iterator;
        }

        const auto keyword_iter = _keywords.find(identifier);
        if(keyword_iter == _keywords.end()) {
            _tokens.push_back(new IdentifierToken(std::move(identifier), 0, 0));
        } else {
            _tokens.push_back(new SimpleToken(keyword_iter->second, 0, 0));
        }
    }

    [[nodiscard]] static inline uint64_t parse_bin_literal(Utf8Iterator& iterator) {
        auto current = *iterator;
        uint64_t value = 0;

        std::string literal;
        char buffer[7];

        while(character::is_bin_digit(current) || current == static_cast<uint64_t>('_')) {
            if(current == static_cast<uint64_t>('_')) {
                continue;
            }

            utf8::from_unicode(current, buffer);
            literal += buffer;

            current = *++iterator;
        }

        if(literal.length() > 64) {
            throw std::runtime_error("Invalid binary integer literal");
        }

        for(auto i = 0; i < literal.length(); i++) {
            if(literal[i] == '1') {
                value += 1 << (literal.length() - i - 1);
            }
        }

        return value;
    }

    [[nodiscard]] static inline uint64_t parse_oct_literal(Utf8Iterator& iterator) {
        auto current = *iterator;
        uint64_t value = 0;

        std::string literal;
        char buffer[7];

        while(character::is_oct_digit(current) || current == static_cast<uint64_t>('_')) {
            if(current == static_cast<uint64_t>('_')) {
                continue;
            }

            utf8::from_unicode(current, buffer);
            literal += buffer;

            current = *++iterator;
        }

        if(literal.length() > 21) {
            throw std::runtime_error("Invalid octal integer literal");
        }

        for(auto i = 0; i < literal.length(); i++) {
            value += static_cast<uint64_t>(literal[i] - '0') << (3 * (literal.length() - i - 1));
        }

        return value;
    }

    [[nodiscard]] static inline uint64_t parse_hex_literal(Utf8Iterator& iterator) {
        auto current = *iterator;
        uint64_t value = 0;

        std::string literal;
        char buffer[7];

        while(character::is_hex_digit(current) || current == static_cast<uint64_t>('_')) {
            if(current == static_cast<uint64_t>('_')) {
                continue;
            }

            utf8::from_unicode(current, buffer);
            literal += buffer;

            current = *++iterator;
        }

        if(literal.length() > 16) {
            throw std::runtime_error("Invalid hexadecimal integer literal");
        }

        for(auto i = 0; i < literal.length(); i++) {
            const auto ch = literal[i];
            uint64_t digit = 0;

            if(ch >= static_cast<uint64_t>('0') && ch <= static_cast<uint64_t>('9')) {
                digit = ch - static_cast<uint64_t>('0');
            } else if(ch >= static_cast<uint64_t>('a') && ch <= static_cast<uint64_t>('f')) {
                digit = ch - static_cast<uint64_t>('a') + 10;
            } else if(ch >= static_cast<uint64_t>('A') && ch <= static_cast<uint64_t>('F')) {
                digit = ch - static_cast<uint64_t>('A') + 10;
            }

            value += digit << (4 * (literal.length() - i - 1));
        }

        return value;
    }

    void Tokenizer::parse_number(uint64_t& unicode, Utf8Iterator& iterator) {
        if(unicode == static_cast<uint64_t>('0')) {

            if(iterator.has_chars()) {
                unicode = *++iterator;

                switch(unicode) {
                    case 'b':
                    case 'B': {
                        ++iterator;
                        const auto value = parse_bin_literal(iterator);
                        _tokens.push_back(new U64LiteralToken(value, 0, 0));
                    }
                        break;
                    case 'o':
                    case 'O': {
                        ++iterator;
                        const auto value = parse_oct_literal(iterator);
                        _tokens.push_back(new U64LiteralToken(value, 0, 0));
                    }
                        break;
                    case 'x':
                    case 'X': {
                        ++iterator;
                        const auto value = parse_hex_literal(iterator);
                        _tokens.push_back(new U64LiteralToken(value, 0, 0));
                    }
                        break;
                    default:
                        break;
                }
            }
        }
    }

    void Tokenizer::parse_operator(uint64_t& unicode, Utf8Iterator& iterator) {
        switch(unicode) {
            case '!':
                if(iterator.has_chars() && iterator[1] == '=') {
                    _tokens.push_back(new SimpleToken(TokenType::NotEquals, 0, 0));
                    ++iterator;
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Not, 0, 0));
                }
                break;
            case '"':
                karmac_unimplemented();
                break;
            case '%':
                if(iterator.has_chars() && iterator[1] == '=') {
                    ++iterator;
                    _tokens.push_back(new SimpleToken(TokenType::ModAssign, 0, 0));
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Mod, 0, 0));
                }
                break;
            case '&':
                if(iterator.has_chars()) {
                    unicode = *++iterator;

                    switch(unicode) {
                        case '&':
                            _tokens.push_back(new SimpleToken(TokenType::Conjunction, 0, 0));
                            break;
                        case '=':
                            _tokens.push_back(new SimpleToken(TokenType::AndAssign, 0, 0));
                            break;
                        default:
                            --iterator;
                            _tokens.push_back(new SimpleToken(TokenType::And, 0, 0));
                            break;
                    }
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::And, 0, 0));
                }
                break;
            case '(':
                _pending_brackets.push(')');
                _tokens.push_back(new SimpleToken(TokenType::LeftBracket, 0, 0));
                break;
            case ')': {
                if(_pending_brackets.empty()) {
                    throw std::runtime_error("Invalid closing bracket: )");
                }

                const auto last_bracket = _pending_brackets.top();
                _pending_brackets.pop();

                if(last_bracket != ')') {
                    throw std::runtime_error("Invalid closing bracket: )");
                }

                _tokens.push_back(new SimpleToken(TokenType::RightBracket, 0, 0));
            }
                break;
            case '*':
                if(iterator.has_chars() && iterator[1] == '=') {
                    _tokens.push_back(new SimpleToken(TokenType::MulAssign, 0, 0));
                    ++iterator;
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Mul, 0, 0));
                }
                break;
            case '+':
                if(iterator.has_chars()) {
                    unicode = *++iterator;

                    switch(unicode) {
                        case '+':
                            _tokens.push_back(new SimpleToken(TokenType::Increment, 0, 0));
                            break;
                        case '=':
                            _tokens.push_back(new SimpleToken(TokenType::AddAssign, 0, 0));
                            break;
                        default:
                            --iterator;
                            _tokens.push_back(new SimpleToken(TokenType::Add, 0, 0));
                            break;
                    }
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Add, 0, 0));
                }
                break;
            case ',':
                _tokens.push_back(new SimpleToken(TokenType::Comma, 0, 0));
                break;
            case '-':
                if(iterator.has_chars()) {
                    unicode = *++iterator;

                    switch(unicode) {
                        case '-':
                            _tokens.push_back(new SimpleToken(TokenType::Decrement, 0, 0));
                            break;
                        case '=':
                            _tokens.push_back(new SimpleToken(TokenType::SubAssign, 0, 0));
                            break;
                        case '>':
                            _tokens.push_back(new SimpleToken(TokenType::Arrow, 0, 0));
                            break;
                        default:
                            --iterator;
                            _tokens.push_back(new SimpleToken(TokenType::Sub, 0, 0));
                            break;
                    }
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Sub, 0, 0));
                }
                break;
            case '.':
                if(iterator.has_chars() && iterator[1] == '.') {
                    _tokens.push_back(new SimpleToken(TokenType::DoubleDot, 0, 0));
                    ++iterator;
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Dot, 0, 0));
                }
                break;
            case '/':
                if(iterator.has_chars() && iterator[1] == '=') {
                    _tokens.push_back(new SimpleToken(TokenType::DivAssign, 0, 0));
                    ++iterator;
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Div, 0, 0));
                }
                break;
            case ':':
                if(iterator.has_chars() && iterator[1] == ':') {
                    _tokens.push_back(new SimpleToken(TokenType::DoubleColon, 0, 0));
                    ++iterator;
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Colon, 0, 0));
                }
                break;
            case ';':
                _tokens.push_back(new SimpleToken(TokenType::Semicolon, 0, 0));
                break;
            case '<':
                if(iterator.has_chars()) {
                    unicode = *++iterator;

                    switch(unicode) {
                        case '<':
                            if(iterator.has_chars() && iterator[1] == '=') {
                                _tokens.push_back(new SimpleToken(TokenType::LeftShiftAssign, 0, 0));
                                ++iterator;
                            } else {
                                _tokens.push_back(new SimpleToken(TokenType::LeftShift, 0, 0));
                            }
                            break;
                        case '=':
                            _tokens.push_back(new SimpleToken(TokenType::LessEquals, 0, 0));
                            break;
                        default:
                            --iterator;
                            _tokens.push_back(new SimpleToken(TokenType::Less, 0, 0));
                            break;
                    }
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Less, 0, 0));
                }
                break;
            case '=':
                if(iterator.has_chars() && iterator[1] == '=') {
                    _tokens.push_back(new SimpleToken(TokenType::Equals, 0, 0));
                    ++iterator;
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Assign, 0, 0));
                }
                break;
            case '>':
                if(iterator.has_chars()) {
                    unicode = *++iterator;

                    switch(unicode) {
                        case '=':
                            _tokens.push_back(new SimpleToken(TokenType::GreaterEquals, 0, 0));
                            break;
                        case '>':
                            if(iterator.has_chars() && iterator[1] == '=') {
                                _tokens.push_back(new SimpleToken(TokenType::RightShiftAssign, 0, 0));
                                ++iterator;
                            } else {
                                _tokens.push_back(new SimpleToken(TokenType::RightShift, 0, 0));
                            }
                            break;
                        default:
                            --iterator;
                            _tokens.push_back(new SimpleToken(TokenType::Greater, 0, 0));
                            break;
                    }
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Greater, 0, 0));
                }
                break;
            case '?':
                _tokens.push_back(new SimpleToken(TokenType::QuestionMark, 0, 0));
                break;
            case '[':
                _pending_brackets.push(']');
                _tokens.push_back(new SimpleToken(TokenType::LeftSquareBracket, 0, 0));
                break;
            case ']': {
                if(_pending_brackets.empty()) {
                    throw std::runtime_error("Invalid closing bracket: ]");
                }

                const auto last_bracket = _pending_brackets.top();
                _pending_brackets.pop();

                if(last_bracket != ']') {
                    throw std::runtime_error("Invalid closing bracket: ]");
                }

                _tokens.push_back(new SimpleToken(TokenType::RightSquareBracket, 0, 0));
            }
                break;
            case '^':
                if(iterator.has_chars() && iterator[1] == '=') {
                    ++iterator;
                    _tokens.push_back(new SimpleToken(TokenType::XorAssign, 0, 0));
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Xor, 0, 0));
                }
                break;
            case '{':
                _pending_brackets.push('}');
                _tokens.push_back(new SimpleToken(TokenType::LeftCurlyBracket, 0, 0));
                break;
            case '|':
                if(iterator.has_chars()) {
                    unicode = *++iterator;

                    switch(unicode) {
                        case '|':
                            _tokens.push_back(new SimpleToken(TokenType::Disjunction, 0, 0));
                            break;
                        case '=':
                            _tokens.push_back(new SimpleToken(TokenType::OrAssign, 0, 0));
                            break;
                        default:
                            --iterator;
                            _tokens.push_back(new SimpleToken(TokenType::Or, 0, 0));
                            break;
                    }
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Or, 0, 0));
                }
                break;
            case '}': {
                if(_pending_brackets.empty()) {
                    throw std::runtime_error("Invalid closing bracket: }");
                }

                const auto last_bracket = _pending_brackets.top();
                _pending_brackets.pop();

                if(last_bracket != '}') {
                    throw std::runtime_error("Invalid closing bracket: }");
                }

                _tokens.push_back(new SimpleToken(TokenType::RightCurlyBracket, 0, 0));
            }
                break;
            default: {
                char buffer[7];
                utf8::from_unicode(unicode, buffer);

                std::string message = "Unexpected token: '";
                message.append(buffer);
                message.push_back('\'');

                throw std::runtime_error(message);
            }
                break;
        }
    }

    Tokenizer::Tokenizer(const std::string_view& source) {
        Utf8Iterator iterator(source.data());

        while(iterator.has_chars()) {
            character::skip_whitespace(iterator); //TODO: do line thing

            auto unicode = *iterator;

            if(character::is_identifier_start(unicode)) {
                parse_identifier(unicode, iterator);
                continue;
            }

            if(character::is_number_start(unicode)) {
                parse_number(unicode, iterator);
                continue;
            }


            parse_operator(unicode, iterator);

            ++iterator;
        }

        if(!_pending_brackets.empty()) {
            //TODO: throw exception with pending brackets
        }
    }

    Tokenizer::~Tokenizer() {
        for(const auto* token : _tokens) {
            delete token;
        }
    }
}