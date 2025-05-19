//
// Created by ramesh on 09/05/25.
//

#pragma once

#include <initializer_list>

namespace RK {
    //----------------------------------------------------------

template<typename ValueType>
class Array {

public:
    Array(std::initializer_list<ValueType> values)
        : _values(values), _size(values.size())
        {}

    size_t size() const { return _size; }

    Array(const Array& other) {
        const size_t size = other._size;
        _values = new ValueType[size];
        _size = size;
        memcpy(_values, other._values, _size * sizeof(ValueType));
    }

    Array& operator=(const Array& other) {
        if (this != &other) {
            const size_t size = other._size;
            delete[] _values;
            _values = new ValueType[size];
            _size = size;
            memcpy(_values, other._values, _size * sizeof(ValueType));
        }
        return *this;
    }

private:
    ValueType *_values;
    size_t _size;
};

} // RK Namespace
