#include <stdio.h>
#include "libstring.h"

int main() {
    printf("In this version of libstring, header is %ld bytes.\n", sizeof(string_real));

    string myString = string_append("Hello there, my name is ", "Mason! What is your name?");
    string myStringTwo = string_dup(myString);

    printf("Both strings are the same:\n");
    printf("s1(%p):%s\n", myString, myString);
    printf("s2(%p):%s\n", myStringTwo, myStringTwo);

    myStringTwo = string_append(myStringTwo, " Testing string manipulations!");
    
    printf("First string unchanged, Second string has added value\n");
    printf("s1(%p):%s\n", myString, myString);
    printf("s2(%p):%s\n", myStringTwo, myStringTwo);
    
    myString = string_copy(myString, "Steve", 24);
    
    printf("First string is named Steve now, Second string unchanged\n");
    printf("s1(%p):%s\n", myString, myString);
    printf("s2(%p):%s\n", myStringTwo, myStringTwo);

    string_free(myString);
    string_free(myStringTwo);
    return 0;
}