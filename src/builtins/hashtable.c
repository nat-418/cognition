#include <builtins/hashtable.h>
#include <builtinslib.h>
#include <macros.h>

extern stack_t *STACK;

void cog_def(value_t *v) {
  contain_t *cur = stack_peek(STACK);
  stack_t *stack = cur->stack;
  if (stack->size < 2) {
    eval_error("TOO FEW ARGUMENTS", v);
    return;
  }
  value_t *quot = stack_pop(stack);
  value_t *wordc = stack_pop(stack);
  if (wordc->container->stack->size != 1) {
    eval_error("TYPE ERROR", v);
    stack_push(stack, wordc);
    stack_push(stack, quot);
    return;
  }
  value_t *word = wordc->container->stack->items[0];
  if (word->type != VWORD) {
    eval_error("TYPE ERROR", v);
    stack_push(stack, wordc);
    stack_push(stack, quot);
    return;
  }
  ht_add(cur->word_table, word->str_word, quot->container, contain_free);
  word->str_word = NULL;
  quot->container = NULL;
  value_free(wordc);
  value_free(quot);
}

void cog_undef(value_t *v) {
  contain_t *cur = stack_peek(STACK);
  stack_t *stack = cur->stack;
  value_t *wordc = stack_pop(stack);
  if (!wordc) {
    eval_error("TOO FEW ARGUMENTS", v);
    return;
  }
  if (wordc->container->stack->size != 1) {
    eval_error("TYPE ERROR", v);
    stack_push(stack, wordc);
    return;
  }
  value_t *word = wordc->container->stack->items[0];
  if (word->type != VWORD) {
    eval_error("TYPE ERROR", v);
    stack_push(stack, wordc);
    return;
  }
  ht_delete(cur->word_table, word->str_word, contain_free);
  ht_delete(cur->flit, word->str_word, value_stack_free);
  free(word);
}

void cog_unglue(value_t *v) {
  contain_t *cur = stack_peek(STACK);
  stack_t *stack = cur->stack;
  value_t *wordc = stack_peek(stack);
  if (!wordc) {
    eval_error("TOO FEW ARGUMENTS", v);
    return;
  }
  if (wordc->container->stack->size != 1) {
    eval_error("BAD ARGUMENT TYPE", v);
    return;
  }
  value_t *wordval = wordc->container->stack->items[0];
  if (wordval->type != VWORD) {
    eval_error("BAD ARGUMENT TYPE", v);
    return;
  }
  contain_t *def = ht_get(cur->word_table, wordval->str_word);;
  if (def == NULL) {
    eval_error("UNDEFINED WORD", v);
    return;
  }
  value_free(wordval);
  wordc->container->stack->size = 0;
  for (int i = 0; i < def->stack->size; i++) {
    stack_push(wordc->container->stack, value_copy(def->stack->items[i]));
  }
}

void cog_isdef(value_t *v) {
  contain_t *cur = stack_peek(STACK);
  stack_t *stack = cur->stack;
  value_t *wordc = stack_peek(stack);
  if (!wordc) {
    eval_error("TOO FEW ARGUMENTS", v);
    return;
  }
  if (wordc->container->stack->size != 1) {
    eval_error("BAD ARGUMENT TYPE", v);
    return;
  }
  value_t *wordval = wordc->container->stack->items[0];
  if (wordval->type != VWORD) {
    eval_error("BAD ARGUMENT TYPE", v);
    return;
  }
  bool exists = ht_exists(cur->word_table, wordval->str_word) || ht_exists(cur->flit, wordval->str_word);
  wordval->str_word->length = 0;
  wordval->str_word->value[0] = '\0';
  if (exists) {
    string_append(wordval->str_word, 't');
  }
}

void cog_alias(value_t *v) {
  contain_t *cur = stack_peek(STACK);
  stack_t *stack = cur->stack;
  value_t *quot = stack_pop(stack);
  if (!quot) {
    eval_error("TOO FEW ARGUMENTS", v);
    return;
  }
  value_t *wordc = stack_pop(stack);
  if (!wordc) {
    eval_error("TOO FEW ARGUMENTS", v);
    stack_push(stack, quot);
    return;
  }
  if (wordc->container->stack->size != 1) {
    eval_error("BAD ARGUMENT TYPE", v);
    stack_push(stack, wordc);
    stack_push(stack, quot);
    return;
  }
  value_t *wordval = wordc->container->stack->items[0];
  if (wordval->type != VWORD) {
    eval_error("BAD ARGUMENT TYPE", v);
    stack_push(stack, wordc);
    stack_push(stack, quot);
    return;
  }
  stack_t *macro = init_stack(DEFAULT_STACK_SIZE);
  stack_t *family = init_stack(DEFAULT_STACK_SIZE);
  stack_push(family, cur);
  expandstack(quot->container, macro, family);
  free(family->items);
  free(family);
  ht_add(cur->flit, string_copy(wordval->str_word), macro, contain_free);
  value_free(wordc);
  value_free(quot);
}

void cog_bind(value_t *v) {
  contain_t *cur = stack_peek(STACK);
  stack_t *stack = cur->stack;
  value_t *quot = stack_pop(stack);
  if (!quot) {
    eval_error("TOO FEW ARGUMENTS", v);
    return;
  }
  value_t *wordc = stack_pop(stack);
  if (!wordc) {
    eval_error("TOO FEW ARGUMENTS", v);
    stack_push(stack, quot);
    return;
  }
  if (wordc->container->stack->size != 1) {
    eval_error("BAD ARGUMENT TYPE", v);
    stack_push(stack, wordc);
    stack_push(stack, quot);
    return;
  }
  value_t *wordval = wordc->container->stack->items[0];
  if (wordval->type != VWORD) {
    eval_error("BAD ARGUMENT TYPE", v);
    stack_push(stack, wordc);
    stack_push(stack, quot);
    return;
  }
  stack_t *macro = init_stack(DEFAULT_STACK_SIZE);
  stack_t *family = init_stack(DEFAULT_STACK_SIZE);
  stack_push(family, cur);
  expandstack(quot->container, macro, family);
  free(family->items);
  free(family);
  ht_add(cur->word_table, string_copy(wordval->str_word), macro, contain_free);
  value_free(wordc);
  value_free(quot);
}

void add_funcs_hashtable(ht_t *flit) {
  add_func(flit, cog_def, "def");
  add_func(flit, cog_undef, "undef");
  add_func(flit, cog_unglue, "unglue");
  add_func(flit, cog_isdef, "isdef");
  add_func(flit, cog_alias, "alias");
}
