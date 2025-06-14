//
// Created by ramesh on 30/05/25.
//

#include <Shared/Array.h>
#include <cassert>

int main() {

    Array<Int16> arr1 = { 10, 20, 30 };
    Array<Int16> arr2 = { 10, 20, 30 };
    assert(arr1 == arr2);

    arr1.resize(2);
    arr2.resize(2);

    assert(arr1.size() == arr2.size());

    for (int i = 0; i<2; i++) assert(arr1[i] == arr2[i]);


    return 0; // Success
}
