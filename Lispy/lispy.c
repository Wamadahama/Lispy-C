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

#pragma region lval_definitions
/* lval structure used to contain what kind of evalutation something is */
typedef struct {
  int type;
  long num;
  /* Error and Symbol types have some string data */
  char* err;
  char* sym;
  /* Count and Pointer to list of "lval*" */
  int count;
  struct lval** cell;
} lval;

/* Possible lval types */
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };
/* Possible error types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM, LERR_MODULO_ERR };

/* Create a new number type lval */
lval* lval_num(long x) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_NUM;
  v->num = x;
  return v;
}

/* Create a new error type lval */
lval* lval_err(char* m) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_ERR;
  v->err = malloc(strlen(m) + 1);
  strcpy(v->err, m)
  return v;
}

/* Create a new sym type lval */
lval* lval_sym(char* s) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(s) + 1);
  strcpy(v->sym, s);
  return v;
}

/* Create a new se type lval */
lval* lval_sexpr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

/* Print out an lval */
void lval_print(lval v){
  switch (v.type) {
    /* If its a number then print the number and exit */
    case LVAL_NUM: printf("%li", v.num); break;

    /* If its an error then print out what error it is */
    case LVAL_ERR:
      if (v.err == LERR_DIV_ZERO) {
        printf("%s", "Error: Division By Zero!");
      }
      if (v.err == LERR_BAD_OP) {
        printf("%s", "Error: Invalid Operator!");
      }
      if (v.err == LERR_BAD_NUM) {
        printf("%s", "Error: Invalid Number!");
      }

      if (v.err == LERR_MODULO_ERR) {
        printf("%s", "Modulus division error!");
      }

      break;
   }
}

void lval_println(lval v){ lval_print(v); putchar('\n'); }

void lval_del(lval* v){
  switch (v->type) {
    /* Do nothing special for the number type */
    case LVAL_NUM: break;

    /* For Err or Sym free the string data */
    case LVAL_ERR: free(v->err); break;
    case LVAL_SYM: free(v->sym); break;

    /* If Sexpr then delete all the elements inside */
    case LVAL_SEXPR:
      for (int i = 0; i < v->count; i++)
        lval_del(v->cell[i]);

      /* Also free the memory allocated to contain the pointers */
      free(v->cell);
    break;
  }

  /* Free the memory allocated for the "lval" struct itself */
  free(v);
}

#pragma endregion lval_definitions


#pragma region evaluation
/*Evalutate the operators*/
lval eval_op(lval x, char* op, lval y){

  if (x.type == LVAL_ERR) { return x; }
  if (y.type == LVAL_ERR) { return y; }
  if(strcmp(op, "+") == 0) { return lval_num(x.num + y.num);}
  if(strcmp(op, "-") == 0) { return lval_num(x.num - y.num);}
  if(strcmp(op, "*") == 0) { return lval_num(x.num * y.num);}
  if(strcmp(op, "/") == 0) {
    return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
  }
  if(strcmp(op, "%") == 0) {
    return lval_num((x.num % y.num));
   }
  if(strcmp(op, "^") == 0) { return lval_num(power(x.num, y.num)); }
  if(strcmp(op, "min") == 0) { return lval_num(min(x.num, y.num)); }
  if(strcmp(op, "max") == 0) { return lval_num(max(x.num, y.num)); }

  // No evalulation occured then print out that it's a bad operator
  return lval_err(LERR_BAD_OP);
}



/* Used to evalute input */
lval eval(mpc_ast_t* t){

  /* IF tagged as number return it directly. */
  /* Str str is like searching for a substring*/

  if (strstr(t->tag, "number")) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  /* The operator is always the second child*/
  char* op = t->children[1]->contents;

  /* Store the third child in 'x' */
  lval x = eval(t->children[2]);

  int i = 3;
  while (strstr(t->children[i]->tag, "expr")){
    x = eval_op(x, op, eval(t->children[i]));
    ++i;
  }

  return x;
}

#pragma endregion evalutaion

int main(void){

  /* Create the parsers for mathematical expressions */
  mpc_parser_t* Number    = mpc_new("number");
  mpc_parser_t* Symbol    = mpc_new("symbol");
  mpc_parser_t* Sexpr     = mpc_new("sexpr");
  mpc_parser_t* Expr      = mpc_new("expr");
  mpc_parser_t* Lispy     = mpc_new("lispy");

  /* Define the grammar in the language */
  mpca_lang(MPCA_LANG_DEFAULT,
  "                                                                                                 \
    number   : /-?[0-9]+[.][0-9]+/ | /-?[0-9]+/ ;                                                   \
    symbol   : '+' | '-' | '*' | '/' | '%' | '^' | \"min\" | \"max\" ;                              \
    sexpr    : '(' <expr> ')' ;                                                                     \
    expr     : <number> | '(' <operator> <expr>+ ')' ;                                              \
    lispy    : /^/ <operator> <expr>+ /$/  ;                                                        \
  ",
  Number, Operator, Sexpr, Expr, Lispy);

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
        lval result = eval(r.output);
        lval_println(result);
        mpc_ast_delete(r.output);

      } else {
        /* Otherwise Print the error */
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
      }


      free(input);
  }

  mpc_cleanup(5, Number, Operator, Sexpr , Expr, Lispy);
  return 0;
}
