#pragma once

#include "../../assert.hpp"
#include "utf8.hpp"
#include <iterator>

namespace karmac {
    class Utf8Iterator final {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = uint64_t;
    private:
        const char* _start;
        const char* _head;

    public:
        explicit Utf8Iterator(const char* p) noexcept : _start(p), _head(p) {
            karmac_assert(p);
        }

        inline void reset() noexcept {
            _head = _start;
        }

        [[nodiscard]] inline bool has_chars() const {
            return *(*this) != static_cast<uint64_t>('0');
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

        inline Utf8Iterator& operator ++() {
            const auto len = utf8::num_chars(_head);
            _head += len;
            return *this;
        }

        inline Utf8Iterator& operator +=(size_t count) {
            for(auto i = 0; i < count; i++) {
                ++(*this);
            }

            return *this;
        }

        [[nodiscard]] inline Utf8Iterator operator ++(int) {
            auto iterator = *this;
            ++(*this);
            return iterator;
        }

        [[nodiscard]] inline value_type operator [](size_t index) const {
            auto iterator = *this;
            iterator += index;
            return *iterator;
        }

        [[nodiscard]] inline bool operator ==(const Utf8Iterator& other) const noexcept {
            return _start == other._start && _head == other._head;
        }

        [[nodiscard]] inline bool operator !=(const Utf8Iterator& other) const noexcept {
            return _start != other._start || _head != other._head;
        }
    };
}