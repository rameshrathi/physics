//
// Created by ramesh on 30/05/25.
//

#include <RK/String.h>

namespace RK::impl {

template <typename T>
String<T>::String(const char * str) {
    const Size len = strlen(str);
    _data = new T[len+1];
    _size = len;
    for (Size index = 0; index < len; index++ ) {
        _data[index] = str[index];
    }
}

template <typename T>
String<T> :: String(const std::initializer_list<T> & list) {
    _data = new T[list.size()+1];
    _size = list.size();
    Size index = 0;
    for (const T & item : list) {
        _data[index++] = item;
    }
}

// Copy
template <typename T>
String<T> :: String (const String& other) {
    _data = new T[other.size() + 1];
    _size = other.size();
    Size i = 0;
    for (const T & item : other) {
        _data[i++] = item;
    }
}

// Move
template <typename T>
String<T> :: String (String&& other) noexcept {
    _data = other._data;
    _size = other._size;
    other._data = nullptr;
    other._size = 0;
}

template <typename T>
typename String<T> :: String & operator = (const String<T> & other) {
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

template <typename T>
std::ostream& String<T> :: operator << (std::ostream& os, const String& array) {
    for (Size i = 0; i < array.size(); i++) {
        os << array[i];
    }
    return os;
}

} // RK::Impl