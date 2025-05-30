//
// Created by ramesh on 30/05/25.
//

#include <RK/Helpers.h>
#include <cassert>
#include <RK/String.h>

int main() {

    const char * s1 = "Hello2";
    const char * s2 = "Hello1";
    assert(RK::str_compare(s1, s2) == 1);
    assert(RK::str_length(s1) == 6);

    const char * s3 = "Hello Mr Ramesh";
    assert(RK::str_compare(s3, s3) == 0);
    assert(RK::str_length(s3) == 15);

    RK::String r1 = "Ramesh";
    RK::String r2 = "Kumar";
    assert( r1[5] == 'h' );
    r1[5] = '\0';
    r1.resize(5);
    assert(r1.size() == 5);

    RK::swap(r1, r2);
    assert(r1 == "Kumar");
    assert(r2 == "Ramesh");

    return 0; // Success
}