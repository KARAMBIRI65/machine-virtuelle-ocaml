#include <stdio.h>
#include <stdlib.h>
#include "az_primitives.h"
#include "az_values.h"

/* @requires : v contains a FILE* encoded as a value_t (casted pointer)
   @assigns  : nothing
   @ensures  : returns the FILE* corresponding to v */
static FILE *az_value_to_file(value_t v) {
  return (FILE *)(long int)v;
}

/* @requires : f != NULL
   @assigns  : nothing
   @ensures  : returns a value_t storing f */
static value_t az_file_to_value(FILE *f) {
  return (value_t)(long int)f;
}

/* @requires : v is an OCaml int encoding (odd) OR a small raw int (0/1/2)
   @assigns  : nothing
   @ensures  : returns the "best guess" integer value */
static long int az_get_int_loose(value_t v) {
  /* If it looks like an encoded int (odd), decode it */
  if (az_is_int(v)) return az_decode_int(v);
  /* Otherwise treat as raw integer (some toolchains may pass raw small ints) */
  return (long int)v;
}

/* @requires : prim_id is a primitive id
   @assigns  : may write to streams (stderr/stdout), may read/flush
   @ensures  :
     - 288: flush channel a1, returns unit
     - 293: read one char from channel a1, returns encoded int (0 if EOF)
     - 302: returns stdin when arg denotes fd 0, else returns unit (no error spam)
     - 304: returns stdout for fd 1, stderr for fd 2, else returns unit (no error spam)
     - otherwise: returns unit and may print an error */
value_t az_prim_call1(int prim_id, value_t a1) {
  FILE *f;
  int ch;

  if (prim_id == 288) {
    f = az_value_to_file(a1);
    fflush(f);
    return AZ_UNIT;
  }

  if (prim_id == 293) {
    f = az_value_to_file(a1);
    ch = fgetc(f);
    if (ch == EOF) ch = 0;
    return az_encode_int((long int)ch);
  }

  if (prim_id == 302) {
    /* expected: fd 0 -> stdin */
    long int fd = az_get_int_loose(a1);
    if (fd == 0) return az_file_to_value(stdin);
    /* don't spam stderr during tests */
    return AZ_UNIT;
  }

  if (prim_id == 304) {
    /* expected: fd 1 -> stdout, fd 2 -> stderr */
    long int fd = az_get_int_loose(a1);
    if (fd == 1) return az_file_to_value(stdout);
    if (fd == 2) return az_file_to_value(stderr);
    return AZ_UNIT;
  }

  fprintf(stderr, "Unsupported primitive (call1): %d\n", prim_id);
  return AZ_UNIT;
}

/* @requires : prim_id is one of {15, 310} with expected arguments
   @assigns  : may allocate (15), may write to stream (310)
   @ensures  :
     - 15: alloc block of size decode_int(a1) filled with a2, returns pointer as value_t
     - 310: prints character decode_int(a2) to channel a1, returns unit
     - otherwise: returns unit and may print error */
value_t az_prim_call2(int prim_id, value_t a1, value_t a2) {
  value_t *block;
  long int n;
  FILE *f;
  long int code;

  if (prim_id == 15) {
    n = az_decode_int(a1);
    if (n < 0) n = 0;

    block = (value_t *)malloc((size_t)n * sizeof(value_t));
    if (block == NULL) {
      fprintf(stderr, "Allocation failed in primitive 15\n");
      return AZ_UNIT;
    }

    /* termination reason: i increases until i == n */
    {
      long int i = 0;
      while (i < n) {
        block[(int)i] = a2;
        i = i + 1;
      }
    }

    return (value_t)(long int)block;
  }

  if (prim_id == 310) {
    f = az_value_to_file(a1);
    code = az_decode_int(a2);
    fputc((int)code, f);
    return AZ_UNIT;
  }

  fprintf(stderr, "Unsupported primitive (call2): %d\n", prim_id);
  return AZ_UNIT;
}

/* @requires : any
   @assigns  : may write stderr
   @ensures  : returns unit (not implemented here) */
value_t az_prim_call3(int prim_id, value_t a1, value_t a2, value_t a3) {
  (void)a1; (void)a2; (void)a3;
  fprintf(stderr, "Unsupported primitive (call3): %d\n", prim_id);
  return AZ_UNIT;
}

value_t az_prim_call4(int prim_id, value_t a1, value_t a2, value_t a3, value_t a4) {
  (void)a1; (void)a2; (void)a3; (void)a4;
  fprintf(stderr, "Unsupported primitive (call4): %d\n", prim_id);
  return AZ_UNIT;
}

value_t az_prim_call5(int prim_id, value_t a1, value_t a2, value_t a3, value_t a4, value_t a5) {
  (void)a1; (void)a2; (void)a3; (void)a4; (void)a5;
  fprintf(stderr, "Unsupported primitive (call5): %d\n", prim_id);
  return AZ_UNIT;
}

value_t az_prim_calln(int prim_id, value_t *args, int p) {
  (void)args; (void)p;
  fprintf(stderr, "Unsupported primitive (calln): %d\n", prim_id);
  return AZ_UNIT;
}

