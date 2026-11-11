/* =========================
   File: az_primitives.h
   ========================= */
#ifndef AZ_PRIMITIVES_H
#define AZ_PRIMITIVES_H

#include "az_values.h"

/* @requires : prim_id corresponds to a supported primitive for arity 1
   @assigns  : may read/write streams, may print errors
   @ensures  : returns a value_t according to primitive spec; if unsupported, returns AZ_UNIT */
value_t az_prim_call1(int prim_id, value_t a1);

/* @requires : prim_id corresponds to a supported primitive for arity 2
   @assigns  : may allocate memory, may write streams, may print errors
   @ensures  : returns a value_t according to primitive spec; if unsupported, returns AZ_UNIT */
value_t az_prim_call2(int prim_id, value_t a1, value_t a2);

/* @requires : none (can be unsupported)
   @assigns  : may print errors
   @ensures  : returns AZ_UNIT if unsupported */
value_t az_prim_call3(int prim_id, value_t a1, value_t a2, value_t a3);

value_t az_prim_call4(int prim_id, value_t a1, value_t a2, value_t a3, value_t a4);

value_t az_prim_call5(int prim_id, value_t a1, value_t a2, value_t a3, value_t a4, value_t a5);

/* @requires : args points to an array of length at least p, and p >= 0
   @assigns  : may allocate/print errors depending on prim_id
   @ensures  : returns a value_t according to primitive spec; if unsupported, returns AZ_UNIT */
value_t az_prim_calln(int prim_id, value_t *args, int p);

#endif
