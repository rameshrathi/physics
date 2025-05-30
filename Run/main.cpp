// LAB/main.cpp

#include <RK/Types.h>
#include <RK/String.h>
#include <RK/Helpers.h>

#include <iostream>
#include <cassert>

int main(int argc, char *argv[])
{
    using namespace std;
    using namespace RK;

    cout << "Size of UInt32 : " << sizeof(UInt32) << "\n";
    cout << "Size of UInt64 : " << sizeof(UInt64) << "\n";

    String s1 = "Hello Mr X";
    String s2 = "Hello Mr Y";

    // RK::swap(s1, s2);
    //
    // assert(s1 == s2);
    // assert(s1 == s2);

    cout << "\n";
    return 0;

}