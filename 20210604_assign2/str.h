/*  20210604 정현우
    assignmnet #2
    str.h   */

#ifndef _STR_H_
#define _STR_H_
#include <limits.h> /* for LONG_MIN, LONG_MAX */
#include <unistd.h> /* for typedef of size_t */

/* Part 1 */
/* recieve pointer of a string 
and return number of characters in the string */
size_t StrGetLength(const char* pcSrc);

/* recieve pointer of source string and destination string
copies source string to destination and return it */
char *StrCopy(char *pcDest, const char* pcSrc);

/* recieve pointer of two source strings
return 0 if two strings are identical
if not, return difference of character */
int StrCompare(const char* pcS1, const char* pcS2);

/* recieve pointer of haystack strings and a character
return first location of the character in haystack
return NULL if no character found */
char *StrFindChr(const char *pcHaystack, int c);

/*recieve pointer of haystack strings and a needle string
return first location of needle in haystack
return NULL if no needle found
return haystack location when needle is empty */
char *StrFindStr(const char* pcHaystack, const char *pcNeedle);

/*recieve pointer of destination string and source string
paste source string after the destination string and return it*/
char *StrConcat(char *pcDest, const char* pcSrc);

/* recieve pointer of string
skip initial space, and read sign and number in character type
return number in long int type
return LONG_MAX/LONG_MIN if overflow/underflow */
long int StrToLong(const char *nptr, char **endptr, int base);

#endif /* _STR_H_ */