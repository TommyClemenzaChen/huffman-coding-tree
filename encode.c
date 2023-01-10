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

/*
union {
    uint8_t b[n];
    header s;
} hh;
*/
int main(int argc, char **argv) {
  int opt = 0;

  int infile = 0;
  bool input = true;
  int outfile = STDOUT_FILENO;
  // unlink("tempfile-XXXXXX");
  bool stats = false;
  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    switch (opt) {
    case ('i'):
      input = false;
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
      fprintf(stderr, "  A Huffman encoder.\n");
      fprintf(stderr,
              "  Compresses a file using the Huffman coding algorithm.\n\n");

      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  ./encode [-h] [-i infile] [-o outfile]\n\n");

      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to compress.\n");
      fprintf(stderr, "  -o outfile     Output of compressed data.\n");
      return 0;
    default:

      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman encoder.\n");
      fprintf(stderr,
              "  Compresses a file using the Huffman coding algorithm.\n\n");

      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  ./encode [-h] [-i infile] [-o outfile]\n\n");

      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to compress.\n");
      fprintf(stderr, "  -o outfile     Output of compressed data.\n");

      return 1;
    }
  }
  if (input == true) {
    char filename[] = "tempfile-XXXXXX";
    int inTemp = mkstemp(filename);
    uint8_t inputTemp[4096];
    int byteRead = -1;

    while (byteRead != 0) {
      byteRead = read_bytes(0, inputTemp, 4096);
      write_bytes(inTemp, inputTemp, byteRead);
    }
    infile = inTemp;
    lseek(infile, 0, SEEK_SET);
  }

  // Create my histogram by counting the words
  uint64_t *hist = (uint64_t *)calloc(256, sizeof(uint64_t));

  uint8_t temp[4096];
  Code table[ALPHABET];
  uint16_t treeSize = 0;

  int byteCou = read_bytes(infile, temp, BLOCK);
  // Creating my histogram based on the data
  for (int i = 0; i < byteCou; i++) {

    if (hist[temp[i]] == 0) {
      treeSize++;
    }
    hist[temp[i]] += 1;
  }
  if (hist[0] == 0) {
    treeSize++;
    hist[0] = 1;
  }
  if (hist[1] == 0) {

    treeSize++;
    hist[1] = 1;
  }

  Node *root = build_tree(hist);
  build_codes(root, table);
  /*
  int counter = 0;
  for(int x = 0; x< ALPHABET; x++){
          if(counter == treeSize){
                  break;
          }
          if(code_empty(&table[x]) == false){
                  counter++;
                  printf("%d: ", x);
                  code_print(&table[x]);
          }
  }
  */

  struct stat fileStat;
  fstat(infile, &fileStat);

  // Header structure
  Header h;
  h.magic = MAGIC;
  h.permissions = fileStat.st_mode;
  h.tree_size = (3 * treeSize) - 1;
  h.file_size = fileStat.st_size;
  //
  fchmod(outfile, fileStat.st_mode);

  write_bytes(outfile, (uint8_t *)&h, sizeof(Header));

  dump_tree(outfile, root);

  // uint8_t * readBytes = (uint8_t *)malloc(BLOCK * sizeof(uint8_t));

  for (uint16_t i = 0; i < fileStat.st_size; i++) {

    write_code(outfile, &table[temp[i]]);
  }
  flush_codes(outfile);

  struct stat fileSt;
  fstat(outfile, &fileSt);
  if (stats) {
    fprintf(stderr, "Uncompressed file size: %ld bytes\n", fileStat.st_size);
    fprintf(stderr, "Compressed file size: %ld bytes\n", fileSt.st_size);
    fprintf(stderr, "Space saving: %.2f%%\n",
            (100 * (1 - ((double)fileSt.st_size / (double)fileStat.st_size))));
  }

  close(infile);
  close(outfile);
  free(hist);
  return 0;
}
