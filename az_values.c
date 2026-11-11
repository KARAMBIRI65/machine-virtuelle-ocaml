/* =========================
   File: az_values.c
   ========================= */
#include "az_values.h"

/* @requires : none
   @assigns  : nothing
   @ensures  : returns the encoding of integer n (2n+1) */
value_t az_encode_int(value_t n) {
  return (value_t)(2 * n + 1);
}

/* @requires : v is an encoding of an integer
   @assigns  : nothing
   @ensures  : returns the decoded integer */
value_t az_decode_int(value_t v) {
  return (value_t)((v - 1) / 2);
}

/* @requires : none
   @assigns  : nothing
   @ensures  : returns 1 iff v is an encoding of an integer (odd) */
int az_is_int(value_t v) {
  return (int)(v & 1);
}

/* @requires : none
   @assigns  : nothing
   @ensures  : returns 1 iff v encodes false */
int az_is_false(value_t v) {
  return (int)(v == AZ_FALSE);
}
