/*--------------------------------------------------------------------*/
/* execution.c                                                        */
/* Author: 20210604 Jung Hyeonu                                       */
/* Syntactically analyze tokens and execute commands                  */
/*--------------------------------------------------------------------*/

#include "execute.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dynarray.h"
#include "userdef.h"

/*--------------------------------------------------------------------*/

//int parse(DynArray_T oTokens){}

int execute(DynArray_T oTokens)
{
    Token_T psToken;

    psToken = (Token_T) DynArray_get(oTokens, 0);
    assert(psToken != NULL);

    if(strcmp(psToken->pcValue, "setenv") == 0)
    {}
    else if(strcmp(psToken->pcValue, "unsetenv") == 0)
    {}
    else if(strcmp(psToken->pcValue, "cd") == 0)
    {}
    else if(strcmp(psToken->pcValue, "exit") == 0)
    {
        exit(0);
    }
    else if(strcmp(psToken->pcValue, "fg") == 0)
    {}
    else
    {}

    return TRUE;
}