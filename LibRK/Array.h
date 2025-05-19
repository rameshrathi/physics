//
// Created by ramesh on 09/05/25.
//

#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <initializer_list>
#include <algorithm>

template <typename T>
class Array {
public:
    Array() = default;

    explicit Array(std::size_t size)
        : _size(size), _data(new T[size]()) {}

    Array(std::initializer_list<T> list)
        : _size(list.size()), _data(new T[_size]) {
        std::copy(list.begin(), list.end(), _data);
    }

    Array(const Array& other)
        : _size(other._size), _data(new T[other._size]) {
        std::copy(other._data, other._data + other._size, _data);
    }

    Array(Array&& other) noexcept
        : _size(other._size), _data(other._data) {
        other._size = 0;
        other._data = nullptr;
    }

    Array& operator=(const Array& other);

    Array& operator=(Array&& other) noexcept;

    ~Array() {
        delete[] _data;
    }

    T& operator[](std::size_t index) {
        return _data[index];
    }

    const T& operator[](std::size_t index) const {
        return _data[index];
    }

    T& at(std::size_t index) {
        if (index >= _size) throw std::out_of_range("Index out of bounds");
        return _data[index];
    }

    const T& at(std::size_t index) const {
        if (index >= _size) throw std::out_of_range("Index out of bounds");
        return _data[index];
    }

    std::size_t size() const noexcept {
        return _size;
    }

    T* data() noexcept {
        return _data;
    }

    const T* data() const noexcept {
        return _data;
    }

    void fill(const T& value) {
        std::fill(_data, _data + _size, value);
    }

private:
    std::size_t _size = 0;
    T* _data = nullptr;
};
