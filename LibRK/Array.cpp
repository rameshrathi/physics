//
// Created by ramesh on 19/05/25.
//

#include "Array.h"

template <typename T>
Array<T>& Array<T>::operator=(const Array& other) {
    if (this != &other) {
        T* new_data = new T[other._size];
        std::copy(other._data, other._data + other._size, new_data);
        delete[] _data;
        _data = new_data;
        _size = other._size;
    }
    return *this;
}

template <typename T>
Array<T>& Array<T>::operator=(Array&& other) noexcept {
    if (this != &other) {
        delete[] _data;
        _data = other._data;
        _size = other._size;
        other._data = nullptr;
        other._size = 0;
    }
    return *this;
}