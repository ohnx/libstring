/**
 * @file
 * @brief Extra function for string_printf
 * 
 * 
 */
#include "libstring.h"

/**
 * sprintf
 * 
 * Give it exactly what you'd give printf
 * 
 * @return a pointer to a library-compatible string
 */
string string_printf(const char *fmt, ...) {
    va_list in, copy;
    string a = NULL;
    int size;

    /* start the main input */
    va_start(in, fmt);
    /* make a copy of the input */
    va_copy(copy, in);

    /* run vsnprintf just to see how much bytes needed */
    size = vsnprintf(a, 0, fmt, copy) + 1;
    va_end(copy);

    /* allocate the amount now */
    a = string_new_size(size);
    string_to_sr(a)->len = size - 1;

    /* re-run */
    vsnprintf(a, size, fmt, in);
    va_end(in);

    /* done */
    return a;
}
