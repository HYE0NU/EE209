/*--------------------------------------------------------------------*/
/* main.c                                                             */
/* Author: 20210604 Jung Hyeonu                                       */
/* Main function of ish                                               */
/*--------------------------------------------------------------------*/

#include <stdio.h>  // for FILE* type and stdin macro
#include <linux/limits.h> // for PATH_MAX macro
#include <stdlib.h> // for getenv()
#include <string.h> // for strcpy and strcat
#include <unistd.h>
#include "input.h"
#include "userdef.h"

//enum {FALSE, TRUE};
char* g_programName;

int main(int argc, char* argv[])

{
   const char* homedir = getenv("HOME");
   char rcDir[PATH_MAX];
   FILE* fpRc;
   g_programName = argv[0];

   if(homedir == NULL) return FALSE;

   strcpy(rcDir, homedir);
   strcat(rcDir, "/.ishrc");

   if((fpRc = fopen(rcDir, "r")) != NULL)
   {
      inputMain(fpRc);
      fclose(fpRc);
   }

   inputMain(stdin);

   printf("\n");
   return 0;
}
