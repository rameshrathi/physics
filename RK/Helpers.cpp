//
// Created by ramesh on 30/05/25.
//

#include "Helpers.h"

namespace RK {

template <typename T>
T && copy(T & obj) {
    T tmp = obj;
    return tmp;
}

template<typename T>
void swap(T & obj1, T & obj2) noexcept {
    T tmp = obj1;
    obj1 = obj2;
    obj2 = tmp;
}

} // RK