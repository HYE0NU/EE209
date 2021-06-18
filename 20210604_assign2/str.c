/*  20210604 정현우
    assignmnet #2
    str.c   */

#include <assert.h> /* to use assert() */
#include <ctype.h> /* to use isspace() */
#include <stdio.h> /* to use fprintf */
#include <errno.h> /* to use errno */
#include <limits.h> /* to use LONG_MIN, LONG_MAX */
#include "str.h"

/* Part 1 */

/* recieve pointer of a string 
and return number of characters in the string */
size_t StrGetLength(const char* pcSrc)
{
    const char *pcEnd;
    assert(pcSrc); /* NULL address, 0, and FALSE are identical. */
    pcEnd = pcSrc;
	
    while (*pcEnd) /* null character and FALSE are identical. */
        pcEnd++;
    return (size_t)(pcEnd - pcSrc);
}


/* recieve pointer of source string and destination string
copies source string to destination and return it */
char *StrCopy(char *pcDest, const char* pcSrc)
{
    char* pcDestCpy;
    assert(pcSrc && pcDest);
    pcDestCpy=pcDest;

    while(1){
        *pcDest=*pcSrc;
        if(*pcSrc == '\0') break;
        pcDest++;
        pcSrc++;
    }

    return pcDestCpy;
}

/* recieve pointer of two source strings
return 0 if two strings are identical
if not, return difference of character */
int StrCompare(const char* pcS1, const char* pcS2)
{
    int diff;
    assert(pcS1 && pcS2);

    while(*pcS1 != '\0' || *pcS2 != '\0'){
        diff = *pcS1-*pcS2;
        if(diff !=0) return diff;
        pcS1++;
        pcS2++;
    }
    
    return 0;
}

/* recieve pointer of two source strings and length to compare
return 0 if two strings are identical
if not, return difference of character
WARNING: function does not consider exceeding NULL point */
int StrNCompare(const char* pcS1, const char* pcS2, size_t len)
{
    int diff, i;
    assert(pcS1 && pcS2);

    for(i=0;i<len;i++){
        diff = *pcS1-*pcS2;
        if(diff !=0) return diff;
        pcS1++;
        pcS2++;
    }

    return 0;
}

/* recieve pointer of haystack strings and a character
return first location of the character in haystack
return NULL if no character found */
char *StrFindChr(const char* pcHaystack, int c)
{
    char* pcNeedle;
    assert(pcHaystack);
    pcNeedle = (char*)pcHaystack;

    while(1){
        if(*pcNeedle == c) return pcNeedle;
        if(*pcNeedle == '\0') return NULL;
        pcNeedle++;
    }
}

/*recieve pointer of haystack strings and a needle string
return first location of needle in haystack
return NULL if no needle found
return haystack location when needle is empty */
char *StrFindStr(const char* pcHaystack, const char *pcNeedle)
{
    assert(pcHaystack && pcNeedle);
    size_t needlelen;
    
    needlelen = StrGetLength(pcNeedle);
    if(*pcNeedle == '\0') return (char*)pcHaystack;

    while(*pcHaystack){
        if(*pcHaystack == *pcNeedle){
            if(StrNCompare(pcHaystack, pcNeedle, needlelen) == 0){
                return (char*)pcHaystack;
            }
        }
        pcHaystack++;
    }
 
    return NULL;
}

/*recieve pointer of destination string and source string
paste source string after the destination string and return it*/
char *StrConcat(char *pcDest, const char* pcSrc)
{
    char* pcDestCpy;
    assert(pcDest && pcSrc);
    pcDestCpy = pcDest;

    while(*pcDest){
        pcDest++;
    }
    while(*pcSrc){
        *pcDest = *pcSrc;
        pcDest++;
        pcSrc++;
    }
    *pcDest=*pcSrc;
    
    return pcDestCpy;
}

/* recieve pointer of string
skip initial space, and read sign and number in character type
return number in long int type
return LONG_MAX/LONG_MIN if overflow/underflow */
long int StrToLong(const char *nptr, char **endptr, int base)
{
    /* handle only when base is 10 */
    if (base != 10){
        errno = EINVAL;
        return 0;
    }

    unsigned long int value=0, flowval;
    long int result;
    int digit, flowdgt, neg = 0;
    assert(nptr);
    
    /* save non-digit pointer */
    if(endptr!=NULL){
        *endptr = (char*)nptr;
    }

    /* skip space (' ', '\t', '\n', ...)*/
    while(*nptr && isspace(*nptr)) nptr++;
    /* read sign */
    if(*nptr == '+'){
        nptr++;
    }
    else if(*nptr == '-'){
        neg = 1;
        nptr++;
    }
    /* if there are no digits */
    else if(!isdigit(*nptr)){
        errno = EINVAL;
        return 0;
    }
    /* overflow/underflow threshold setting */
    flowval = neg? -(unsigned long)LONG_MIN : (unsigned long)LONG_MAX;
    flowdgt = flowval % base;
    flowval /= base;

    /* read number and handle overflow/underflow */
    while(isdigit(*nptr)){
        digit = *nptr-'0';
        if(value>flowval){
            errno=ERANGE;
            if(!neg) return LONG_MAX;
            else return LONG_MIN;
        }
        else if(value==flowval && digit>=flowdgt){
            errno=ERANGE;
            if(!neg) return LONG_MAX;
            else return LONG_MIN;
        }
        value*=base;
        value+=(digit);
        nptr++;
        if(endptr!=NULL) *endptr = (char*)nptr;
    }
    
    result = (long)value;
    if(neg) result *= -1;
    return result;
}