#include <iostream>
#include <type_traits>

template <typename T>
std::enable_if_t<std::is_arithmetic<T>::value>
add_two_values(T first, T second) {
    std::cout << "SUM = " << first + second << std::endl;
}

template <typename T>
std::enable_if_t<std::is_arithmetic<T>::value, T>
add_values(T first, T second) {
  return first + second;
}

int main()
{
    std::cout << "Result : " << add_values(100, 200)  << std::endl;
    return 0;
}