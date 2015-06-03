#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

#ifdef _WIN32

#include <string.h>

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy,buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}


void add_history(char* unused){}


#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(int argc, char const *argv[]) {

  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* lispy = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number    : /-?[0-9]+/ ;                            \
      operator  : '+' | '-' | '*' | '/' ;                 \
      expr      : <number> | '(' <operator> <expr>+ ')' ; \
      lispy     : /^/ <operator> <expr> + /$/ ;           \
    ",
    Number, Operator, Expr, lispy
  );



  puts ("Lispy Version 0.0.0.2");
  puts ("Press Ctrl+c to Exit\n");

  while(1) {
    char* input = readline("lispy> ");
    add_history(input);

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, lispy, &r)) {
      /* On Success Print the AST */
      mpc_ast_t* a = r.output;
      mpc_ast_print(a);
      mpc_ast_delete(r.output);
      // printf("Tag: %s\n", a->tag);
      // printf("Contents: %s\n", a->contents);
      // printf("Number of children: %i\n", a->children_num);
      //
      // /* Get first child */
      // mpc_ast_t* c0 = a->children[1];
      // printf("First Child Tag %s\n", c0->tag);
      // printf("First Child contents %s\n", c0->contents);
      // printf("First Child Number of Children %s\n", c0->tag);

    } else {
      /* Otherwise Print the Error */
      mpc_err_print(r.error);
      mpc_err_print(r.error);
    }
  }

  mpc_cleanup(4,Number, Operator, Expr, lispy);
  return 0;
}
