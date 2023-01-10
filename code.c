
#include "code.h"
#include "defines.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
// 256/8
Code code_init(void) {
  Code c;
  c.top = 0;
  memset(c.bits, 0, MAX_CODE_SIZE);

  return c;
}

uint32_t code_size(Code *c) { return c->top; }

bool code_empty(Code *c) {
  if (c->top == 0) {
    return true;
  }
  return false;
}

bool code_full(Code *c) {
  if (c->top >= 255) {
    return true;
  }
  return false;
}

bool code_set_bit(Code *c, uint32_t i) {

  if (i >= MAX_CODE_SIZE || code_full(c)) {
    return false;
  }
  c->bits[i / 8] |= (1UL << (i % 8));
  return true;
}

bool code_clr_bit(Code *c, uint32_t i) {

  if (i >= MAX_CODE_SIZE || code_empty(c)) {
    return false;
  }

  c->bits[i / 8] &= ~(1UL << (i % 8));
  return true;
}

bool code_get_bit(Code *c, uint32_t i) {

  uint8_t temp = (c->bits[i / 8] >> (i % 8)) & 1UL;
  if (i >= c->top || temp == 0) {
    return false;
  }

  return true;
}

bool code_push_bit(Code *c, uint8_t bit) {

  if (code_full(c)) {
    return false;
  }
  if (bit == 1) {
    code_set_bit(c, c->top);
  } else {
    code_clr_bit(c, c->top);
  }
  c->top++;
  return true;
}

bool code_pop_bit(Code *c, uint8_t *bit) {
  if (code_empty(c)) {
    return false;
  }
  c->top--;
  *bit = c->bits[c->top];
  code_clr_bit(c, c->top);
  return true;
}

void code_print(Code *c) {
  for (uint32_t x = 0; x < c->top; x++) {
    /*
    if(x % 8 ==0){

            printf("\n");
            printf("%c ", c->bits[x/8]);
    }
    */
    if (x == c->top) {
      printf("|");
    }

    if (code_get_bit(c, x)) {
      printf("1");
    } else {
      printf("0");
    }
  }
  printf("\n");
}
