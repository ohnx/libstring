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
 * The -1 just means "Screw it"
 */
uint16_t allocS[] = {16, 32, 64, 256, 512, 1024, 2048, 4096, 8192, 12288, -1};

/**
 * Create a new string
 * 
 * @return pointer to the new string
 */
string string_new() {
    string_real *sr = calloc(sizeof(string_real) + allocS[0], 1);
    sr->len = 0;
    sr->tot = allocS[0];
    sr->flg = SR_MAGIC_NUMBER;
    return sr_to_string(sr);
}

/**
 * Create a new string with a given size
 * 
 * @return pointer to the new string
 */
string string_new_size(uint16_t size) {
    string_real *sr = calloc(sizeof(string_real) + sizeof(string_unit)*size, 1);
    sr->len = 0;
    sr->tot = size;
    sr->flg = SR_MAGIC_NUMBER;
    return sr_to_string(sr);
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
        if (realS == -1) {
            realS = minS;
            goto escape;
        }
    }
    realS = allocS[i];
    
escape:
    /* reallocate string */
    ret = realloc(string_to_sr(a), sizeof(string_real) + sizeof(string_unit)*realS);
    
    /* clear the realloc'd memory */
    memset(sr_to_string(ret), 0, realS - ret->tot);
    
    /* change the total to reflect new size */
    ret->tot = realS;
    
    /* return the new string */
    return sr_to_string(ret);
}


/**
 * Copy string B to A with offset offset
 * 
 * a string #1
 * b string #2
 * offset offset
 * 
 * @return the new string (guaranteed to be library-compatible string)
 */
string string_copy(string a, const string b, uint16_t offset) {
    uint16_t aL, bL, rS;
    
    if (!is_sr(a)) {
        string ret;
        
        /* get lengths */
        aL = strlen(a);
        bL = strlen(b);
        
        /* find the final size required. first find length required 
         * 1) if length of a is greater than bL + offset, we just keep aL
         * 2) otherwise, the length required is offset + bL
         *
         * then, find the size (aka add one for null terminator)
         */
        rS = (aL >= (bL + offset) ? aL : offset + bL) + 1;
        
        /* not a valid string for this library, make it one */
        ret = string_new_size(rS);
        
        /* set the headers - tot and flag are both already set */
        string_to_sr(ret)->len = rS - 1; /* subtract one to get length */
        
        /* copy the original string first */
        memcpy(ret, a, sizeof(string_unit)*aL);
        
        /* copy the second string to where it should be */
        memcpy(ret+offset, b, sizeof(string_unit)*bL);
        
        return ret;
    } else {
        /* we know that a is for sure a valid string */
        aL = string_to_sr(a)->len;
        
        /* find length of b */
        if (!is_sr(b))
            /* no idea what the length is, use strlen */
            bL = strlen(b);
        else
            /* we know how long the string is */
            bL = string_to_sr(b)->len;
        
        /* find the final size required. first find length required 
         * 1) if length of a is greater than bL + offset, we just keep aL
         * 2) otherwise, the length required is offset + bL
         *
         * then, find the size (aka add one for null terminator)
         */
        rS = (aL >= (bL + offset) ? aL : offset + bL) + 1;
        
        if (rS > string_to_sr(a)->tot) /* need to reallocate memory for string */
            a = string_realloc(a, rS);
        
        /* set the headers */
        string_to_sr(a)->len = rS - 1; /* subtract one to get length */
        
        /* guarantee that there is enough room now, can copy string over */
        memcpy(a+offset, b, sizeof(string_unit)*bL);
        
        return a;
    }
}

/**
 * Make a duplicate of a string
 * 
 * a string
 * 
 * @return the copy of the string (guaranteed to be library-compatible string)
 */
string string_dup(string a) {
    string_real *ret;
    uint16_t aS, realS;
    unsigned int i = 0;
    
    if (!is_sr(a)) {
        aS = strlen(a) + 1; /* +1 for null terminator */
        
        /* loop through all the lengths */
        for (i = 0; allocS[i] < aS; i++) {
            realS = allocS[i];
            if (realS == -1) {
                realS = aS;
                goto escape;
            }
        }
        realS = allocS[i];
            
    escape:
        ret = string_to_sr(string_new_size(realS));
        ret->len = aS - 1; /* minus 1 to convert from size to length */

        /* can only copy to the string portion */
        memcpy(sr_to_string(ret), a, sizeof(string_unit)*aS);
    } else {
        aS = string_to_sr(a)->len + 1;
        
        ret = string_to_sr(string_new_size(aS));
        
        /* we can copy everything, even the header */
        memcpy(ret, string_to_sr(a), sizeof(string_real) + sizeof(string_unit)*aS);
    }
    
    return sr_to_string(ret);
}

/**
 * Append two strings
 * 
 * a string #1
 * b string #2
 * 
 * @return the appended string a+b (guaranteed to be library-compatible string)
 */
string string_append(string a, string b) {
    uint16_t aL;
    
    /* append is just copying to the end of the string A */
    if (!is_sr(a))
        aL = strlen(a);
    else
        aL = string_to_sr(a)->len;
    
    return string_copy(a, b, aL);
}
