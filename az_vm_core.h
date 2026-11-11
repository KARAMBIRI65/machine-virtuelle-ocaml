/* =========================
   File: az_vm_core.h
   ========================= */
#ifndef AZ_VM_CORE_H
#define AZ_VM_CORE_H

#include "az_sobf_io.h"
#include "az_stack.h"
#include "az_values.h"

typedef struct {
  code_t *code;
  int code_len;

  int pc;
  value_t acc;
  az_stack stack;

  value_t *globals;
  int globals_len;

  value_t atoms[256];
} az_vm;

/* @requires : vm != NULL and prog != NULL
   @assigns  : vm fields, vm->stack, vm->atoms
   @ensures  : vm is initialized with prog and ready to run; returns 1 on success, 0 on failure */
int az_vm_init(az_vm *vm, const az_program *prog);

/* @requires : vm != NULL
   @assigns  : vm->stack (freed), resets stack fields
   @ensures  : vm stack is destroyed; atoms/blocks not freed (per simplified spec) */
void az_vm_destroy(az_vm *vm);

/* @requires : vm != NULL
   @assigns  : vm state (pc, acc, stack, globals, heap blocks), may print errors
   @ensures  : returns accumulator at STOP, or exits on fatal errors */
value_t az_vm_run(az_vm *vm);

/* @requires : vm != NULL
   @assigns  : outputs to stdout
   @ensures  : prints final state in required format */
void az_vm_print_end(const az_vm *vm);

#endif
