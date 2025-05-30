//
// Created by ramesh on 29/05/25.
//

#pragma once

#include <RK/Types.h>
#include <ostream>
#include <initializer_list>
#include <type_traits>

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

    String(const char* str);
    String(const std::initializer_list<T>& list);

    String (const String& other);
    String (String&& other) noexcept;

    // String& operator = (const String& other);
    // String& operator = (String&& other) noexcept;

    /* --------------------------------------------------------- */
    Size size () const noexcept { return _size; }
    const T& value() { return _data; }

    T& operator[] (Size index) const noexcept { return _data[index]; }
    void resize (const Size size) noexcept { _size = size; }

    // ---------  Iterators ----------------
    T* begin () const noexcept { return _data; }
    T* end () const noexcept { return _data + _size; }

    // Compare if T = Char
    template<typename U = T>
    std::enable_if_t<std::is_same<U, Char>::value, Bool>
    operator == ( const String<U>& other ) const {
        return std::strcmp(value(), other.value()) == 0;
    }

    // Output stream
    // friend std::ostream&  operator << (std::ostream& os, const String& array);

private:
    T *_data = nullptr;
    Size _size = 0;
};

} // impl

using String = impl::String<Char>;

} // RK