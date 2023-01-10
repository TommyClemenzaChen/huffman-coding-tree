
#include "code.h"
#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "h i: o: v"

void printTree(Node *t) {

  if (t != NULL) {

    printTree(t->left);
    node_print(t);
    printTree(t->right);
  }
  return;
}

int main(int argc, char **argv) {
  int opt = 0;
  // input file, output file and statistics

  int infile = STDIN_FILENO;
  int outfile = STDOUT_FILENO;
  bool stats = false;
  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    switch (opt) {
    case ('i'):
      infile = open(optarg, O_RDONLY);
      break;
    case ('o'):
      outfile = open(optarg, O_WRONLY);
      break;
    case ('v'):
      stats = true;
      break;
    case ('h'):
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman decoder.\n");
      fprintf(stderr,
              "  Decompresses a file using the Huffman coding algorithm.\n\n");

      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  ./decode [-h] [-i infile] [-o outfile]\n\n");

      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to decompress.\n");

      fprintf(stderr, "  -o outfile     Output of decompressed data.\n");
      return 0;
    default:
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman decoder.\n");
      fprintf(stderr,
              "  Decompresses a file using the Huffman coding algorithm.\n\n");

      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  ./decode [-h] [-i infile] [-o outfile]\n\n");

      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to decompress.\n");
      fprintf(stderr, "  -o outfile     Output of decompressed data.\n");

      return 1;
    }
  }
  // start coding!
  uint8_t treeBuffer[BLOCK];
  uint8_t bit;

  Header h;
  read_bytes(infile, (uint8_t *)&h, sizeof(Header));
  if (h.magic != MAGIC) {
    printf("Error: unable to read header.\n");
    return 1;
  }

  fchmod(outfile, h.permissions);
  read_bytes(infile, treeBuffer, h.tree_size);

  Node *root = rebuild_tree(h.tree_size, treeBuffer);

  Node *tempRoot = root;
  uint64_t charCount = 0;
  while (read_bit(infile, &bit) && charCount < h.file_size) {
    // checks if its a leaf node
    if (tempRoot->left == NULL && tempRoot->right == NULL) {
      tempRoot = root;
      write_bytes(outfile, &treeBuffer[charCount], 1);

      charCount++;
    }

    if (bit == 0) {
      tempRoot = tempRoot->left;

    } else if (bit == 1) {
      tempRoot = tempRoot->right;
    }
  }

  struct stat fileStat;
  struct stat fileSt;

  fstat(infile, &fileStat);
  fstat(outfile, &fileSt);
  if (stats) {
    fprintf(stderr, "Uncompressed file size: %ld bytes\n", fileStat.st_size);
    fprintf(stderr, "Compressed file size: %ld bytes\n", fileSt.st_size);
    fprintf(stderr, "Space saving: %.2f%%\n",
            (100 * (1 - ((double)fileSt.st_size / (double)fileStat.st_size))));
  }

  close(infile);
  close(outfile);
  return 0;
}
