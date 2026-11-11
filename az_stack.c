/* =========================
   File: az_stack.c
   ========================= */
#include <stdlib.h>
#include "az_stack.h"

/* @requires : s != NULL
   @assigns  : may reallocate s->data, updates s->cap (and s->data)
   @ensures  : returns 1 on success, 0 if realloc fails */
static int az_stack_grow(az_stack *s) {
  int new_cap;
  value_t *new_data;

  if (s->cap <= 0) {
    new_cap = 16;
  } else {
    new_cap = 2 * s->cap;
  }

  new_data = (value_t *)realloc(s->data, (size_t)new_cap * sizeof(value_t));
  if (new_data == NULL) {
    return 0;
  }
  s->data = new_data;
  s->cap = new_cap;
  return 1;
}

/* @requires : s != NULL
   @assigns  : s->data, s->size, s->cap
   @ensures  : returns 1 on success (initial buffer allocated),
               returns 0 on allocation failure */
int az_stack_init(az_stack *s) {
  if (s == NULL) return 0;

  s->data = NULL;
  s->size = 0;
  s->cap = 0;

  s->data = (value_t *)malloc((size_t)16 * sizeof(value_t));
  if (s->data == NULL) {
    return 0;
  }
  s->cap = 16;
  return 1;
}

/* @requires : s != NULL
   @assigns  : frees s->data, resets fields
   @ensures  : s is empty and owns no memory */
void az_stack_destroy(az_stack *s) {
  if (s == NULL) return;
  free(s->data);
  s->data = NULL;
  s->size = 0;
  s->cap = 0;
}

/* @requires : s != NULL
   @assigns  : nothing
   @ensures  : returns 1 iff size==0 */
int az_stack_is_empty(const az_stack *s) {
  if (s == NULL) return 1;
  return (int)(s->size == 0);
}

/* @requires : s != NULL
   @assigns  : s->data, s->size, maybe s->cap
   @ensures  : pushes v on the stack;
               returns 1 on success, 0 on allocation failure */
int az_stack_push(az_stack *s, value_t v) {
  if (s == NULL) return 0;

  if (s->size >= s->cap) {
    if (!az_stack_grow(s)) return 0;
  }
  s->data[s->size] = v;
  s->size = s->size + 1;
  return 1;
}

/* @requires : s != NULL and s->size > 0
   @assigns  : s->size
   @ensures  : removes top element and returns it */
value_t az_stack_pop(az_stack *s) {
  value_t v;
  s->size = s->size - 1;
  v = s->data[s->size];
  return v;
}

/* @requires : s != NULL and 0 <= depth < s->size
   @assigns  : nothing
   @ensures  : returns element at depth (0 is top) */
value_t az_stack_peek_depth(const az_stack *s, int depth) {
  int idx;
  idx = s->size - 1 - depth;
  return s->data[idx];
}

/* @requires : s != NULL and 0 <= depth < s->size
   @assigns  : s->data[idx]
   @ensures  : overwrites element at depth with v */
void az_stack_set_depth(az_stack *s, int depth, value_t v) {
  int idx;
  idx = s->size - 1 - depth;
  s->data[idx] = v;
}

/* @requires : s != NULL and 0 <= n <= s->size
   @assigns  : s->size
   @ensures  : removes exactly n elements from the top */
void az_stack_pop_n(az_stack *s, int n) {
  /* termination reason: n decreases strictly to 0 */
  while (n > 0) {
    s->size = s->size - 1;
    n = n - 1;
  }
}
