//
// Created by ramesh on 29/05/25.
//

#pragma once

#include <iostream>
#include <initializer_list>
#include <RK/Types.h>

template <typename T>
class Array {

public:
    ~Array () {
        delete[] _data;
    }

    Array () = default;

    Array(const std::initializer_list<T> & list) {
        _data = new T[list.size()];
        _size = list.size();
        Size i = 0;
        for (const T & item : list) {
            _data[i++] = item;
        }
    }

    // Copy
    Array (const Array& other) {
        _data = new T[other._size];
        _size = other._size;
        Size i = 0;
        for (const T & item : other)
            _data[i++] = item;
    }
    Array& operator = (const Array& other) {
        if (this == &other) {
            return *this;
        }
        T *newData = new T[other._size];
        delete[] _data;

        for (Size i = 0; i < other._size; i++) {
            newData[i] = other._data[i];
        }
        _size = other._size;
        _data = newData;
        return *this;
    }

    // Move
    Array (Array&& other) noexcept {
        _data = other._data;
        _size = other._size;
        other._data = nullptr;
        other._size = 0;
    }
    Array& operator = (Array&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        delete [] _data;
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
    std::ostream& operator << (std::ostream& os, const Array& array) {
        for (Size i = 0; i < array.size(); i++) {
            os << array[i];
        }
        return os;
    }

private:
    T *_data = nullptr;
    Size _size = 0;
};

