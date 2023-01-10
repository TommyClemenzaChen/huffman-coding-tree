#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "node.h"

Node *node_create(uint8_t symbol, uint64_t frequency) {
  Node *n = (Node *)malloc(sizeof(Node));
  n->frequency = frequency;
  n->symbol = symbol;

  n->left = n->right = NULL;

  return n;
}

void node_delete(Node **n) {
  free(*n);
  *n = NULL;
}

Node *node_join(Node *left, Node *right) {
  uint64_t sum = (left->frequency) + (right->frequency);
  Node *parent = node_create('$', sum);
  parent->left = left;
  parent->right = right;

  return parent;
}

void node_print(Node *n) {

  if (iscntrl(n->symbol) == 0 && isprint(n->symbol) != 0) {
    printf("Symbol: %c, Frequency: %lu\n", n->symbol, n->frequency);
  } else {
    printf("Symbol: 0x%02" PRIx8 ", Frequency: %lu\n", n->symbol, n->frequency);
  }
}

bool node_cmp(Node *n, Node *m) {
  // return true if first node greater freq than second
  return (n->frequency > m->frequency);
}

void node_print_sym(Node *n) {
  if (!iscntrl(n->symbol) && isprint(n->symbol)) {

    printf("Symbol: %c \n", n->symbol);
  } else {
    printf("Symbol: 0x%02" PRIx8, n->symbol);
  }
}
