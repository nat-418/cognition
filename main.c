#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern ht_t *WORD_TABLE;
extern array_t *STACK;
extern char *INBUF;
extern parser_t *PARSER;

void usage() {
  printf("Usage: stem [-hv] [file]\n");
  exit(1);
}

void version() {
  printf("Author: Preston Pan, MIT License 2023\n");
  printf("stem, version 1.0\n");
  exit(0);
}

int main(int argc, char **argv) {
  value_t *v;
  size_t len;

  if (argc < 2) {
    usage();
  }

  if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    usage();
  } else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
    version();
  }

  FILE *fp = fopen(argv[1], "rb");
  ssize_t bytes_read = getdelim(&INBUF, &len, '\0', fp);

  PARSER = init_parser(INBUF);
  STACK = init_array(10);
  WORD_TABLE = init_ht(500);

  while (1) {
    v = parser_get_next(PARSER);
    if (v == NULL)
      break;
    eval(v);
  }

  free(INBUF);
  ht_free(WORD_TABLE);
  array_free(STACK);
  free(PARSER);
  free(INBUF);
  return 0;
}
