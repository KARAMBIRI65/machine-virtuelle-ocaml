/* =========================
   File: az_vm_main.c
   ========================= */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "az_sobf_io.h"
#include "az_vm_core.h"

/* @requires : prog != NULL
   @assigns  : stderr output
   @ensures  : prints correct usage line */
static void az_usage(const char *prog) {
  fprintf(stderr, "Usage: %s file.sobf [--print-end-machine]\n", prog);
}

/* @requires : argv != NULL, argc >= 1
   @assigns  : program state, stdout/stderr outputs, dynamic allocations via loader/vm
   @ensures  :
     - expects argv[1] as SOBF file path
     - if argv[2] == --print-end-machine prints final machine state
     - returns 0 on success, non-zero on error */
int main(int argc, char **argv) {
  az_program prog;
  az_vm vm;
  int print_end;
  value_t res;
  int ok;

  if (argc < 2) {
    az_usage(argv[0]);
    return 1;
  }

  print_end = 0;
  if (argc >= 3) {
    if (strcmp(argv[2], "--print-end-machine") == 0) {
      print_end = 1;
    }
  }

  az_program_init(&prog);

  fprintf(stderr, "[MAIN] Loading: %s\n", argv[1]);

  ok = az_load_sobf(argv[1], &prog);
  if (!ok) {
    fprintf(stderr, "[MAIN] ERROR: cannot load SOBF file: %s\n", argv[1]);
    az_program_destroy(&prog);
    return 1;
  }

  fprintf(stderr, "[MAIN] Loaded OK: code_count=%d global_count=%d\n",
          prog.code_count, prog.global_count);

  if (!az_vm_init(&vm, &prog)) {
    fprintf(stderr, "VM init failed\n");
    az_program_destroy(&prog);
    return 1;
  }

  fprintf(stderr, "[MAIN] Running VM...\n");
  res = az_vm_run(&vm);
  fprintf(stderr, "[MAIN] VM finished. res=%lld\n", (long long)res);

  if (print_end) {
    az_vm_print_end(&vm);
  }

  printf("%lld\n", (long long)res);

  az_vm_destroy(&vm);
  az_program_destroy(&prog);
  return 0;
}
