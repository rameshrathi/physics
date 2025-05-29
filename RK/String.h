//
// Created by ramesh on 29/05/25.
//

#pragma once

#include <RK/Types.h>
#include <RK/Array.h>

namespace RK {
namespace impl {

// String
template <typename T>
class String {
public:
    ~String () {
        delete[] _data;
    }

    String () = default;

    String(const char * str) {
        const Size len = strlen(str);
        _data = new T[len+1];
        _size = len;
        for (Size index = 0; index < len; index++ ) {
            _data[index++] = str[index];
        }
    }

    String(const std::initializer_list<T> & list) {
        _data = new T[list.size()+1];
        _size = list.size();
        Size index = 0;
        for (const T & item : list) {
            _data[index++] = item;
        }
    }

    // Copy
    String (const String& other) {
        _data = new T[other.size() + 1];
        _size = other.size();
        Size i = 0;
        for (const T & item : other) {
            _data[i++] = item;
        }
    }
    String& operator = (const String& other) {
        if (this == &other) {
            return *this;
        }
        T *newData = new T[other.size() + 1];
        delete [] _data;
        _size = other.size();
        _data = newData;
        Size i = 0;
        for (const T & item : other) {
            _data[i++] = item;
        }
        return *this;
    }

    // Move
    String (String&& other) noexcept {
        _data = other._data;
        _size = other._size;
        other._data = nullptr;
        other._size = 0;
    }
    String& operator = (String&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        _data = other._data;
        _size = other._size;
        other._data = nullptr;
        other._size = 0;
        return *this;
    }

    /* --------------------------------------------------------- */
    Size size () const noexcept { return _size; }

    T& operator[] (Size index) const noexcept { return _data[index]; }

    T* data () const noexcept { return _data; }

    void resize (const Size size) noexcept { _size = size; }

    // ---------  Iterators ----------------
    T* begin () const noexcept { return _data; }
    T* end () const noexcept { return _data + _size; }

    // Output stream
    friend std::ostream& operator << (std::ostream& os, const String& array) {
        for (Size i = 0; i < array._size; i++) {
            os << array._data[i];
        }
        return os;
    }

private:
    T *_data = nullptr;
    Size _size = 0;
};

} // impl

using String = impl::String<Char>;

} // RK