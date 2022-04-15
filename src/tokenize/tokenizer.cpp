#include "tokenizer.hpp"
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

    void Tokenizer::parse_identifier(uint64_t unicode, Utf8Iterator& iterator) noexcept {
        std::string identifier;

        const auto line_offset = iterator.get_line_offset();

        char buffer[7];
        while(character::is_identifier(*iterator)) {
            utf8::from_unicode(*iterator, buffer);
            identifier += buffer;

            ++iterator;
        }

        const auto keyword_iter = _keywords.find(identifier);
        if(keyword_iter == _keywords.end()) {
            _tokens.push_back(new IdentifierToken(std::move(identifier), line_offset));
        } else {
            _tokens.push_back(new SimpleToken(keyword_iter->second, line_offset));
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
            if(literal[i] == static_cast<uint64_t>('1')) {
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

    void Tokenizer::parse_number(uint64_t unicode, Utf8Iterator& iterator) {
        const auto line_offset = iterator.get_line_offset();

        if(unicode == static_cast<uint64_t>('0')) {

            if(iterator.has_chars()) {
                unicode = *++iterator;

                switch(unicode) {
                    case 'b':
                    case 'B': {
                        ++iterator;
                        const auto value = parse_bin_literal(iterator); //TODO:
                        _tokens.push_back(new U64LiteralToken(value, line_offset));
                    }
                        break;
                    case 'o':
                    case 'O': {
                        ++iterator;
                        const auto value = parse_oct_literal(iterator); //TODO:
                        _tokens.push_back(new U64LiteralToken(value, line_offset));
                    }
                        break;
                    case 'x':
                    case 'X': {
                        ++iterator;
                        const auto value = parse_hex_literal(iterator); //TODO:
                        _tokens.push_back(new U64LiteralToken(value, line_offset));
                    }
                        break;
                    default:
                        break;
                }
            }
        }
    }

    static inline void parse_string_literal(Utf8Iterator& iterator, std::string& literal) {
        char buffer[7];

        while(iterator.has_chars()) {
            auto current = *iterator;
            switch(current) {
                case static_cast<uint64_t>('"'):
                    ++iterator;
                    return;
                case static_cast<uint64_t>('\\'): {
                    if(!iterator.has_chars()) {
                        throw std::runtime_error("Invalid token: \\");
                    }

                    current = *++iterator;
                    switch(current) {
                        case static_cast<uint64_t>('"'):
                            literal += '"';
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

    void Tokenizer::parse_string(Utf8Iterator& iterator) {
        ++iterator;

        const auto line_offset = iterator.get_line_offset();

        std::string literal;
        parse_string_literal(iterator, literal);

        _tokens.push_back(new StringLiteralToken(std::move(literal), line_offset)); //TODO: check if line offset is correct here
    }

    void Tokenizer::parse_line_comment(Utf8Iterator& iterator) {
        ++iterator;

        while(iterator.has_chars()) {
            if(*iterator == static_cast<uint64_t>('\n')) {
                break;
            }

            ++iterator;
        }
    }

    void Tokenizer::parse_multiline_comment(Utf8Iterator& iterator) {
        ++iterator;

        size_t num_tokens = 1;

        while(iterator.has_chars()) {
            const auto current = *iterator;

            switch(current) {
                case static_cast<uint64_t>('*'):
                    if(iterator[1] == static_cast<uint64_t>('/')) {
                        ++iterator;
                        if(--num_tokens == 0) {
                            return;
                        }
                    }
                    break;
                case static_cast<uint64_t>('/'):
                    if(iterator[1] == static_cast<uint64_t>('*')) {
                        ++num_tokens;
                        ++iterator;
                    }
                    break;
                default:
                    break;
            }

            ++iterator;
        }

        throw std::runtime_error("Invalid token, expected */");
    }

    void Tokenizer::parse_operator(uint64_t unicode, Utf8Iterator& iterator) {
        switch(unicode) {
            case '!':
                if(iterator.has_chars() && iterator[1] == '=') {
                    _tokens.push_back(new SimpleToken(TokenType::NotEquals, iterator));
                    ++iterator;
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Not, iterator));
                }
                break;
            case '%':
                if(iterator.has_chars() && iterator[1] == '=') {
                    _tokens.push_back(new SimpleToken(TokenType::ModAssign, iterator));
                    ++iterator;
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Mod, iterator));
                }
                break;
            case '&':
                if(iterator.has_chars()) {
                    const auto line_offset = iterator.get_line_offset();

                    unicode = *++iterator;

                    switch(unicode) {
                        case '&':
                            _tokens.push_back(new SimpleToken(TokenType::Conjunction, line_offset));
                            break;
                        case '=':
                            _tokens.push_back(new SimpleToken(TokenType::AndAssign, line_offset));
                            break;
                        default:
                            --iterator;
                            _tokens.push_back(new SimpleToken(TokenType::And, line_offset));
                            break;
                    }
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::And, iterator));
                }
                break;
            case '(':
                _pending_tokens.push(')');
                _tokens.push_back(new SimpleToken(TokenType::LeftBracket, iterator));
                break;
            case ')': {
                if(_pending_tokens.empty()) {
                    throw std::runtime_error("Invalid closing bracket: )");
                }

                const auto last_bracket = _pending_tokens.top();
                _pending_tokens.pop();

                if(last_bracket != ')') {
                    throw std::runtime_error("Invalid closing bracket: )");
                }

                _tokens.push_back(new SimpleToken(TokenType::RightBracket, iterator));
            }
                break;
            case '*':
                if(iterator.has_chars() && iterator[1] == '=') {
                    _tokens.push_back(new SimpleToken(TokenType::MulAssign, iterator));
                    ++iterator;
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Mul, iterator));
                }
                break;
            case '+':
                if(iterator.has_chars()) {
                    const auto line_offset = iterator.get_line_offset();

                    unicode = *++iterator;

                    switch(unicode) {
                        case '+':
                            _tokens.push_back(new SimpleToken(TokenType::Increment, line_offset));
                            break;
                        case '=':
                            _tokens.push_back(new SimpleToken(TokenType::AddAssign, line_offset));
                            break;
                        default:
                            --iterator;
                            _tokens.push_back(new SimpleToken(TokenType::Add, line_offset));
                            break;
                    }
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Add, iterator));
                }
                break;
            case ',':
                _tokens.push_back(new SimpleToken(TokenType::Comma, iterator));
                break;
            case '-':
                if(iterator.has_chars()) {
                    const auto line_offset = iterator.get_line_offset();

                    unicode = *++iterator;

                    switch(unicode) {
                        case '-':
                            _tokens.push_back(new SimpleToken(TokenType::Decrement, line_offset));
                            break;
                        case '=':
                            _tokens.push_back(new SimpleToken(TokenType::SubAssign, line_offset));
                            break;
                        case '>':
                            _tokens.push_back(new SimpleToken(TokenType::Arrow, line_offset));
                            break;
                        default:
                            --iterator;
                            _tokens.push_back(new SimpleToken(TokenType::Sub, line_offset));
                            break;
                    }
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Sub, iterator));
                }
                break;
            case '.':
                if(iterator.has_chars() && iterator[1] == '.') {
                    _tokens.push_back(new SimpleToken(TokenType::DoubleDot, iterator));
                    ++iterator;
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Dot, iterator));
                }
                break;
            case '/':
                if(iterator.has_chars()) {
                    const auto line_offset = iterator.get_line_offset();

                    unicode = *++iterator;

                    switch(unicode) {
                        case '*':
                            parse_multiline_comment(iterator);
                            break;
                        case '/':
                            parse_line_comment(iterator);
                            break;
                        case '=':
                            _tokens.push_back(new SimpleToken(TokenType::DivAssign, line_offset));
                            break;
                        default:
                            --iterator;
                            _tokens.push_back(new SimpleToken(TokenType::Div, line_offset));
                            break;
                    }
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Div, iterator));
                }
                break;
            case ':':
                if(iterator.has_chars() && iterator[1] == ':') {
                    _tokens.push_back(new SimpleToken(TokenType::DoubleColon, iterator));
                    ++iterator;
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Colon, iterator));
                }
                break;
            case ';':
                _tokens.push_back(new SimpleToken(TokenType::Semicolon, iterator));
                break;
            case '<':
                if(iterator.has_chars()) {
                    const auto line_offset = iterator.get_line_offset();

                    unicode = *++iterator;

                    switch(unicode) {
                        case '<':
                            if(iterator.has_chars() && iterator[1] == '=') {
                                _tokens.push_back(new SimpleToken(TokenType::LeftShiftAssign, line_offset));
                                ++iterator;
                            } else {
                                _tokens.push_back(new SimpleToken(TokenType::LeftShift, line_offset));
                            }
                            break;
                        case '=':
                            _tokens.push_back(new SimpleToken(TokenType::LessEquals, line_offset));
                            break;
                        default:
                            --iterator;
                            _tokens.push_back(new SimpleToken(TokenType::Less, line_offset));
                            break;
                    }
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Less, iterator));
                }
                break;
            case '=':
                if(iterator.has_chars() && iterator[1] == '=') {
                    _tokens.push_back(new SimpleToken(TokenType::Equals, iterator));
                    ++iterator;
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Assign, iterator));
                }
                break;
            case '>':
                if(iterator.has_chars()) {
                    const auto line_offset = iterator.get_line_offset();

                    unicode = *++iterator;

                    switch(unicode) {
                        case '=':
                            _tokens.push_back(new SimpleToken(TokenType::GreaterEquals, line_offset));
                            break;
                        case '>':
                            if(iterator.has_chars() && iterator[1] == '=') {
                                _tokens.push_back(new SimpleToken(TokenType::RightShiftAssign, line_offset));
                                ++iterator;
                            } else {
                                _tokens.push_back(new SimpleToken(TokenType::RightShift, line_offset));
                            }
                            break;
                        default:
                            --iterator;
                            _tokens.push_back(new SimpleToken(TokenType::Greater, line_offset));
                            break;
                    }
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Greater, iterator));
                }
                break;
            case '?':
                _tokens.push_back(new SimpleToken(TokenType::QuestionMark, iterator));
                break;
            case '[':
                _pending_tokens.push(']');
                _tokens.push_back(new SimpleToken(TokenType::LeftSquareBracket, iterator));
                break;
            case ']': {
                if(_pending_tokens.empty()) {
                    throw std::runtime_error("Invalid closing bracket: ]");
                }

                const auto last_bracket = _pending_tokens.top();
                _pending_tokens.pop();

                if(last_bracket != ']') {
                    throw std::runtime_error("Invalid closing bracket: ]");
                }

                _tokens.push_back(new SimpleToken(TokenType::RightSquareBracket, iterator));
            }
                break;
            case '^':
                if(iterator.has_chars() && iterator[1] == '=') {
                    _tokens.push_back(new SimpleToken(TokenType::XorAssign, iterator));
                    ++iterator;
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Xor, iterator));
                }
                break;
            case '{':
                _pending_tokens.push('}');
                _tokens.push_back(new SimpleToken(TokenType::LeftCurlyBracket, iterator));
                break;
            case '|':
                if(iterator.has_chars()) {
                    const auto line_offset = iterator.get_line_offset();

                    unicode = *++iterator;

                    switch(unicode) {
                        case '|':
                            _tokens.push_back(new SimpleToken(TokenType::Disjunction, line_offset));
                            break;
                        case '=':
                            _tokens.push_back(new SimpleToken(TokenType::OrAssign, line_offset));
                            break;
                        default:
                            --iterator;
                            _tokens.push_back(new SimpleToken(TokenType::Or, line_offset));
                            break;
                    }
                } else {
                    _tokens.push_back(new SimpleToken(TokenType::Or, iterator));
                }
                break;
            case '}': {
                if(_pending_tokens.empty()) {
                    throw std::runtime_error("Invalid closing bracket: }");
                }

                const auto last_bracket = _pending_tokens.top();
                _pending_tokens.pop();

                if(last_bracket != '}') {
                    throw std::runtime_error("Invalid closing bracket: }");
                }

                _tokens.push_back(new SimpleToken(TokenType::RightCurlyBracket, iterator));
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
        }
    }

    Tokenizer::Tokenizer(const std::string_view& source) {
        Utf8Iterator iterator(source.data());

        while(iterator.has_chars()) {
            character::skip_whitespace(iterator);

            auto unicode = *iterator;

            if(unicode == static_cast<uint64_t>('"')) {
                parse_string(iterator);
                continue;
            }

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

        if(!_pending_tokens.empty()) {
            throw std::runtime_error("Pending tokens must not be empty");
        }
    }
}