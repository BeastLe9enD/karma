#pragma once

#include "utf8_iterator.hpp"
#include <stack>

namespace karmac {
    class TextIterator;
    class Utf8BiIterator final {
        friend class TextIterator;
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = uint64_t;
    private:
        const char* _start;
        const char* _head;
        std::stack<size_t> _last_offsets;
    public:
        explicit Utf8BiIterator(const char* p) noexcept : _start(p), _head(p) {
            karmac_assert(p);
        }

        inline void reset() noexcept {
            _head = _start;
            _last_offsets = std::stack<size_t>();
        }

        [[nodiscard]] inline bool has_chars() const {
            return *(*this) != static_cast<uint64_t>('\0');
        }

        [[nodiscard]] inline bool has_chars(size_t count) const {
            Utf8Iterator iterator(get_head());

            for(auto i = 0; i < count; i++) {
                if(!iterator.has_chars()) {
                    return false;
                }

                ++iterator;
            }

            return true;
        }

        [[nodiscard]] inline size_t get_remaining_chars() const {
            Utf8Iterator iterator(get_head());
            size_t count = 0;

            while(iterator.has_chars()) {
                ++count;
                ++iterator;
            }

            return count;
        }

        [[nodiscard]] inline size_t get_remaining_chars(size_t max) const {
            Utf8Iterator iterator(get_head());
            size_t count = 0;

            while(iterator.has_chars() && count < max) {
                ++count;
                ++iterator;
            }

            return count;
        }

        [[nodiscard]] inline const char* get_head() const noexcept {
            return _head;
        }

        //Operators
        [[nodiscard]] inline value_type operator *() const {
            return utf8::to_unicode(_head);
        }

        inline Utf8BiIterator& operator ++() {
            const auto len = utf8::num_chars(_head);
            _last_offsets.push(len);
            _head += len;

            return *this;
        }

        inline Utf8BiIterator& operator --() {
            karmac_assert(!_last_offsets.empty());

            const auto len = _last_offsets.top();
            _last_offsets.pop();

            _head -= len;

            return *this;
        }

        inline Utf8BiIterator& operator +=(size_t count) {
            for(auto i = 0; i < count; i++) {
                ++(*this);
            }

            return *this;
        }

        inline Utf8BiIterator& operator -=(size_t count) {
            for(auto i = 0; i < count; i++) {
                --(*this);
            }

            return *this;
        }

        [[nodiscard]] inline Utf8BiIterator operator ++(int) {
            auto iterator = *this;
            ++(*this);
            return iterator;
        }

        [[nodiscard]] inline Utf8BiIterator operator --(int) {
            auto iterator = *this;
            --(*this);
            return iterator;
        }

        [[nodiscard]] inline value_type operator [](size_t index) const {
            Utf8Iterator iterator(_head);
            iterator += index;
            return *iterator;
        }

        [[nodiscard]] inline bool operator ==(const Utf8BiIterator& other) const noexcept {
            return _start == other._start && _head == other._head && _last_offsets == other._last_offsets;
        }

        [[nodiscard]] inline bool operator !=(const Utf8BiIterator& other) const noexcept {
            return _start != other._start || _head != other._head || _last_offsets != other._last_offsets;
        }
    };
}