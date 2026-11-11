/* =========================
   File: az_stack.h
   ========================= */
#ifndef AZ_STACK_H
#define AZ_STACK_H

#include "az_values.h"

typedef struct {
  value_t *data;
  int size;
  int cap;
} az_stack;

/* @requires : s != NULL
   @assigns  : s->data, s->size, s->cap
   @ensures  : s is an empty stack with some capacity,
               returns 1 on success, 0 on allocation failure */
int az_stack_init(az_stack *s);

/* @requires : s != NULL
   @assigns  : frees s->data, sets s->data to NULL, resets size/cap
   @ensures  : stack does not own memory anymore and is empty */
void az_stack_destroy(az_stack *s);

/* @requires : s != NULL
   @assigns  : nothing
   @ensures  : returns 1 iff stack is empty */
int az_stack_is_empty(const az_stack *s);

/* @requires : s != NULL
   @assigns  : may reallocate s->data, updates s->size and maybe s->cap
   @ensures  : pushes v, returns 1 on success, 0 on allocation failure */
int az_stack_push(az_stack *s, value_t v);

/* @requires : s != NULL and s->size > 0
   @assigns  : s->size
   @ensures  : pops and returns the top value */
value_t az_stack_pop(az_stack *s);

/* @requires : s != NULL and 0 <= depth < s->size
   @assigns  : nothing
   @ensures  : returns element at depth (0 = top) without popping */
value_t az_stack_peek_depth(const az_stack *s, int depth);

/* @requires : s != NULL and 0 <= depth < s->size
   @assigns  : s->data[...]
   @ensures  : replaces element at depth by v */
void az_stack_set_depth(az_stack *s, int depth, value_t v);

/* @requires : s != NULL and 0 <= n <= s->size
   @assigns  : s->size
   @ensures  : removes n elements (discard) */
void az_stack_pop_n(az_stack *s, int n);

#endif
