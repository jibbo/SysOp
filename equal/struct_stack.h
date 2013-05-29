#ifndef STRUCT_STACK_H

#define STRUCT_STACK_H
#define DIM 1000

typedef struct {
  int indice;
  int elem[DIM];
} stack;

void init(stack & );
void push (int, stack &);
void top (int &, const stack &);
void pop (stack &);

#endif