#pragma once

#include "token.hpp"
#include <stack>
#include <vector>

namespace karmac {
    class Utf8Iterator;
    class Tokenizer final {
    private:
        enum class State {
            Default
        };

        State _state = State::Default;
        std::stack<uint64_t> _pending_brackets;
        std::vector<Token*> _tokens;

        void parse_identifier(uint64_t& unicode, Utf8Iterator& iterator) noexcept;
        void parse_number(uint64_t& unicode, Utf8Iterator& iterator);
        void parse_operator(uint64_t& unicode, Utf8Iterator& iterator);
    public:
        Tokenizer(const std::string_view& source);
        ~Tokenizer();

        [[nodiscard]] inline const std::vector<Token*>& get_tokens() const noexcept {
            return _tokens;
        }
    };
}