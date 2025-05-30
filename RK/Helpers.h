//
// Created by ramesh on 30/05/25.
//

#pragma once

#include <RK/Types.h>
#include <iostream>

namespace RK
{

inline Size str_length(const char * str) {
    const char *ptr = str;
    while (*ptr != '\0')
        ptr++;
    return ptr - str;
}

inline Size str_compare(const char * s1, const char * s2) {
    if (*s1 != *s2) return 0;
    while (*++s1 == *++s2 && (*s1 != '\0' && *s2 != '\0'));
    if (*s1 == '\0' && *s2 == '\0') return 0;
    int miss = 0;
    while (*s1 && *s2 && *s1++ != *s2++) miss++;
    return miss;
}

template <typename T>
T && copy(T & obj) {
    T tmp = obj;
    return std::move(tmp);
}

template<typename T>
void swap(T& obj1, T& obj2) noexcept {
    T tmp = std::move(obj1);
    obj1 = std::move(obj2);
    obj2 = std::move(tmp);
}

} // RK
