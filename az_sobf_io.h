/* =========================
   File: az_sobf_io.h
   ========================= */
#ifndef AZ_SOBF_IO_H
#define AZ_SOBF_IO_H

#include "az_values.h"

/* Program container read from a .sobf file:
   - codes: array of code_t (32-bit)
   - globals: array of value_t (64-bit) */
typedef struct {
  code_t *codes;
  int code_count;

  value_t *globals;
  int global_count;
} az_program;

/* @requires : prog != NULL
   @assigns  : prog->codes, prog->code_count, prog->globals, prog->global_count
   @ensures  : prog is initialized to an empty program (no allocated tables) */
void az_program_init(az_program *prog);

/* @requires : prog != NULL
   @assigns  : frees memory owned by prog->codes and prog->globals,
               then assigns prog fields to the empty program state
   @ensures  : prog does not own allocated tables anymore (reset to empty) */
void az_program_destroy(az_program *prog);

/* @requires : filename != NULL, prog != NULL
   @assigns  : may allocate prog->codes and prog->globals, sets counts,
               reads from the file, may write error messages to stderr
   @ensures  :
      - on success: returns 1 and prog contains valid tables read from filename
      - on failure: returns 0 and prog is left empty (no owned tables) */
int az_load_sobf(const char *filename, az_program *prog);

#endif
