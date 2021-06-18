/*  20210604 정현우
    assignmnet #2
    sgrep.c   */

#include <stdio.h> /* for fgets, fprintf, printf */
#include <stdlib.h> /* for EXIT_SUCCESS and EXIT_FAILURE */
#include <unistd.h> /* for getopt */
#include "str.h" /* for StrGetLength and StrFindStr */

#define MAX_STR_LEN 1023

#define FALSE 0
#define TRUE  1

void CutUnit(char* unit, const char* pattern);
int PatternInLine(char* line, const char* pattern);

/*------------------------------------------------------------------*/

/* recieve pattern and 
copy string before first '*' to destination(unit) */
void
CutUnit(char* unit, const char* pattern)
{
    char *temp = unit;

    while(*pattern != '*' && *pattern != '\0')
    {   
        *temp = *pattern;
        temp++;
        pattern++;
    }
    *temp = '\0';
}

/* get single line string and pattern string 
determines if the pattern is within line; return TRUE/FALSE */
int
PatternInLine(char* line, const char* pattern)
{
    char unit[StrGetLength(pattern)+1];
    char *temp;
    int unitlen;

    /* eliminate front '*'s in pattern */
    while(*pattern == '*') pattern++;

    /* get substring of pattern (before first *) */
    CutUnit(unit, pattern);
    unitlen = StrGetLength(unit);

    /* recursive search */
    if(*pattern == '\0') return TRUE;
    else{
        temp = StrFindStr(line, unit);
        if(temp == NULL) return FALSE;
        else{
            return PatternInLine(temp+unitlen, pattern+unitlen);
        }
    }
}

/*------------------------------------------------------------------*/
/* PrintUsage()
   print out the usage of the Simple Grep Program */
/*------------------------------------------------------------------*/
void 
PrintUsage(const char* argv0) 
{
    const static char *fmt = 
	    "Simple Grep (sgrep) Usage:\n"
	    "%s pattern [stdin]\n";

    printf(fmt, argv0);
}
/*------------------------------------------------------------------*/
/* gets pattern string and read string from stdin
print line if the pattern is within line, or if pattern is empty
pattern can include * character; any string can replace it
print error 
pattern limit: 1023 byte, line limit: 1023 byte
*/
int
SearchPattern(const char *pattern)
{
    char buf[MAX_STR_LEN + 2];
    size_t len;
  
    /* check if pattern is too long */
   if(StrGetLength(pattern) > MAX_STR_LEN){
       fprintf(stderr, "Error: argument is too long\n");
       return FALSE;
   }

    /* Read one line at a time from stdin, and process each line */
    while (fgets(buf, sizeof(buf), stdin)) {

        /* check the length of an input line */
        if ((len = StrGetLength(buf)) > MAX_STR_LEN) {
            fprintf(stderr, "Error: input line is too long\n");
            return FALSE;
        }

        /*print line if pattern exists in the line */
        if(PatternInLine(buf, pattern)){
            printf("%s", buf);
        }
    }
   
  return TRUE;
}
/*------------------------------------------------------------------*/
int 
main(const int argc, const char *argv[]) 
{
  /* Do argument check and parsing */
    if (argc < 2) {
	    fprintf(stderr, "Error: argument parsing error\n");
	    PrintUsage(argv[0]);
	    return (EXIT_FAILURE);
    }

    return SearchPattern(argv[1]) ? EXIT_SUCCESS:EXIT_FAILURE;
}