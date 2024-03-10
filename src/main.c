#include <builtins.h>
#include <builtinslib.h>
#include <cognition.h>
#include <macros.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern stack_t *STACK;
extern parser_t *PARSER;
extern stack_t *EVAL_STACK;
extern stack_t *OBJ_STACK;

/*! prints usage then exits */
void usage() {
  printf("Usage: crank [-hv] [file]\n");
  exit(1);
}

/*! prints version and exits */
void version() {
  printf("Author: Preston Pan, Matthew Hinton, MIT License 2024\n");
  printf("cognition, version 1.0 alpha\n");
  exit(0);
}

/*! frees all global variables */
void global_free() {
  free(PARSER->source);
  stack_free(OBJ_STACK, obj_free);
  stack_free(STACK, contain_free);
  free(PARSER);
  stack_free(EVAL_STACK, value_free);
}

int main(int argc, char **argv) {
  value_t *v;
  size_t len = 0;
  char *buf = "";

  /* Parsing arguments */
  if (argc < 2) {
    usage();
  }

  if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    usage();
  } else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
    version();
  }

  /* Read code from file */
  FILE *FP = fopen(argv[1], "rb");

  if (!FP) {
    usage();
  }

  ssize_t bytes_read = getdelim(&buf, &len, '\0', FP);
  fclose(FP);

  /* Set up global variables */
  PARSER = init_parser(buf);
  STACK = init_stack(DEFAULT_STACK_SIZE);
  EVAL_STACK = init_stack(DEFAULT_STACK_SIZE);
  OBJ_STACK = init_stack(DEFAULT_STACK_SIZE);

  /* initialise environment */
  contain_t *stack = init_contain(init_ht(DEFAULT_HT_SIZE),
                                  init_ht(DEFAULT_HT_SIZE),
                                  init_stack(DEFAULT_STACK_SIZE));
  stack->faliases = init_stack(DEFAULT_STACK_SIZE);
  stack_push(stack->faliases, init_string("f"));
  add_funcs(stack->flit);
  stack_push(STACK, stack);
  void *(ot)[2] = {stack, init_ht(DEFAULT_HT_SIZE)};
  stack_push(OBJ_STACK, ot);

  /* parse and eval loop */
  while (1) {
    v = parser_get_next(PARSER);
    if (v == NULL)
      break;
    //cog_questionmark(v);
    //printf("\nword: [%s]\n", v->str_word->value);
    eval(v);
  }
  printf("\n");
  printf("Stack at end:\n");
  contain_t *cur = stack_peek(STACK);
  for (int i = 0; i < cur->stack->size; i++) {
    print_value(cur->stack->items[i], "\n");
  }
  printf("\n");
  printf("Error stack:\n");
  stack_t *error_stack = cur->err_stack;
  if (error_stack) {
    for (int i = 0; i < error_stack->size; i++) {
      print_value(error_stack->items[i], "\n");
    }
  }
  printf("\n");
  printf("delims: '");
  print_str_formatted(cur->delims);
  if (cur->dflag) printf("' (whitelist)\n");
  else printf("' (blacklist)\n");
  printf("ignored: '");
  print_str_formatted(cur->ignored);
  if (cur->iflag) printf("' (whitelist)\n");
  else printf("' (blacklist)\n");

  if (cur->cranks) {
    if (cur->cranks->size) {
      int(*cr)[2] = cur->cranks->items[0];
      printf("crank %d\n", cr[0][1]);
    } else printf("crank 0\n");
  } else printf("crank 0\n");

  /* Free all global variables */
  global_free();
  return 0;
}
