/*--------------------------------------------------------------------*/
/* execution.h                                                        */
/* Author: 20210604 Jung Hyeonu                                       */
/* Syntactically analyze tokens and execute commands                  */
/*--------------------------------------------------------------------*/

#ifndef EXECUTE_INCLUDED
#define EXECUTE_INCLUDED

#include "userdef.h"

int execute(DynArray_T oTokens);

#endif