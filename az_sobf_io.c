/* =========================
   File: az_sobf_io.c
   ========================= */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "az_sobf_io.h"

/* @requires : prog != NULL
   @assigns  : prog fields
   @ensures  : initializes prog to empty */
void az_program_init(az_program *prog) {
  prog->codes = NULL;
  prog->code_count = 0;
  prog->globals = NULL;
  prog->global_count = 0;
}

/* @requires : prog != NULL
   @assigns  : frees prog->codes and prog->globals, resets fields
   @ensures  : prog becomes empty */
void az_program_destroy(az_program *prog) {
  if (prog == NULL) return;
  free(prog->codes);
  free(prog->globals);
  az_program_init(prog);
}

/* @requires : f != NULL, buf != NULL, buf_sz > 0
   @assigns  : buf[...], reads from f
   @ensures  : returns 1 if a line is read, 0 on EOF/error */
static int az_read_header_line(FILE *f, char *buf, int buf_sz) {
  char *r;
  r = fgets(buf, buf_sz, f);
  if (r == NULL) return 0;
  return 1;
}

/* @requires : filename != NULL and prog != NULL
   @assigns  : prog->codes, prog->globals, counts; reads file; may print errors
   @ensures  : returns 1 on success; returns 0 on any error and leaves prog empty */
int az_load_sobf(const char *filename, az_program *prog) {
  FILE *f;
  char buf[128];
  int c, v;
  size_t got;

  if (filename == NULL || prog == NULL) return 0;

  f = fopen(filename, "rb");
  if (f == NULL) {
    fprintf(stderr, "Cannot open file: %s\n", filename);
    return 0;
  }

  if (!az_read_header_line(f, buf, 128)) {
    fclose(f);
    return 0;
  }

  /* accept "SOBF\n" */
  if (strncmp(buf, "SOBF", 4) != 0) {
    fprintf(stderr, "Bad SOBF header\n");
    fclose(f);
    return 0;
  }

  if (!az_read_header_line(f, buf, 128)) {
    fclose(f);
    return 0;
  }

  if (sscanf(buf, "%d %d", &c, &v) != 2) {
    fprintf(stderr, "Bad counts line\n");
    fclose(f);
    return 0;
  }
  if (c < 0 || v < 0) {
    fprintf(stderr, "Negative counts\n");
    fclose(f);
    return 0;
  }

  prog->codes = (code_t *)malloc((size_t)c * sizeof(code_t));
  prog->globals = (value_t *)malloc((size_t)v * sizeof(value_t));
  if ((c > 0 && prog->codes == NULL) || (v > 0 && prog->globals == NULL)) {
    fprintf(stderr, "Allocation failed\n");
    az_program_destroy(prog);
    fclose(f);
    return 0;
  }
  prog->code_count = c;
  prog->global_count = v;

  got = fread(prog->codes, sizeof(code_t), (size_t)c, f);
  if ((int)got != c) {
    fprintf(stderr, "Cannot read code table (binary)\n");
    az_program_destroy(prog);
    fclose(f);
    return 0;
  }

  got = fread(prog->globals, sizeof(value_t), (size_t)v, f);
  if ((int)got != v) {
    fprintf(stderr, "Cannot read globals table (binary)\n");
    az_program_destroy(prog);
    fclose(f);
    return 0;
  }

  fclose(f);
  return 1;
}
