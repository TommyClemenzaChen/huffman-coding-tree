#include "code.h"
#include "defines.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Node *build_tree(uint64_t hist[static ALPHABET]) {
  PriorityQueue *q = pq_create(ALPHABET);

  for (uint64_t x = 0; x <= 255; x++) {
    if (hist[x] > 0) {

      Node *temp = node_create(x, hist[x]);
      enqueue(q, temp);
    }
  }

  Node *l;
  Node *r;
  while (pq_size(q) > 1) {

    dequeue(q, &l);
    dequeue(q, &r);

    Node *parent = node_join(l, r);
    enqueue(q, parent);
  }
  Node *res;
  dequeue(q, &res);
  pq_delete(&q);
  return res;
}

// psedocode from another person, not copy code
static Code c;
static bool first = true;
void build_codes(Node *root, Code table[static ALPHABET]) {
  if (first) {
    Code temp = code_init();
    c = temp;
    first = false;
  }

  if (root != NULL) {
    if (root->left == NULL && root->right == NULL) {

      table[root->symbol] = c;

    } else {
      uint8_t trash = 0;
      code_push_bit(&c, 0);
      build_codes(root->left, table);
      code_pop_bit(&c, &trash);

      code_push_bit(&c, 1);
      build_codes(root->right, table);
      code_pop_bit(&c, &trash);
    }
  }
}

void dump_tree(int outfile, Node *root) {
  if (root != NULL) {
    dump_tree(outfile, root->left);
    dump_tree(outfile, root->right);

    // See if its a leaf or not a leaf
    uint8_t L = 'L';
    uint8_t I = 'I';
    if (root->left == NULL && root->right == NULL) {
      //
      write_bytes(outfile, &L, 1);
      write_bytes(outfile, &root->symbol, 1);

    } else {
      write_bytes(outfile, &I, 1);
    }
  }
  return;
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
  Stack *s = stack_create(nbytes);
  Node *firstVal = node_create(tree[0], 0);
  stack_push(s, firstVal);
  int index = 0;

  while (index < nbytes) {

    if (tree[index] == 'L') {
      index++;
      Node *leaf = node_create(tree[index], 0);
      stack_push(s, leaf);

      // If it is an interior node, join the next two node with this node being
      // the parent
    } else if (tree[index] == 'I') {
      Node *right;
      Node *left;

      stack_pop(s, &right);
      stack_pop(s, &left);

      Node *parent = node_join(left, right);
      stack_push(s, parent);
    }

    index++;
  }

  Node *res;
  stack_pop(s, &res);
  return res;
}

void delete_tree(Node **root) {
  // check if the root is null
  if (*root != NULL) {

    delete_tree(&(*root)->left);
    delete_tree(&(*root)->right);

    node_delete(root);
  }
  return;
}
