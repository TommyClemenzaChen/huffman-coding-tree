#include "node.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "pq.h"
struct PriorityQueue {

  uint64_t tail;
  uint64_t capacity;
  Node **queue;
};

void swap(PriorityQueue *q, uint32_t index1, uint32_t index2) {
  Node *temp = q->queue[index1];
  q->queue[index1] = q->queue[index2];
  q->queue[index2] = temp;
}

int cmp(uint64_t x, uint64_t y) {
  if (x < y) {
    return -1;
  } else if (x > y) {
    return 1;
  } else {
    return 0;
  }
}

uint32_t l_child(uint32_t n) { return 2 * n + 1; }
// Gets the right child of the node
uint32_t r_child(uint32_t n) { return 2 * n + 2; }
// Gets its parent which is the node before it
uint32_t parent(uint32_t n) { return (n - 1) / 2; }
//
void up_heap(PriorityQueue *pq, uint32_t n) {
  // Trying to find if it is less
  while (n > 0 &&
         cmp(pq->queue[n]->frequency, pq->queue[parent(n)]->frequency) == -1) {
    swap(pq, n, parent(n));
    // pq->queue[n], pq->queue[parent(n)]);
    n = parent(n);
  }
}
// no clue either
void down_heap(PriorityQueue *pq, uint32_t heap_size) {
  uint32_t n = 0;
  uint32_t smaller;
  // Altered from psedo, now its Greater
  while (l_child(n) < heap_size) {
    // sees if there is no left child.
    if (r_child(n) == heap_size) {
      smaller = l_child(n);
    } else {
      // compares children to find the smaller one
      if (pq->queue[l_child(n)]->frequency <=
          pq->queue[r_child(n)]->frequency) {
        smaller = l_child(n);
      } else {
        smaller = r_child(n);
      }
    }

    if (pq->queue[n]->frequency <= pq->queue[smaller]->frequency) {
      break;
    }
    swap(pq, n, smaller);
    // swap(pq->queue[n], pq->queue[smaller]);
    n = smaller;
  }
}

PriorityQueue *pq_create(uint32_t capacity) {

  PriorityQueue *q = (PriorityQueue *)malloc(sizeof(PriorityQueue));
  q->tail = 0;
  q->capacity = capacity;
  q->queue = (Node **)malloc(capacity * sizeof(Node *));
  return q;
}

void pq_delete(PriorityQueue **q) {

  for (uint64_t x = 0; x < (*q)->tail; x++) {
    if ((*q)->queue[x] != NULL) {
      node_delete(&((*q)->queue[x]));
    }
  }
  free((*q)->queue);
  free(*q);
  *q = NULL;
}

bool pq_empty(PriorityQueue *q) {
  if (q->tail == 0) {
    return true;
  }
  return false;
}

bool pq_full(PriorityQueue *q) {
  if (q->tail == q->capacity) {
    return true;
  }
  return false;
}

uint32_t pq_size(PriorityQueue *q) { return q->tail; }

bool enqueue(PriorityQueue *q, Node *n) {
  if (pq_full(q)) {
    return false;
  }

  q->queue[q->tail] = n;
  up_heap(q, q->tail);

  q->tail++;

  return true;
}

bool dequeue(PriorityQueue *q, Node **n) {
  if (pq_empty(q)) {
    return false;
  }

  down_heap(q, q->tail);
  swap(q, 0, q->tail - 1);

  *n = q->queue[q->tail - 1];
  q->tail--;
  down_heap(q, q->tail);

  return true;
}

void pq_print(PriorityQueue *q) {
  // copy
  // Call dequeue n_elements time
  for (uint64_t x = 0; x < q->tail; x++) {
    node_print(q->queue[x]);
  }
  printf("\n");
}
