/**
 * @file
 * @brief All functions in libstring.
 * 
 * 
 */
#include "libstring.h"

/*
 * !!! Important terminology clarification !!!
 * size: size in memory
 * length: the length of the string (not including null terminator)
 * 
 * string_real.len is a length!
 * string_real.tot is a size!
 */

/**
 * Allocation sizes by default <br />
 * Each one is ~double the previous one
 * 
 * The 0 just means allocate as much as needed, no extras
 */
uint16_t allocS[] = {16, 32, 64, 256, 512, 1024, 2048, 4096, 8192, 12288, 0};

/**
 * Create a new string
 * 
 * @return pointer to the new string
 */
string string_new() {
    string_real *sr = calloc(sizeof(string_real) + allocS[0], 1);
    sr->len = 0;
    sr->tot = allocS[0];
    sr->tmp = 0;
    sr->flg = SR_MAGIC_NUMBER;
    return sr_to_string(sr);
}

/**
 * Get the number of bytes available in the string
 * 
 * @return the number of bytes available
 */
uint16_t string_bavail(string a) {
    return is_sr(a) ? (string_to_sr(a)->tot) - (string_to_sr(a)->len) : 0;
}

/**
 * Reallocate string A with minimum size 
 * 
 * a the string to reallocate
 * minS the minimum size of the string
 * 
 * @return pointer to the new string
 */
string string_realloc(string a, uint16_t minS) {
    unsigned int i = 0;
    uint16_t realS;
    string_real *ret;
    
    /* loop through all the lengths */
    for (i = 0; allocS[i] < minS; i++) {
        realS = allocS[i];
        if (realS == 0) {
            realS = minS;
            goto escape;
        }
    }
    realS = allocS[i];
    
escape:
    if (a) {/* reallocate string */
        ret = realloc(string_to_sr(a), sizeof(string_real) + sizeof(string_unit)*realS);
        
        /* clear the realloc'd memory past the old string */
        memset(sr_to_string(ret) + ret->len, 0, realS - ret->len);
    } else {/* newly allocate the string */
        ret = calloc(sizeof(string_real) + sizeof(string_unit)*realS, 1);
        
        /* set the magic flag */
        ret->tmp = 0;
        ret->flg = SR_MAGIC_NUMBER;
        ret->len = 0;
    }
    
    /* change the total to reflect new size */
    ret->tot = realS;
    
    /* return the new string */
    return sr_to_string(ret);
}


/**
 * Copy num characters from string B to A with offset offset.
 * 
 * <b>if num is 0, then all characters will be copied.</b>
 * 
 * a string #1
 * b string #2
 * offset offset
 * num number
 * 
 * @return the new string (guaranteed to be library-compatible string)
 */
string string_copy(string a, const string b, uint16_t offset, uint16_t num) {
    uint16_t aL, bL, rS;
    string ret;

    /* get length of string a */
    aL = string_length(a);

    /* find length of b */
    bL = num == 0 ? string_length(b) : num;

    /* find the final size required. first find length required
     * 1) if length of a is greater than bL + offset, we just keep aL
     * 2) otherwise, the length required is offset + bL
     *
     * then, find the size (aka add one for null terminator)
     */
    rS = (aL >= (bL + offset) ? aL : offset + bL) + 1;
    
    if (!is_sr(a)) {
        /* not a valid string for this library, make it one */
        ret = string_new_size(rS);
        
        /* set the headers - tot and flag are both already set */
        string_to_sr(ret)->len = rS - 1; /* subtract one to get length */
        
        /* copy the original string first */
        memcpy(ret, a, sizeof(string_unit)*aL);
        
        /* copy the second string to where it should be */
        memcpy(ret+offset, b, sizeof(string_unit)*bL);
        
        /* check if string should be freed */
        if (string_istemporary(b)) string_free(b);

    } else {
        if (rS > string_to_sr(a)->tot) /* need to reallocate memory for string */
            a = string_realloc(a, rS);
        
        /* set the headers */
        string_to_sr(a)->len = rS - 1; /* subtract one to get length */
        
        /* guarantee that there is enough room now, can copy string over */
        memcpy(a+offset, b, sizeof(string_unit)*bL);

        /* check if string should be freed */
        if (string_istemporary(b)) string_free(b);

        ret = a;
    }

    return ret;
}

/**
 * Make a duplicate of a string
 * 
 * a string
 * 
 * @return the copy of the string (guaranteed to be library-compatible string)
 */
string string_dup(const string a) {
    string_real *ret;
    uint16_t aS;

    aS = string_length(a) + 1;
    ret = string_to_sr(string_new_size(aS));

    if (!is_sr(a)) {
        ret->len = aS - 1; /* minus 1 to convert from size to length */

        /* can only copy to the string portion */
        memcpy(sr_to_string(ret), a, sizeof(string_unit)*aS);
    } else {
        /* we can copy everything, even the header */
        memcpy(ret, string_to_sr(a), sizeof(string_real) + sizeof(string_unit)*aS);
    }
    
    /* check if string should be freed */
    if (string_istemporary(a)) string_free(a);
    return sr_to_string(ret);
}

/**
 * Declare a string as temporary, ie, that it will be freed right after use
 * 
 * If the string is not library-compatible, nothing happens.
 * 
 * @return the pointer to the string
 */
string string_temporary(string in) {
    /* we use an interesting hack here... assignment always returns what we
    assigned, so if we do some pointer arithmetic on it, we can get this macro
    to evaluate to the pointer passed in */
    return is_sr(in)?(in + (string_to_sr(in)->tmp = 1) - 1):in;
}

/**
 * Duplicate a non-library string
 * 
 * Use this function if you are concerned about reads before the string
 */
string string_mknew(const string_unit *in) {
    int length;
    string ret;

    /* create new string */
    length = _strlen(in);
    ret = string_new_size(length + 1);
    string_to_sr(ret)->len = length;

    /* copy data */
    memcpy(ret, in, sizeof(string_unit)*length);

    return ret;
}

/**
 * Append a variable number of strings
 * 
 * @return pointer to the new string
 */
string string_appendv(int count, ...) {
    va_list ap;
    string s;
    int i;

    va_start(ap, count); /* init argument list */

    s = string_new();

    for (i = 0; i < count; i++)
        s = string_append(s, va_arg(ap, string)); /* will autofree if it's temp */

    va_end(ap);
    return s;
}