# CC		= gcc
# CFLAGS		= -Wall -Wextra
# CDEBUG		= -g
# LDFLAGS		= 
# LDLIBS		= 

# FILE		= hello
# OBJ           = hello.o

equal : equal.o lib_equal.o struct_stack.o.
	gcc equal.o lib_equal.o struct_stack.o

lib_equal.o : lib_equal.c.
	gcc -c lib_equal.c

struct_stack.o : struct_stack.c.
	gcc -c struct_stack.c

clean:
	rm equal.o

all: equal clean