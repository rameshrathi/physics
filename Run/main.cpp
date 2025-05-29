// LAB/main.cpp

#include <iostream>
#include <RK/Array.h>
#include <RK/String.h>

#include <assert.h>

int main(int argc, char *argv[])
{
    using namespace std;

    const Array<Float32> arr = { 65, 66, 67, 68, 69 };
    cout << "Array = " << arr << endl;

    assert(arr.size() == 5);

    const RK::String s = "Ramesh Kumar";
    cout << "String = " << s << endl;

    assert(s.size() == 12);

    cout << "\n";
    return 0;

}