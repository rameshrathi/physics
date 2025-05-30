// LAB/main.cpp

#include <RK/Types.h>
#include <RK/String.h>
#include <RK/Helpers.h>

#include <iostream>
#include <cassert>

template <typename T>
void show(T & o) {
    std::cout << "Called" << "\n";
}

int main(int argc, char *argv[])
{
    using namespace std;
    using namespace RK;

    cout << "Size of UInt32 : " << sizeof(UInt32) << "\n";
    cout << "Size of UInt64 : " << sizeof(UInt64) << "\n";

    String s1 = "X1";
    String s2 = "X2";

    cout << "S1 : " << s1 << "\n";
    cout << "S2 : " << s2 << "\n";
    RK::swap(s1, s2);
    cout << "S1 : " << s1 << "\n";
    cout << "S2 : " << s2 << "\n";

    assert(s1 == "X2");
    assert(s2 == "X1");

    cout << "\n";
    return 0;

}