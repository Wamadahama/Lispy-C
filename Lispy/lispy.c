#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"
#include "lispy-math.h"


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

/*Evalutate the operators*/
long eval_op(long x, char* op, long y){
  if(strcmp(op, "+") == 0) { return x + y;}
  if(strcmp(op, "-") == 0) { return x - y;}
  if(strcmp(op, "*") == 0) { return x * y;}
  if(strcmp(op, "/") == 0) { return x / y;}
  if(strcmp(op, "%") == 0) { return x % y;}
  if(strcmp(op, "^") == 0) { return power(x, y); }
  if(strcmp(op, "min") == 0) { return min(x, y);}
  if(strcmp(op, "max") == 0) { return max(x, y);}
  return 0;
}


/* Used to evalute input */
long eval(mpc_ast_t* t){
  /* IF tagged as number return it directly. */
  /* Str str is like searching for a substring*/

  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }

  /* The operator is always the second child*/
  char* op = t->children[1]->contents;

  /* Store the third child in 'x' */
  long x = eval(t->children[2]);

  int i = 3;
  while (strstr(t->children[i]->tag, "expr")){
    x = eval_op(x, op, eval(t->children[i]));
    ++i;
  }

  return x;
}


int main(void){

  /* Create the parsers for mathematical expressions */
  mpc_parser_t* Number    = mpc_new("number");
  mpc_parser_t* Operator  = mpc_new("operator");
  mpc_parser_t* Expr      = mpc_new("expr");
  mpc_parser_t* Lispy     = mpc_new("lispy");

  /* Define the grammar in the language */
  mpca_lang(MPCA_LANG_DEFAULT,
  "                                                                                                 \
    number   : /-?[0-9]+[.][0-9]+/ | /-?[0-9]+/ ;                                                   \
    operator : '+' | '-' | '*' | '/' | '%' | '^' | \"min\" | \"max\" ;                              \
    expr     : <number> | '(' <operator> <expr>+ ')' ;                                              \
    lispy    : /^/ <operator> <expr>+ /$/  ;                                                        \
  ",
  Number, Operator, Expr, Lispy);

  /* Print version information*/
  puts("Lispy Version 0.0.0.1.1");
  puts("Press Ctrl+C to Exit\n");

  /* Read Eval Print Loop*/
  while(1){

      /* Print out the prompt */
      char* input = readline("E-Lispy> ");
      add_history(input);

      /* Echo input back to the user*/
      mpc_result_t r;
      if (mpc_parse("<stdin>", input, Lispy, &r)) {

        /* Call the exalutaion function */
        long result = eval(r.output);
        printf("%li\n", result);
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
