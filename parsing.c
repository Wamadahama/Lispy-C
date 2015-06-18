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

/* Error handling section */
enum {LVAL_NUM, LVAL_ERR};
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

typedef struct {
  int type;
  long num;
  int err;
}lval;



lval lval_num(long x){
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

lval lval_err(int x){
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

void lval_print(lval v) {
  switch (v.type) {
    case LVAL_NUM: printf("%li", v.num);
    case LVAL_ERR:
      if (v.err == LERR_DIV_ZERO) {
        printf("Divide by zero error");
      }
      if (v.err == LERR_BAD_OP) {
        printf("That is not a valid operator");
      }
      if (v.err == LERR_BAD_NUM) {
        printf("That number is to big for long to hold");
      }
      break;
  }
}

void lval_println(lval v) {
  lval_print(v);
  putchar('\n');
}

/* Error handling section */


/* Use operator string to see which operation to perform  */
lval eval_op(lval x, char* op, lval y){

  if (x.type == LVAL_ERR) {
    return x;
  }
  if (y.type == LVAL_ERR) {
    return y;
  }

  if(strcmp(op,"+") == 0){ return lval_num(x.num + y.num); }
  if(strcmp(op,"-") == 0){ return lval_num(x.num - y.num); }
  if(strcmp(op,"*") == 0){ return lval_num(x.num * y.num); }
  if(strcmp(op,"/") == 0){
    return y.num == 0
    ? lval_err(LERR_DIV_ZERO)
    : lval_num(x.num / y.num);
  }

  return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t){

  /* If tagged as number return it directly. */
  if (strstr(t->tag, "number")) {

    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  char* op = t->children[1]->contents;
  lval x = eval(t->children[2]);

  int i = 3;
  while(strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}

/* Counts the number of nodes in the tree structure */
int number_of_nodes(mpc_ast_t* t) {
  if (t->children_num == 0) { return 1; }
  if (t->children_num >= 1) {
    int total = 1;
    for (int i = 0; i < t->children_num; i++) {
      total = total + number_of_nodes(t->children[i]);
    }
    return total;
  }
  return 0;
}


void print_child_information(mpc_ast_t* t){
  int nc = number_of_nodes(t);
  for (int i = 0; i < nc; i++) {
    printf("Tag: %s\n", t->tag);
    printf("Contents: %s\n", t->contents);
    printf("Number of children: %s\n", t->children_num);

  }
}




int main(int argc, char const *argv[]) {

  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* lispy = mpc_new("lispy");

  /*

    If for whatever reason you decide to use add, sub, div, and mult
    instead of + , -, /, * you can use the following regex

    operator  : \"add\" | \"sub\" | \"mult\" | \"div\" ; \

  */
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number    : /-?[0-9]+/ ;                            \
      operator  : '+' | '-' | '*' | '/' ;                 \
      expr      : <number> | '(' <operator> <expr>+ ')' ; \
      lispy     : /^/ <operator> <expr> + /$/ ;           \
    ",
    Number, Operator, Expr, lispy
  );



  puts ("Lispy Version 0.0.0.3");
  puts ("Press Ctrl+c to Exit\n");

  while(1) {
    char* input = readline("lispy> ");
    add_history(input);

    mpc_result_t r;

    if (mpc_parse("<stdin>", input, lispy, &r)) {
      lval result = eval(r.output);
      lval_println(result);
      mpc_ast_delete(r.output);
    } else {
      /* Otherwise Print the Error */
      mpc_err_print(r.error);
      mpc_err_print(r.error);
    }

    free(input);
  }

  mpc_cleanup(4,Number, Operator, Expr, lispy);
  return 0;
  }
