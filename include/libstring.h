/**
 * @file
 * @brief Header file for libstring.
 */
#ifndef _LIBSTRING_INC
#define _LIBSTRING_INC
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/**
 * The "unit" of each string
 * For non-unicode, it is a char
 */
#define string_unit

#ifdef USE_UNICODE
#undef string_unit
#define string_unit uint8_t

#error I have no idea how to implement unicode, but this library should be unicode-safe if you change string_unit
int utf8_strlen(const string_unit *str);

/**
 * Internal use strlen
 */
#define _strlen(x) strlen(x)
#else

#undef string_unit
#define string_unit char

/**
 * Internal use strlen
 */
#define _strlen(x) strlen(x)
#endif

/**
 * Main string type used by the library
 * 
 * Most of the time, the user interacts only with the "string".
 * Internally, however, this is how strings are represented.
 * 
 * The value of the string itself follows after this small header.
 * That is the memory address returned by a function.
 * 
 * IMPORTANT DISAMBIGUATION:
 * 
 * * size = size in memory
 * * length = the length of the string (not including null terminator)
 * 
 * string_real.len is a length!
 * string_real.tot is a size!
 */
typedef struct _string_real {
    /** length of the string */
    uint16_t        len;
    /** total amount of memory allocated to the string */
    uint16_t        tot;
    /** flag telling if this string should be free'd after use */
    unsigned char   tmp;
    /** flag telling that this is a string */
    unsigned char   flg;
    /* string itself follows... */
} string_real;

/** a "string" (aka array of whatever type string_unit is) */
typedef string_unit* string;

/**
 * Some random magic number used to check if a string passed to one of the
 * library functions is library-compatible string or not (ie, it has the header
 * that gives some extra information about it)
 */
#define SR_MAGIC_NUMBER 1

/**
 * Convert from a string to the string_real header
 * Simply done by going backwards to reach the header
 */
#define string_to_sr(x) ((string_real *)((unsigned char *)x - sizeof(string_real)))
/**
 * Convert from a string_real header to the string
 * Simply done by going forwards to reach the start of the string
 */
#define sr_to_string(x) ((string)((unsigned char *)x + sizeof(string_real)))
/**
 * Check if a string is a library-specific string by checking if it has this
 * magic number specified
 */
#define is_sr(x) (((string_real *)((unsigned char *)x - sizeof(string_real)))->flg == SR_MAGIC_NUMBER)

string string_new();
string string_realloc(string a, uint16_t minS);
uint16_t string_length(string a);
void string_free(string a);
string string_copy(string a, const string b, uint16_t offset, uint16_t num);
string string_append(string a, string b);
string string_dup(const string a);
string string_temporary(string in);
string string_mknew(const string_unit *in);
string string_appendv(int count, ...);
string string_appendnv(int count, ...);
int string_istemporary(string a);

/**
 * Create a new string with a minimum given size
 * 
 * @return pointer to the new string
 */
#define string_new_size(x) string_realloc(NULL, x)

#endif
