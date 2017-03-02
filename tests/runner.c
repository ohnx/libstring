#include <stdio.h>
#include "libstring.h"
#include "libstring_printf.h"

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
    
    myString = string_copy(myString, "Steve", 24, 0);
    
    printf("First string is named Steve now, Second string unchanged\n");
    printf("s1(%p):%s\n", myString, myString);
    printf("s2(%p):%s\n", myStringTwo, myStringTwo);

    string_free(myString);
    myString = string_new();
    myString = string_copy(myString, "completely different... Cheeseburgers!", 0, 20);
    
    printf("Re-using first string!\n");
    printf("s1_new(%p):%s\n", myString, myString);

    string autofree_test = string_append("My numbers are ", string_temporary(string_printf("%d, %d, and %d.\n", 10, 30, 21)));
    printf("printf and autofree test:\n%s", autofree_test);

    string multia_test = string_appendv(6, "The", " quick", " brown", " fox", "\n", string_temporary(autofree_test));
    printf("multi-argument append test:\n%s", multia_test);

    string_free(myString);
    string_free(myStringTwo);
    string_free(multia_test);
    return 0;
}
