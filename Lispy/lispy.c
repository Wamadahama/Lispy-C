#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline(char* prompt){
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer) + 1);
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

int main(void){

  /* Create the parsers for mathematical expressions */
  mpc_parser_t* Number    = mpc_new("number");
  mpc_parser_t* Operator  = mpc_new("operator");
  mpc_parser_t* Expr      = mpc_new("expr");
  mpc_parser_t* Lispy     = mpc_new("lispy");

  /* Define the grammas in the language */
  mpca_lang(MPCA_LANG_DEFAULT,
  "                                                                                               \
    number   : /-?[0-9]+[.][0-9]+/ | /-?[0-9]+/ ;                                                 \
    operator : '+' | '-' | '*' | '/' | '%' ;                                                      \
    expr     : <number> | '(' <operator> <expr>+ ')' ;                                            \
    lispy    : /^/ <operator> <expr>+ /$/  ;                                                      \
  ",
  Number, Operator, Expr, Lispy);

  /* Print version information*/
  puts("Lispy Version 0.0.0.0.1");
  puts("Press Ctrl+C to Exit\n");

  // Infinite loop (REPL)
  while(1){

      /* Print out the prompt */
      char* input = readline("E-Lispy> ");
      add_history(input);

      /* Echo input back to the user*/
      mpc_result_t r;
      if (mpc_parse("<stdin>", input, Lispy, &r)) {
        /* On Success Print the AST */
        mpc_ast_print(r.output);
        mpc_ast_delete(r.output);
      } else {
        /* Otherwise Print the error */
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
      }


      free(input);
  }

  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  return 0;
}
