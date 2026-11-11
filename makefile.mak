# =========================
# File: makefile.mak
# =========================
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

OBJ = az_vm_main.o az_vm_core.o az_stack.o az_sobf_io.o az_primitives.o az_values.o

all: az_vm_exec

az_vm_exec: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

az_vm_main.o: az_vm_main.c az_vm_core.h az_sobf_io.h
	$(CC) $(CFLAGS) -c $<

az_vm_core.o: az_vm_core.c az_vm_core.h az_stack.h az_primitives.h az_values.h
	$(CC) $(CFLAGS) -c $<

az_stack.o: az_stack.c az_stack.h az_values.h
	$(CC) $(CFLAGS) -c $<

az_sobf_io.o: az_sobf_io.c az_sobf_io.h az_values.h
	$(CC) $(CFLAGS) -c $<

az_primitives.o: az_primitives.c az_primitives.h az_values.h
	$(CC) $(CFLAGS) -c $<

az_values.o: az_values.c az_values.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o az_vm_exec az_vm_exec.exe
