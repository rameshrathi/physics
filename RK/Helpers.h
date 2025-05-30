//
// Created by ramesh on 30/05/25.
//

#pragma once

namespace RK
{

template <typename T>
T && copy(T & obj);

template<typename T>
void swap(T & obj1, T & obj2) noexcept;

} // RK
