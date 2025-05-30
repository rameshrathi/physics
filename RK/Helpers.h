//
// Created by ramesh on 30/05/25.
//

#pragma once

#include <iostream>

namespace RK
{

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
