#include <iostream>

// calculating factorial at compile-time
template <int N> struct Factorial
{
    static const int value = N * Factorial<N - 1>::value;
};

// base case (Factorial<0>)
template <> struct Factorial<0>
{
    static constexpr int value = 1;
};

int main()
{
    // Factorial computation
    // happens at compile-time
    std::cout << "Factorial of 5 is: " << Factorial<5>::value;
    return 0;
}