//
// Created by ramesh on 29/05/25.
//

#pragma once

#include <RK/Types.h>
#include <RK/Helpers.h>
#include <initializer_list>
#include <type_traits>

namespace RK {

// String
template <typename T>
struct StringImpl {
    using ValType = T;
    /* ---------  CONSTRUCTORS  --------- */
    ~StringImpl () {
        delete[] _data;
    }
    StringImpl () = default;

    StringImpl(const char* str) {
        const Size len = str_length(str);
        _data = new ValType[len+1];
        _size = len;
        for (Size index = 0; index < len; index++ ) {
            _data[index] = str[index];
        }
    }
    StringImpl(const std::initializer_list<ValType>& list) {
        _data = new ValType[list.size()+1];
        _size = list.size();
        Size index = 0;
        for (const ValType & item : list) {
            _data[index++] = item;
        }
    }
    // Copy
    StringImpl (const StringImpl& other) {
        _data = new ValType[other.size() + 1];
        _size = other.size();
        Size i = 0;
        for (const ValType & item : other) {
            _data[i++] = item;
        }
    }
    // Move
    StringImpl (StringImpl&& other) noexcept {
        _data = other._data;
        _size = other._size;
        other._data = nullptr;
        other._size = 0;
    }

    StringImpl& operator = (const StringImpl& other) {
        if (this == &other) { return *this; }
        delete[] _data;
        _data = new ValType[other._size + 1];
        _size = other._size;
        Size i = 0;
        for (const ValType & item : other) {
            _data[i++] = item;
        }
        return *this;
    }

    StringImpl& operator = (StringImpl&& other) noexcept {
        if (this == &other) { return *this; }
        delete[] _data;
        _data = other._data;
        _size = other._size;
        other._data = nullptr;
        other._size = 0;
        return *this;
    }

    /* --------------------------------------------------------- */
    Size size () const noexcept { return _size; }
    const ValType* value() const { return _data; }

    ValType& operator[] (Size index) const noexcept { return _data[index]; }
    void resize (const Size size) noexcept { _size = size; }

    ValType& operator[] (Size index) noexcept { return _data[index]; }

    // ---------  Iterators ----------------
    const ValType& begin () const noexcept { return _data; }
    const ValType& end () const noexcept { return _data + _size; }

    // Compare if ValType = Char
    template<typename U = ValType>
    std::enable_if_t<std::is_same<U, Char>::value, Bool>
    operator == ( const char *str ) const {
        return RK::str_compare(_data, str) == 0;
    }

    // Compare if ValType = Char
    template<typename U = ValType>
    std::enable_if_t<std::is_same<U, Char>::value, Bool>
    operator == ( const StringImpl & other ) const {
        return RK::str_compare(_data, other._data) == 0;
    }

    // Output stream
    friend std::ostream&  operator << (std::ostream& os, const StringImpl& array) {
        for (Size i = 0; i < array.size(); i++) {
            os << array[i];
        }
        return os;
    }

    // Swap

private:
    /* ---------  DATA  --------- */
    ValType *_data = nullptr;
    Size _size = 0;
};

using String = StringImpl<Char>;

} // RK