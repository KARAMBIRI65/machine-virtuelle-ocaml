/* =========================
   File: az_values.h
   ========================= */
#ifndef AZ_VALUES_H
#define AZ_VALUES_H

#include <stdint.h>

/* Types imposés par le sujet (tailles EXACTES, portables) */
typedef int32_t code_t;     /* 32 bits */
typedef int64_t value_t;    /* 64 bits */

/* Encodages OCaml simplifiés */
#define AZ_FALSE   ((value_t)1)
#define AZ_TRUE    ((value_t)3)
#define AZ_UNIT    ((value_t)1)


/* @requires : none
   @assigns  : nothing
   @ensures  : returns the encoding of integer n (2n+1) */
value_t az_encode_int(value_t n);

/* @requires : v is an encoding of an integer
   @assigns  : nothing
   @ensures  : returns the decoded integer */
value_t az_decode_int(value_t v);

/* @requires : none
   @assigns  : nothing
   @ensures  : returns 1 iff v is an encoding of an integer (odd) */
int az_is_int(value_t v);

/* @requires : none
   @assigns  : nothing
   @ensures  : returns 1 iff v encodes false */
int az_is_false(value_t v);

#endif
