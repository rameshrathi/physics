//
// Created by ramesh on 30/05/25.
//

#include <RK/Array.h>
#include <cassert>

int main() {

    Array<Int16> arr1 = { 10, 20, 30 };
    Array<Int16> arr2 = { 10, 20, 30 };
    assert(arr1 == arr2);


    return 0; // Success
}