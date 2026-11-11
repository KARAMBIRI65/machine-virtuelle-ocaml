/* =========================
   File: az_vm_core.c
   ========================= */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "az_vm_core.h"
#include "az_primitives.h"
#include "az_values.h"

/* @requires : vm != NULL
   @assigns  : nothing
   @ensures  : returns 1 iff pc..pc+needed are valid indices in vm->code */
static int az_check_pc(const az_vm *vm, int needed) {
  if (vm->pc < 0) return 0;
  if (vm->pc + needed >= vm->code_len) return 0;
  return 1;
}

/* @requires : none
   @assigns  : outputs to stderr, terminates process
   @ensures  : exits with code 2 */
static void az_fatal_div0(void) {
  fprintf(stderr, "Fatal error: exception Division_by_zero\n");
  exit(2);
}

/* @requires : vm != NULL and prog != NULL
   @assigns  : vm fields, initializes stack, fills atoms[0..255]
   @ensures  : returns 1 on success, 0 on failure (stack alloc failure) */
int az_vm_init(az_vm *vm, const az_program *prog) {
  int i;

  if (vm == NULL || prog == NULL) return 0;

  vm->code = prog->codes;
  vm->code_len = prog->code_count;

  vm->globals = prog->globals;
  vm->globals_len = prog->global_count;

  vm->pc = 0;
  vm->acc = AZ_UNIT;

  if (!az_stack_init(&vm->stack)) return 0;

  /* Atoms: we just need distinct addresses. */
  /* termination reason: i increases strictly until i == 256 */
  i = 0;
  while (i < 256) {
    vm->atoms[i] = (value_t)(intptr_t)malloc(0);
    i = i + 1;
  }

  return 1;
}

/* @requires : vm != NULL
   @assigns  : destroys vm->stack
   @ensures  : stack freed; atoms/blocks not freed (per simplified spec) */
void az_vm_destroy(az_vm *vm) {
  if (vm == NULL) return;
  az_stack_destroy(&vm->stack);
}

/* @requires : vm != NULL
   @assigns  : stdout output
   @ensures  : prints vm final state exactly in the required format */
void az_vm_print_end(const az_vm *vm) {
  int i;

  printf("Index: %d\n", vm->pc);
  printf("Accumulator: %lld\n", (long long)vm->acc);

  printf("Stack:\n");
  /* termination reason: i increases strictly until i == vm->stack.size */
  i = 0;
  while (i < vm->stack.size) {
    value_t v = az_stack_peek_depth(&vm->stack, i);
    printf("%lld\n", (long long)v);
    i = i + 1;
  }

  printf("Global:\n");
  /* termination reason: i increases strictly until i == vm->globals_len */
  i = 0;
  while (i < vm->globals_len) {
    printf("%d %lld\n", i, (long long)vm->globals[i]);
    i = i + 1;
  }
}

/* @requires : vm != NULL
   @assigns  : vm state changes while interpreting bytecode
   @ensures  : returns vm->acc when opcode STOP is executed; exits on fatal errors */
value_t az_vm_run(az_vm *vm) {
  int running;

  running = 1;

  /* termination reason: STOP sets running=0, otherwise errors exit */
  while (running) {
    code_t op;

    if (!az_check_pc(vm, 0)) {
      fprintf(stderr, "PC out of bounds\n");
      exit(1);
    }

    op = vm->code[vm->pc];

    switch (op) {

      /* ========== BASIC ACC ========== */
      case 0:  vm->acc = az_stack_peek_depth(&vm->stack, 0); vm->pc = vm->pc + 1; break;
      case 1:  vm->acc = az_stack_peek_depth(&vm->stack, 1); vm->pc = vm->pc + 1; break;
      case 2:  vm->acc = az_stack_peek_depth(&vm->stack, 2); vm->pc = vm->pc + 1; break;
      case 3:  vm->acc = az_stack_peek_depth(&vm->stack, 3); vm->pc = vm->pc + 1; break;
      case 4:  vm->acc = az_stack_peek_depth(&vm->stack, 4); vm->pc = vm->pc + 1; break;
      case 5:  vm->acc = az_stack_peek_depth(&vm->stack, 5); vm->pc = vm->pc + 1; break;
      case 6:  vm->acc = az_stack_peek_depth(&vm->stack, 6); vm->pc = vm->pc + 1; break;
      case 7:  vm->acc = az_stack_peek_depth(&vm->stack, 7); vm->pc = vm->pc + 1; break;

      case 8: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "ACC needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        vm->acc = az_stack_peek_depth(&vm->stack, n);
        vm->pc = vm->pc + 2;
      } break;

      case 9:
        if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "Stack push failed\n"); exit(1); }
        vm->pc = vm->pc + 1;
        break;

      /* PUSHACC0..7 */
      case 10: if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
               vm->acc = az_stack_peek_depth(&vm->stack, 0); vm->pc = vm->pc + 1; break;
      case 11: if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
               vm->acc = az_stack_peek_depth(&vm->stack, 1); vm->pc = vm->pc + 1; break;
      case 12: if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
               vm->acc = az_stack_peek_depth(&vm->stack, 2); vm->pc = vm->pc + 1; break;
      case 13: if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
               vm->acc = az_stack_peek_depth(&vm->stack, 3); vm->pc = vm->pc + 1; break;
      case 14: if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
               vm->acc = az_stack_peek_depth(&vm->stack, 4); vm->pc = vm->pc + 1; break;
      case 15: if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
               vm->acc = az_stack_peek_depth(&vm->stack, 5); vm->pc = vm->pc + 1; break;
      case 16: if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
               vm->acc = az_stack_peek_depth(&vm->stack, 6); vm->pc = vm->pc + 1; break;
      case 17: if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
               vm->acc = az_stack_peek_depth(&vm->stack, 7); vm->pc = vm->pc + 1; break;

      case 18: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "PUSHACC needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
        vm->acc = az_stack_peek_depth(&vm->stack, n);
        vm->pc = vm->pc + 2;
      } break;

      case 19: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "POP needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        if (n < 0 || n > vm->stack.size) { fprintf(stderr, "POP invalid n\n"); exit(1); }
        az_stack_pop_n(&vm->stack, n);
        vm->pc = vm->pc + 2;
      } break;

      case 20: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "ASSIGN needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        az_stack_set_depth(&vm->stack, n, vm->acc);
        vm->acc = AZ_UNIT;
        vm->pc = vm->pc + 2;
      } break;

      case 92: /* CHECK_SIGNALS : do nothing */
        vm->pc = vm->pc + 1;
        break;

      case 143: /* STOP */
        running = 0;
        break;

      /* ========== BRANCHES ========== */
      case 84: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "BRANCH needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        vm->pc = vm->pc + n + 1;
      } break;

      case 85: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "BRANCHIF needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        if (!az_is_false(vm->acc)) vm->pc = vm->pc + n + 1;
        else vm->pc = vm->pc + 2;
      } break;

      case 86: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "BRANCHIFNOT needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        if (az_is_false(vm->acc)) vm->pc = vm->pc + n + 1;
        else vm->pc = vm->pc + 2;
      } break;

      case 87: {
        int n;
        value_t i;
        int offset;

        if (!az_check_pc(vm, 1)) { fprintf(stderr, "SWITCH needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];

        if (!az_is_int(vm->acc)) {
          vm->pc = vm->pc + 2 + n;
          break;
        }

        i = az_decode_int(vm->acc);
        if (i < 0 || i >= (value_t)n) {
          vm->pc = vm->pc + 2 + n;
          break;
        }

        offset = vm->code[vm->pc + 2 + (int)i];
        vm->pc = vm->pc + offset + 2;
      } break;

      case 88: /* BOOLNOT */
        if (az_is_false(vm->acc)) vm->acc = AZ_TRUE;
        else vm->acc = AZ_FALSE;
        vm->pc = vm->pc + 1;
        break;

      case 121: { /* EQ */
        value_t v = az_stack_pop(&vm->stack);
        vm->acc = (v == vm->acc) ? AZ_TRUE : AZ_FALSE;
        vm->pc = vm->pc + 1;
      } break;

      case 122: { /* NEQ */
        value_t v = az_stack_pop(&vm->stack);
        vm->acc = (v == vm->acc) ? AZ_FALSE : AZ_TRUE;
        vm->pc = vm->pc + 1;
      } break;

      /* ========== INTEGERS ========== */
      case 99:  vm->acc = az_encode_int(0); vm->pc = vm->pc + 1; break;
      case 100: vm->acc = az_encode_int(1); vm->pc = vm->pc + 1; break;
      case 101: vm->acc = az_encode_int(2); vm->pc = vm->pc + 1; break;
      case 102: vm->acc = az_encode_int(3); vm->pc = vm->pc + 1; break;

      case 103: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "CONSTINT needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        vm->acc = az_encode_int((value_t)n);
        vm->pc = vm->pc + 2;
      } break;

      case 104: if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
                vm->acc = az_encode_int(0); vm->pc = vm->pc + 1; break;
      case 105: if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
                vm->acc = az_encode_int(1); vm->pc = vm->pc + 1; break;
      case 106: if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
                vm->acc = az_encode_int(2); vm->pc = vm->pc + 1; break;
      case 107: if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
                vm->acc = az_encode_int(3); vm->pc = vm->pc + 1; break;

      case 108: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "PUSHCONSTINT needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
        vm->acc = az_encode_int((value_t)n);
        vm->pc = vm->pc + 2;
      } break;

      case 109: { /* NEGINT */
        value_t n = az_decode_int(vm->acc);
        vm->acc = az_encode_int(-n);
        vm->pc = vm->pc + 1;
      } break;

      case 110: { /* ADDINT */
        value_t n = az_decode_int(vm->acc);
        value_t m = az_decode_int(az_stack_pop(&vm->stack));
        vm->acc = az_encode_int(n + m);
        vm->pc = vm->pc + 1;
      } break;

      case 111: { /* SUBINT */
        value_t n = az_decode_int(vm->acc);
        value_t m = az_decode_int(az_stack_pop(&vm->stack));
        vm->acc = az_encode_int(n - m);
        vm->pc = vm->pc + 1;
      } break;

      case 112: { /* MULINT */
        value_t n = az_decode_int(vm->acc);
        value_t m = az_decode_int(az_stack_pop(&vm->stack));
        vm->acc = az_encode_int(n * m);
        vm->pc = vm->pc + 1;
      } break;

      case 113: { /* DIVINT */
        value_t n = az_decode_int(vm->acc);
        value_t m = az_decode_int(az_stack_pop(&vm->stack));
        if (m == 0) az_fatal_div0();
        vm->acc = az_encode_int(n / m);
        vm->pc = vm->pc + 1;
      } break;

      case 114: { /* MODINT */
        value_t n = az_decode_int(vm->acc);
        value_t m = az_decode_int(az_stack_pop(&vm->stack));
        if (m == 0) az_fatal_div0();
        vm->acc = az_encode_int(n % m);
        vm->pc = vm->pc + 1;
      } break;

      case 115: { /* ANDINT */
        value_t n = az_decode_int(vm->acc);
        value_t m = az_decode_int(az_stack_pop(&vm->stack));
        vm->acc = az_encode_int((value_t)(n & m));
        vm->pc = vm->pc + 1;
      } break;

      case 116: { /* ORINT */
        value_t n = az_decode_int(vm->acc);
        value_t m = az_decode_int(az_stack_pop(&vm->stack));
        vm->acc = az_encode_int((value_t)(n | m));
        vm->pc = vm->pc + 1;
      } break;

      case 117: { /* XORINT */
        value_t n = az_decode_int(vm->acc);
        value_t m = az_decode_int(az_stack_pop(&vm->stack));
        vm->acc = az_encode_int((value_t)(n ^ m));
        vm->pc = vm->pc + 1;
      } break;

      case 118: { /* LSLINT */
        value_t n = az_decode_int(vm->acc);
        value_t m = az_decode_int(az_stack_pop(&vm->stack));
        vm->acc = az_encode_int((value_t)(n << m));
        vm->pc = vm->pc + 1;
      } break;

      case 119: { /* LSRINT (logical) */
        uint64_t un = (uint64_t)az_decode_int(vm->acc);
        uint64_t um = (uint64_t)az_decode_int(az_stack_pop(&vm->stack));
        uint64_t r = un >> um;
        vm->acc = az_encode_int((value_t)r);
        vm->pc = vm->pc + 1;
      } break;

      case 120: { /* ASRINT (arithmetic) */
        value_t n = az_decode_int(vm->acc);
        value_t m = az_decode_int(az_stack_pop(&vm->stack));
        vm->acc = az_encode_int((value_t)(n >> m));
        vm->pc = vm->pc + 1;
      } break;

      case 123: { value_t n = az_decode_int(vm->acc);
                  value_t m = az_decode_int(az_stack_pop(&vm->stack));
                  vm->acc = (n < m) ? AZ_TRUE : AZ_FALSE;
                  vm->pc = vm->pc + 1; } break;

      case 124: { value_t n = az_decode_int(vm->acc);
                  value_t m = az_decode_int(az_stack_pop(&vm->stack));
                  vm->acc = (n <= m) ? AZ_TRUE : AZ_FALSE;
                  vm->pc = vm->pc + 1; } break;

      case 125: { value_t n = az_decode_int(vm->acc);
                  value_t m = az_decode_int(az_stack_pop(&vm->stack));
                  vm->acc = (n > m) ? AZ_TRUE : AZ_FALSE;
                  vm->pc = vm->pc + 1; } break;

      case 126: { value_t n = az_decode_int(vm->acc);
                  value_t m = az_decode_int(az_stack_pop(&vm->stack));
                  vm->acc = (n >= m) ? AZ_TRUE : AZ_FALSE;
                  vm->pc = vm->pc + 1; } break;

      case 137: { uint64_t n = (uint64_t)az_decode_int(vm->acc);
                  uint64_t m = (uint64_t)az_decode_int(az_stack_pop(&vm->stack));
                  vm->acc = (n < m) ? AZ_TRUE : AZ_FALSE;
                  vm->pc = vm->pc + 1; } break;

      case 138: { uint64_t n = (uint64_t)az_decode_int(vm->acc);
                  uint64_t m = (uint64_t)az_decode_int(az_stack_pop(&vm->stack));
                  vm->acc = (n >= m) ? AZ_TRUE : AZ_FALSE;
                  vm->pc = vm->pc + 1; } break;

      case 127: {
        int m;
        value_t n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "OFFSETINT needs operand\n"); exit(1); }
        m = vm->code[vm->pc + 1];
        n = az_decode_int(vm->acc);
        vm->acc = az_encode_int(n + (value_t)m);
        vm->pc = vm->pc + 2;
      } break;

      case 129:
        vm->acc = az_is_int(vm->acc) ? AZ_TRUE : AZ_FALSE;
        vm->pc = vm->pc + 1;
        break;

      /* BEQ/BNEQ/... */
      case 131: { int m, c; value_t n;
                  if (!az_check_pc(vm, 2)) { fprintf(stderr, "BEQ needs 2 operands\n"); exit(1); }
                  m = vm->code[vm->pc + 1];
                  c = vm->code[vm->pc + 2];
                  n = az_decode_int(vm->acc);
                  if (n == (value_t)m) vm->pc = vm->pc + c + 2;
                  else vm->pc = vm->pc + 3; } break;

      case 132: { int m, c; value_t n;
                  if (!az_check_pc(vm, 2)) { fprintf(stderr, "BNEQ needs 2 operands\n"); exit(1); }
                  m = vm->code[vm->pc + 1];
                  c = vm->code[vm->pc + 2];
                  n = az_decode_int(vm->acc);
                  if (n != (value_t)m) vm->pc = vm->pc + c + 2;
                  else vm->pc = vm->pc + 3; } break;

      case 133: { int m, c; value_t n;
                  if (!az_check_pc(vm, 2)) { fprintf(stderr, "BLTINT needs 2 operands\n"); exit(1); }
                  m = vm->code[vm->pc + 1];
                  c = vm->code[vm->pc + 2];
                  n = az_decode_int(vm->acc);
                  if ((value_t)m < n) vm->pc = vm->pc + c + 2;
                  else vm->pc = vm->pc + 3; } break;

      case 134: { int m, c; value_t n;
                  if (!az_check_pc(vm, 2)) { fprintf(stderr, "BLEINT needs 2 operands\n"); exit(1); }
                  m = vm->code[vm->pc + 1];
                  c = vm->code[vm->pc + 2];
                  n = az_decode_int(vm->acc);
                  if ((value_t)m <= n) vm->pc = vm->pc + c + 2;
                  else vm->pc = vm->pc + 3; } break;

      case 135: { int m, c; value_t n;
                  if (!az_check_pc(vm, 2)) { fprintf(stderr, "BGTINT needs 2 operands\n"); exit(1); }
                  m = vm->code[vm->pc + 1];
                  c = vm->code[vm->pc + 2];
                  n = az_decode_int(vm->acc);
                  if ((value_t)m > n) vm->pc = vm->pc + c + 2;
                  else vm->pc = vm->pc + 3; } break;

      case 136: { int m, c; value_t n;
                  if (!az_check_pc(vm, 2)) { fprintf(stderr, "BGEINT needs 2 operands\n"); exit(1); }
                  m = vm->code[vm->pc + 1];
                  c = vm->code[vm->pc + 2];
                  n = az_decode_int(vm->acc);
                  if ((value_t)m >= n) vm->pc = vm->pc + c + 2;
                  else vm->pc = vm->pc + 3; } break;

      case 139: { int m, c; uint64_t n;
                  if (!az_check_pc(vm, 2)) { fprintf(stderr, "BULTINT needs 2 operands\n"); exit(1); }
                  m = vm->code[vm->pc + 1];
                  c = vm->code[vm->pc + 2];
                  n = (uint64_t)az_decode_int(vm->acc);
                  if ((uint64_t)m < n) vm->pc = vm->pc + c + 2;
                  else vm->pc = vm->pc + 3; } break;

      case 140: { int m, c; uint64_t n;
                  if (!az_check_pc(vm, 2)) { fprintf(stderr, "BUGEINT needs 2 operands\n"); exit(1); }
                  m = vm->code[vm->pc + 1];
                  c = vm->code[vm->pc + 2];
                  n = (uint64_t)az_decode_int(vm->acc);
                  if ((uint64_t)m >= n) vm->pc = vm->pc + c + 2;
                  else vm->pc = vm->pc + 3; } break;

      /* ========== PRIMITIVES CALLS ========== */
      case 93: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "C_CALL1 needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        vm->acc = az_prim_call1(n, vm->acc);
        vm->pc = vm->pc + 2;
      } break;

      case 94: {
        int n;
        value_t v2;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "C_CALL2 needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        v2 = az_stack_pop(&vm->stack);
        vm->acc = az_prim_call2(n, vm->acc, v2);
        vm->pc = vm->pc + 2;
      } break;

      case 95: {
        int n;
        value_t a2, a3;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "C_CALL3 needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        a2 = az_stack_pop(&vm->stack);
        a3 = az_stack_pop(&vm->stack);
        vm->acc = az_prim_call3(n, vm->acc, a2, a3);
        vm->pc = vm->pc + 2;
      } break;

      case 96: {
        int n;
        value_t a2, a3, a4;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "C_CALL4 needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        a2 = az_stack_pop(&vm->stack);
        a3 = az_stack_pop(&vm->stack);
        a4 = az_stack_pop(&vm->stack);
        vm->acc = az_prim_call4(n, vm->acc, a2, a3, a4);
        vm->pc = vm->pc + 2;
      } break;

      case 97: {
        int n;
        value_t a2, a3, a4, a5;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "C_CALL5 needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        a2 = az_stack_pop(&vm->stack);
        a3 = az_stack_pop(&vm->stack);
        a4 = az_stack_pop(&vm->stack);
        a5 = az_stack_pop(&vm->stack);
        vm->acc = az_prim_call5(n, vm->acc, a2, a3, a4, a5);
        vm->pc = vm->pc + 2;
      } break;

      case 98: {
        int p, n;
        value_t *args;
        int i;

        if (!az_check_pc(vm, 2)) { fprintf(stderr, "C_CALLN needs 2 operands\n"); exit(1); }
        p = vm->code[vm->pc + 1];
        n = vm->code[vm->pc + 2];
        if (p < 0) { fprintf(stderr, "C_CALLN invalid p\n"); exit(1); }

        args = (value_t *)malloc((size_t)p * sizeof(value_t));
        if (args == NULL) { fprintf(stderr, "C_CALLN alloc failed\n"); exit(1); }

        if (p > 0) args[0] = vm->acc;

        /* termination reason: i increases strictly until i == p */
        i = 1;
        while (i < p) {
          args[i] = az_stack_pop(&vm->stack);
          i = i + 1;
        }

        vm->acc = az_prim_calln(n, args, p);

        free(args);
        vm->pc = vm->pc + 3;
      } break;

      /* ========== GLOBALS ========== */
      case 53: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "GETGLOBAL needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        vm->acc = vm->globals[n];
        vm->pc = vm->pc + 2;
      } break;

      case 54: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "PUSHGETGLOBAL needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
        vm->acc = vm->globals[n];
        vm->pc = vm->pc + 2;
      } break;

      case 57: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "SETGLOBAL needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        vm->globals[n] = vm->acc;
        vm->acc = AZ_UNIT;
        vm->pc = vm->pc + 2;
      } break;

      /* ========== BLOCKS ========== */
      case 62: {
        int n;
        value_t *b;
        int i;

        if (!az_check_pc(vm, 1)) { fprintf(stderr, "MAKEBLOCK needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        if (n < 0) { fprintf(stderr, "MAKEBLOCK invalid n\n"); exit(1); }

        b = (value_t *)malloc((size_t)n * sizeof(value_t));
        if (b == NULL) { fprintf(stderr, "MAKEBLOCK alloc failed\n"); exit(1); }

        if (n > 0) b[0] = vm->acc;

        /* termination reason: i increases strictly until i == n */
        i = 1;
        while (i < n) {
          b[i] = az_stack_pop(&vm->stack);
          i = i + 1;
        }

        vm->acc = (value_t)(intptr_t)b;

        /* spec says pc advanced by 3 */
        vm->pc = vm->pc + 3;
      } break;

      case 63: {
        value_t *b = (value_t *)malloc((size_t)1 * sizeof(value_t));
        if (b == NULL) { fprintf(stderr, "MAKEBLOCK1 alloc failed\n"); exit(1); }
        b[0] = vm->acc;
        vm->acc = (value_t)(intptr_t)b;
        vm->pc = vm->pc + 2;
      } break;

      case 64: {
        value_t *b = (value_t *)malloc((size_t)2 * sizeof(value_t));
        if (b == NULL) { fprintf(stderr, "MAKEBLOCK2 alloc failed\n"); exit(1); }
        b[0] = vm->acc;
        b[1] = az_stack_pop(&vm->stack);
        vm->acc = (value_t)(intptr_t)b;
        vm->pc = vm->pc + 2;
      } break;

      case 65: {
        value_t *b = (value_t *)malloc((size_t)3 * sizeof(value_t));
        if (b == NULL) { fprintf(stderr, "MAKEBLOCK3 alloc failed\n"); exit(1); }
        b[0] = vm->acc;
        b[1] = az_stack_pop(&vm->stack);
        b[2] = az_stack_pop(&vm->stack);
        vm->acc = (value_t)(intptr_t)b;
        vm->pc = vm->pc + 2;
      } break;

      /* ========== FIELDS ========== */
      case 67: { value_t *b = (value_t *)(intptr_t)vm->acc; vm->acc = b[0]; vm->pc = vm->pc + 1; } break;
      case 68: { value_t *b = (value_t *)(intptr_t)vm->acc; vm->acc = b[1]; vm->pc = vm->pc + 1; } break;
      case 69: { value_t *b = (value_t *)(intptr_t)vm->acc; vm->acc = b[2]; vm->pc = vm->pc + 1; } break;
      case 70: { value_t *b = (value_t *)(intptr_t)vm->acc; vm->acc = b[3]; vm->pc = vm->pc + 1; } break;

      case 71: {
        int n;
        value_t *b;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "GETFIELD needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        b = (value_t *)(intptr_t)vm->acc;
        vm->acc = b[n];
        vm->pc = vm->pc + 2;
      } break;

      case 73: { value_t *b = (value_t *)(intptr_t)vm->acc; b[0] = az_stack_pop(&vm->stack); vm->acc = AZ_UNIT; vm->pc = vm->pc + 1; } break;
      case 74: { value_t *b = (value_t *)(intptr_t)vm->acc; b[1] = az_stack_pop(&vm->stack); vm->acc = AZ_UNIT; vm->pc = vm->pc + 1; } break;
      case 75: { value_t *b = (value_t *)(intptr_t)vm->acc; b[2] = az_stack_pop(&vm->stack); vm->acc = AZ_UNIT; vm->pc = vm->pc + 1; } break;
      case 76: { value_t *b = (value_t *)(intptr_t)vm->acc; b[3] = az_stack_pop(&vm->stack); vm->acc = AZ_UNIT; vm->pc = vm->pc + 1; } break;

      case 77: {
        int n;
        value_t *b;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "SETFIELD needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        b = (value_t *)(intptr_t)vm->acc;
        b[n] = az_stack_pop(&vm->stack);
        vm->acc = AZ_UNIT;
        vm->pc = vm->pc + 2;
      } break;

      case 80: {
        int n;
        value_t *b = (value_t *)(intptr_t)vm->acc;
        n = (int)az_decode_int(az_stack_pop(&vm->stack));
        vm->acc = b[n];
        vm->pc = vm->pc + 1;
      } break;

      case 81: {
        int n;
        value_t v;
        value_t *b = (value_t *)(intptr_t)vm->acc;
        n = (int)az_decode_int(az_stack_pop(&vm->stack));
        v = az_stack_pop(&vm->stack);
        b[n] = v;
        vm->acc = AZ_UNIT;
        vm->pc = vm->pc + 1;
      } break;

      case 55: {
        int n, p;
        value_t *b;
        if (!az_check_pc(vm, 2)) { fprintf(stderr, "GETGLOBALFIELD needs 2 operands\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        p = vm->code[vm->pc + 2];
        b = (value_t *)(intptr_t)vm->globals[n];
        vm->acc = b[p];
        vm->pc = vm->pc + 3;
      } break;

      case 56: {
        int n, p;
        value_t *b;
        if (!az_check_pc(vm, 2)) { fprintf(stderr, "PUSHGETGLOBALFIELD needs 2 operands\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        p = vm->code[vm->pc + 2];
        if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
        b = (value_t *)(intptr_t)vm->globals[n];
        vm->acc = b[p];
        vm->pc = vm->pc + 3;
      } break;

      case 128: { /* OFFSETREF */
        int n;
        value_t *b;
        value_t m;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "OFFSETREF needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        b = (value_t *)(intptr_t)vm->acc;
        m = az_decode_int(b[0]);
        b[0] = az_encode_int(m + (value_t)n);
        vm->acc = AZ_UNIT;
        vm->pc = vm->pc + 2;
      } break;

      /* ========== ATOMS ========== */
      case 58:
        vm->acc = vm->atoms[0];
        vm->pc = vm->pc + 1;
        break;

      case 59: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "ATOM needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        vm->acc = vm->atoms[n];
        vm->pc = vm->pc + 2;
      } break;

      case 60:
        if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
        vm->acc = vm->atoms[0];
        vm->pc = vm->pc + 1;
        break;

      case 61: {
        int n;
        if (!az_check_pc(vm, 1)) { fprintf(stderr, "PUSHATOM needs operand\n"); exit(1); }
        n = vm->code[vm->pc + 1];
        if (!az_stack_push(&vm->stack, vm->acc)) { fprintf(stderr, "push failed\n"); exit(1); }
        vm->acc = vm->atoms[n];
        vm->pc = vm->pc + 2;
      } break;

      default:
        fprintf(stderr, "Unknown opcode: %d at pc=%d\n", (int)op, vm->pc);
        exit(1);
    }
  }

  return vm->acc;
}
