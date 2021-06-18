/*--------------------------------------------------------------------*/
/* input.h                                                            */
/* Original Author: Bob Dondero                                       */
/* Illustrate lexical analysis using a deterministic finite state     */
/* automaton (DFA)                                                    */
/* modified by 20210604 Jung Hyeonu                                   */
/*--------------------------------------------------------------------*/

#ifndef INPUT_INCLUDED
#define INPUT_INCLUDED

typedef struct Token * Token_T;

#include "dynarray.h"
#include <stdio.h> // for FILE* type

#ifndef MAX_LINE_SIZE
#define MAX_LINE_SIZE 1024
#endif

// enum {FALSE, TRUE};  already declared in dynarray.h

int inputMain(FILE* fpInput);

#endif