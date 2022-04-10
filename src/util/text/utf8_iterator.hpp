#pragma once

#include "../assert.hpp"
#include "utf8.hpp"
#include <stack>
#include <string_view>

namespace karmac {
    class Utf8Iterator final {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = uint64_t;

    private:
        const char* _head = nullptr;
        std::stack<size_t> _last_offsets;
    public:
        explicit Utf8Iterator(const char* p) noexcept : _head(p) {
            karmac_assert(p);
        }

        [[nodiscard]] inline Utf8Iterator copy() const noexcept {
            return Utf8Iterator(_head);
        }

        [[nodiscard]] inline bool has_chars() const noexcept {
            return *(*this) != '\0';
        }

        [[nodiscard]] inline bool has_chars(size_t count) const noexcept {
            auto iterator = this->copy();

            for(auto i = 0; i < count; i++) {
                if(!iterator.has_chars()) {
                    return false;
                }

                ++iterator;
            }

            return true;
        }

        [[nodiscard]] value_type inline operator *() const noexcept {
            return utf8::to_unicode(_head);
        }

        inline Utf8Iterator& operator ++() noexcept {
            const auto len = utf8::num_chars(_head);
            _last_offsets.push(len);
            _head += len;
            return *this;
        }

        [[nodiscard]] inline Utf8Iterator operator ++(int) noexcept {
            auto iterator = *this;
            ++(*this);
            return iterator;
        }

        inline Utf8Iterator& operator +=(size_t count) noexcept {
            for(auto i = 0; i < count; i++) {
                ++(*this);
            }
            return *this;
        }

        inline Utf8Iterator& operator --() noexcept {
            karmac_assert(!_last_offsets.empty());
            const auto len = _last_offsets.top();
            _last_offsets.pop();

            _head -= len;

            return *this;
        }

        [[nodiscard]] inline Utf8Iterator operator --(int) noexcept {
            auto iterator = *this;
            --(*this);
            return iterator;
        }

        inline Utf8Iterator& operator -=(size_t count) noexcept {
            for(auto i = 0; i < count; i++) {
                --(*this);
            }
            return *this;
        }

        [[nodiscard]] inline uint64_t operator [](size_t index) const noexcept {
            auto iterator = *this;
            iterator += index;
            return *iterator;
        }

        [[nodiscard]] inline bool operator ==(const Utf8Iterator& other) const noexcept {
            return _head == other._head;
        }

        [[nodiscard]] inline bool operator !=(const Utf8Iterator& other) const noexcept {
            return _head != other._head;
        }
    };
}