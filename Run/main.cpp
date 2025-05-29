// LAB/main.cpp

#include <iostream>
#include <RK/Array.h>
#include <RK/String.h>

int main(int argc, char *argv[])
{
    using namespace std;

    const Array<Float32> arr = { 65, 66, 67, 68, 69 };
    cout << "Array = " << arr << endl;

    const RK::String s = { 'R', 'A', 'M', 'E', 'S', 'H', '\0' };
    cout << "String = " << s << endl;

    cout << "\n";
    return 0;

}