/*20210604 정현우 (Jung Hyeonu) 
assignment1, decomment.c */

#include <stdio.h> /*for printf*/
#include <stdlib.h> /*for EXIT_SUCCESS macro*/
#include <ctype.h> /*for State type*/

/*DFA states*/
enum DFAState {IN_CODE, ENTER_COMMENT, IN_COMMENT, EXIT_COMMENT,
 IN_CHAR, IN_STRING};

/*boolean value for backslash usage (in char,string state)*/
enum Backslash {True, False};

/*Modules for each DFA states*/
void at_IN_CODE(enum DFAState *state, char charbuff);
void at_ENTER_COMMENT(enum DFAState *state, char charbuff,
 unsigned int *line, unsigned int *errorline);
void at_IN_COMMENT(enum DFAState *state, char charbuff);
void at_EXIT_COMMENT(enum DFAState *state, char charbuff);
void at_IN_CHAR(enum DFAState *state, char charbuff,
 enum Backslash *backslash);
void at_IN_STRING(enum DFAState *state, char charbuff,
 enum Backslash *backslash);

/*Modules for other function*/

/*counts line number with stdin char input*/
void countline(char charbuff, unsigned int *line){
    if(charbuff == '\n') *line = *line+1;
}

/*get state info; comment termination check and print error*/
int termination(enum DFAState state, unsigned int errline){
    if(state == IN_COMMENT || state == EXIT_COMMENT){ 
        fprintf(stderr, 
        "Error: line %d: unterminated comment\n", errline);
        return EXIT_FAILURE;
    }
    else return EXIT_SUCCESS;
}

/*Removes comment written as / * * / form. 
Reads text(code) from standard input stream. 
Print decommented text to standard output stream.
Print error message to stderr stream if comment is unterminated.*/
/*no global variable used.*/
int main(){ 
    
    enum DFAState state = IN_CODE; /*reset state to IN_CODE*/
    enum Backslash backslash = False; /*reset backslash usage*/
    char charbuff; /*input buffer*/
    unsigned int line=1, errline; /*line# and last comment line#*/

    /*recieve character & do action follwing DFA state*/
    while((charbuff=getchar()) != EOF){ 

        countline(charbuff, &line);
        switch(state){
            case IN_CODE:{
                at_IN_CODE(&state, charbuff);
                break;
            }
            case ENTER_COMMENT:{
                at_ENTER_COMMENT(&state, charbuff, &line, &errline);
                break;
            }
            case IN_COMMENT:{
                at_IN_COMMENT(&state, charbuff);
                break;
            }
            case EXIT_COMMENT:{
                at_EXIT_COMMENT(&state, charbuff);
                break;
            }
            case IN_CHAR:{
                at_IN_CHAR(&state, charbuff, &backslash);
                break;
            }
            case IN_STRING:{
                at_IN_STRING(&state, charbuff, &backslash);
                break;
            }
        }
    }
    return termination(state, errline);
}

/*non-comment state; get char input
ready to start comment or char/str; print char*/
void at_IN_CODE(enum DFAState *state, char charbuff){
    if(charbuff == '/'){ 
    *state = ENTER_COMMENT;
    return;
    }
    else if(charbuff == '\'') *state = IN_CHAR;  
    else if(charbuff == '\"') *state = IN_STRING;

    printf("%c",charbuff);
}

/*get char input; change to comment state 
or return to non-comment state; print char*/
void at_ENTER_COMMENT(enum DFAState *state, char charbuff,
 unsigned int *line, unsigned int *errline){
    if(charbuff == '*'){
        *state = IN_COMMENT;
        *errline = *line;
        printf(" ");
    }
    else if(charbuff == '/'){
        *state = ENTER_COMMENT;
        printf("/");
    }
    else{
        *state = IN_CODE;
        printf("/%c", charbuff);
    }
}

/*comment state; get char input;
ready to end comment; change line*/
void at_IN_COMMENT(enum DFAState *state, char charbuff){
    if(charbuff == '*') *state = EXIT_COMMENT;
    else if(charbuff == '\n') printf("\n");
}

/*get char input; change to non-comment state or return; change line*/
void at_EXIT_COMMENT(enum DFAState *state, char charbuff){
    if(charbuff == '/') *state = IN_CODE;
    else if(charbuff == '*') *state = EXIT_COMMENT;
    else if(charbuff == '\n'){
        printf("\n");
        *state = IN_COMMENT;
    }
    else *state = IN_COMMENT;
}

/*handle constant region(char) state; get char input;
use backslash info to ignore '; print*/
void at_IN_CHAR(enum DFAState *state, char charbuff,
 enum Backslash *backslash){
    if(*backslash == True){
        *backslash = False;
    }
    else if(*backslash == False){
        if(charbuff == '\'') *state = IN_CODE;
        else if(charbuff == '\\') *backslash = True;
    }
    printf("%c", charbuff);
}
/*handle constant region(string) state; get char input;
use backslash infoto ignore "; print*/
void at_IN_STRING(enum DFAState *state, char charbuff,
 enum Backslash *backslash){
    if(*backslash == True){
        *backslash = False;
    }
    else if(*backslash == False){
        if(charbuff == '\"') *state = IN_CODE;
        else if(charbuff == '\\') *backslash = True;
    }
    printf("%c", charbuff);
}