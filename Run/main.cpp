// LAB/main.cpp
#include <RK/Types.h>
#include <RK/String.h>
#include <RK/Helpers.h>
#include <iostream>
#include <ranges>
#include <type_traits>
#include <concepts>

// Ex1
template<typename T> requires std::is_floating_point<T>::value
T absolute(T value) {
    return value < 0 ? -value : value;
}

template<typename  C> requires std::is_integral<C>::value
Size count_(const C& c) {
    int num = c;
    int count = 0;
    while (num > 0) {
        count++;
        num /= 10;
    }
    return count;
}

int main(int argc, char *argv[])
{
    using namespace std;
    using namespace RK;

    cout << "COUNT : " << count_(123456789) << endl;

    cout << "\n";
    return 0;

}