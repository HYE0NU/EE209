/*--------------------------------------------------------------------*/
/* userdef.h                                                          */
/* Author: 20210604 Jung Hyeonu                                       */
/*                                                                    */
/*--------------------------------------------------------------------*/

#ifndef USERDEF_INCLUDED
#define USERDEF_INCLUDED

enum {FALSE, TRUE};
enum TokenType {TOKEN, TOKEN_SPECIAL};
typedef struct DynArray * DynArray_T;

struct Token // from input.c - to prevent inefficient inclusion
{
   /* The type of the token. */
   enum TokenType eType;
   /* The string which is the token's value. */
   char *pcValue;
};
typedef struct Token * Token_T;

#endif