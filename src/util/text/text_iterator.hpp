#pragma once

#include "utf8/utf8_bi_iterator.hpp"
#include "line_offset.hpp"

namespace karmac {
    class TextIterator final {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = uint64_t;
    private:
        static const size_t _TAB_SIZE = 4;

        Utf8BiIterator _delegate;
        std::stack<size_t> _last_line_offsets;
        LineOffset _line_offset;

    public:
        explicit TextIterator(const char* p) noexcept : _delegate(p) {}

        inline void reset() noexcept {
            _delegate.reset();
        }

        [[nodiscard]] inline bool has_chars() const {
            return _delegate.has_chars();
        }

        [[nodiscard]] inline bool has_chars(size_t count) const {
            return _delegate.has_chars(count);
        }

        [[nodiscard]] inline size_t get_remaining_chars() const {
            return _delegate.get_remaining_chars();
        }

        [[nodiscard]] inline size_t get_remaining_chars(size_t max) {
            return _delegate.get_remaining_chars(max);
        }

        [[nodiscard]] inline const char* get_head() const noexcept {
            return _delegate.get_head();
        }

        [[nodiscard]] inline LineOffset get_line_offset() const noexcept {
            return _line_offset;
        }

        //Operators
        [[nodiscard]] inline value_type operator *() const {
            return *_delegate;
        }

        inline TextIterator& operator ++() {
            size_t len;
            const auto current = utf8::to_unicode(_delegate._head, len);

            switch(current) {
                case static_cast<uint64_t>('\t'):
                    _line_offset.offset += _TAB_SIZE;
                    break;
                case static_cast<uint64_t>('\b'):
                    karmac_unimplemented();
                    break;
                case static_cast<uint64_t>('\r'):
                    _last_line_offsets.push(_line_offset.offset);
                    _line_offset.offset = 0;
                    break;
                case static_cast<uint64_t>('\n'):
                    _last_line_offsets.push(_line_offset.offset);
                    _line_offset.offset = 0;
                    ++_line_offset.line;
                    break;
                default:
                    ++_line_offset.offset;
                    break;
            }

            _delegate._last_offsets.push(len);
            _delegate._head += len;

            return *this;
        }

        inline TextIterator& operator --() {
            karmac_assert(!_delegate._last_offsets.empty());
            const auto len = _delegate._last_offsets.top();
            _delegate._last_offsets.pop();

            _delegate._head -= len;

            const auto current = *_delegate;
            switch(current) {
                case static_cast<uint64_t>('\t'):
                    _line_offset.offset -= _TAB_SIZE;
                    break;
                case static_cast<uint64_t>('\b'):
                    karmac_unimplemented();
                    break;
                case static_cast<uint64_t>('\r'):
                    _line_offset.offset = _last_line_offsets.top();
                    _last_line_offsets.pop();
                    break;
                case static_cast<uint64_t>('\n'):
                    _line_offset.offset = _last_line_offsets.top();
                    _last_line_offsets.pop();
                    --_line_offset.line;
                    break;
                default:
                    --_line_offset.offset;
                    break;
            }

            return *this;
        }

        inline TextIterator& operator +=(size_t count) {
            for(auto i = 0; i < count; i++) {
                ++(*this);
            }

            return *this;
        }

        inline TextIterator& operator -=(size_t count) {
            for(auto i = 0; i < count; i++) {
                --(*this);
            }

            return *this;
        }

        [[nodiscard]] inline TextIterator operator ++(int) {
            auto iterator = *this;
            ++(*this);
            return iterator;
        }

        [[nodiscard]] inline TextIterator operator --(int) {
            auto iterator = *this;
            --(*this);
            return iterator;
        }

        [[nodiscard]] inline value_type operator [](size_t index) const {
            return _delegate[index];
        }

        [[nodiscard]] inline bool operator ==(const TextIterator& other) const noexcept {
            return _delegate == other._delegate && _last_line_offsets == other._last_line_offsets && _line_offset == other._line_offset;
        }

        [[nodiscard]] inline bool operator !=(const TextIterator& other) const noexcept {
            return _delegate != other._delegate || _last_line_offsets != other._last_line_offsets || _line_offset != other._line_offset;
        }
    };
}