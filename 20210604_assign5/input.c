/*--------------------------------------------------------------------*/
/* input.c                                                            */
/* Original Author: Bob Dondero                                       */
/* Illustrate lexical analysis using a deterministic finite state     */
/* automaton (DFA)                                                    */
/* modified by 20210604 Jung Hyeonu                                   */
/*--------------------------------------------------------------------*/

#include "input.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dynarray.h"
#include "execute.h"
#include "userdef.h"

//enum {FALSE, TRUE};
//enum TokenType {TOKEN, TOKEN_SPECIAL};
extern char* g_programName;

/*--------------------------------------------------------------------*/

/* A Token is either a number or a word, expressed as a string. */
/*
struct Token
{
   //The type of the token.
   enum TokenType eType;
   //The string which is the token's value.
   char *pcValue;
};
*/
/*--------------------------------------------------------------------*/

static void freeToken(void *pvItem, void *pvExtra)

/* Free token pvItem.  pvExtra is unused. */

{
   struct Token *psToken = (struct Token*)pvItem;
   free(psToken->pcValue);
   free(psToken);
}

/*--------------------------------------------------------------------*/

static void printToken(void *pvItem, void *pvExtra)

/* Print token pvItem to stdout iff it is a number.  pvExtra is
   unused. */

{
   struct Token *psToken = (struct Token*)pvItem;
   printf("%s\n", psToken->pcValue);
}

/*--------------------------------------------------------------------*/

static struct Token *makeToken(enum TokenType eTokenType, char *pcValue){

/* Create and return a Token whose type is eTokenType and whose
   value consists of string pcValue.  Return NULL if insufficient
   memory is available.  The caller owns the Token. */

   struct Token *psToken;

   psToken = (struct Token*)malloc(sizeof(struct Token));
   if (psToken == NULL)
      return NULL;

   psToken->eType = eTokenType;

   psToken->pcValue = (char*)malloc(strlen(pcValue) + 1);
   if (psToken->pcValue == NULL)
   {
      free(psToken);
      return NULL;
   }

   strcpy(psToken->pcValue, pcValue);

   return psToken;
}

/*--------------------------------------------------------------------*/

static int lexLine(const char *pcLine, DynArray_T oTokens)

/* Lexically analyze string pcLine.  Populate oTokens with the
   tokens that pcLine contains.  Return 1 (TRUE) if successful, or
   0 (FALSE) otherwise.  In the latter case, oTokens may contain
   tokens that were discovered before the error. The caller owns the
   tokens placed in oTokens. */

/* lexLine() uses a DFA approach.  It "reads" its characters from
   pcLine. */

{
   enum LexState {STATE_START, STATE_IN_STRING, STATE_IN_TOKEN};

   enum LexState eState = STATE_START;

   int iLineIndex = 0;
   int iValueIndex = 0;
   char c;
   char acValue[MAX_LINE_SIZE];
   struct Token *psToken;

   assert(pcLine != NULL);
   assert(oTokens != NULL);

   for (;;)
   {
      /* "Read" the next character from pcLine. */
      c = pcLine[iLineIndex++];

      switch (eState)
      {
         case STATE_START:
            if ((c == '\n') || (c == '\0'))
               return TRUE;
            else if (c=='\"')
            {
               eState = STATE_IN_STRING;
            }
            else if ((c == ' ') || (c == '\t'))
            {
               eState = STATE_START;
            }
            else if(c == '|')
            {
               acValue[iValueIndex++] = c;
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_SPECIAL, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "%s: Cannot allocate memory\n", g_programName);
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "%s: Cannot allocate memory\n", g_programName);
                  return FALSE;
               }
               iValueIndex = 0;
               eState = STATE_START;
            }
            else
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_TOKEN;
            }
            break;

         case STATE_IN_TOKEN:
            if (isspace(c) || c == '\0')
            {
               /* Create a token. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "%s: Cannot allocate memory\n", g_programName);
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "%s: Cannot allocate memory\n", g_programName);
                  return FALSE;
               }
               iValueIndex = 0;

               if(c == '\n' || c == '\0')
               {
                  return TRUE;
               }

               eState = STATE_START;
            }
            else if(c == '\"')
            {
               eState = STATE_IN_STRING;      
            }
            else if(c == '|')
            {
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "%s: Cannot allocate memory\n", g_programName);
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "%s: Cannot allocate memory\n", g_programName);
                  return FALSE;
               }
               iValueIndex = 0;

               acValue[iValueIndex++] = c;
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_SPECIAL, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "%s: Cannot allocate memory\n", g_programName);
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "%s: Cannot allocate memory\n", g_programName);
                  return FALSE;
               }
               iValueIndex = 0;
               eState = STATE_START;
            }
            else
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_TOKEN;
            }
            break;
         case STATE_IN_STRING:
            if(c == '\n' || c == '\0')
            {
               fprintf(stderr, "%s: Could not find quote pair\n", g_programName);
               return FALSE;
            }
            else if(c == '\"')
            {
               eState = STATE_IN_TOKEN;
            }
            else
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_STRING;
            }
            break;

         default:
            assert(FALSE);
      }
   }
}

/*--------------------------------------------------------------------*/

int inputMain(FILE* fpInput)
{
   char pcLine[MAX_LINE_SIZE];
   DynArray_T oTokens;
   int iSuccessful;

   while(TRUE)
   {
      if(fpInput == stdin) // stdin
      {
         printf("%% ");
         if(fgets(pcLine, MAX_LINE_SIZE, fpInput) == NULL) break;
      }
      else // .ishrc
      {
         if(fgets(pcLine, MAX_LINE_SIZE, fpInput) == NULL) break;
         printf("%% %s", pcLine);
      }

      oTokens = DynArray_new(0);
      if (oTokens == NULL)
      {
         fprintf(stderr, "Cannot allocate memory\n");
         exit(EXIT_FAILURE);
      }

      iSuccessful = lexLine(pcLine, oTokens);
      if (iSuccessful)
      {
         // Syntactic Analysis & Execution
         DynArray_map(oTokens, printToken, NULL); // for checking validity; should delete this later
         execute(oTokens);
      }

      DynArray_map(oTokens, freeToken, NULL);
      DynArray_free(oTokens);
   }

   return TRUE;
}