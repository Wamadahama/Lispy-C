#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline(char* prompt){
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = mallock(strlen(buffer) + 1)
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

/* Fake add history function */
void add_history(char* unused){}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif
/* Declare a buffer for the user input of size 2048 */
static char input[2048];

int main(void){

  /* Print version information*/
  puts("Lispy Version 0.0.0.0.1");
  puts("Press Ctrl+C to Exit\n");

  // Infinite loop (REPL)
  while(1){

      /* Print out the prompt */
      char* input = readline("E-Lispy> ");
      add_history(input);

      /* Eacho input back to the user*/
      printf("No you're a %s", input);
      free(input);
  }

  return 0;
}
