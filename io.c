#include "io.h"
#include "code.h"
#include "defines.h"
#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

uint64_t bytes_read;
uint64_t bytes_written;

int read_bytes(int infile, uint8_t *buf, int nbytes) {
  uint64_t bytesCurr = 100;

  uint64_t cou = 0;
  int bytesLeft = nbytes;
  // keeps reading until there is no more bits to read or
  while (bytesLeft != 0 && bytesCurr != 0) {
    bytesCurr = read(infile, buf++, bytesLeft);
    cou += bytesCurr;
    bytesLeft -= bytesCurr;
  }

  bytes_read += cou;
  return cou;
}

int write_bytes(int outfile, uint8_t *buf, int nbytes) {
  uint64_t bytesCurr = 0;
  uint64_t cou = 0;
  int bytesLeft = nbytes;

  while (bytesLeft != 0) {

    bytesCurr = write(outfile, buf, bytesLeft);
    cou += bytesCurr;
    bytesLeft -= bytesCurr;
  }

  bytes_written += cou;
  return cou;
}
static uint8_t buffer[10000];
static uint64_t ind;
static uint64_t bufferStorage = 0;
bool read_bit(int infile, uint8_t *bit) {
  if (bufferStorage == 0) {
    bufferStorage = read_bytes(infile, buffer, BLOCK);
    bufferStorage *= 8;
  }

  // This will check if there is no more bytes to read.
  if (bufferStorage == 0) {
    return false;
  }

  *bit = (buffer[ind / 8] >> (ind % 8)) & 1;
  ind += 1;
  bufferStorage -= 1;

  return true;
}

// What i think
//  uint8_t has 0
static uint8_t bufferCode[BLOCK];
// x == index
static uint64_t index;
void write_code(int outfile, Code *c) {

  uint32_t size = code_size(c);
  // only flush when buffer isnt
  for (uint32_t x = 0; x < size; x++) {

    // calls write_bytes when bufferCode is full
    if (index != 0 && index % 4096 == 0) {
      write_bytes(outfile, bufferCode, 4096);
      index = 0;
    }

    // bit manipulate to let the buffer take in the bits in code
    if (code_get_bit(c, x)) {

      bufferCode[index / 8] |= 1 << (index % 8);
    }

    index++;
  }
  /*
static uint64_t totalSize;
  for(uint32_t i = 0; i <= (index)/8; i++){
          printf("%c",bufferCode[i]);

  }
  */
}
// only flush at the end
void flush_codes(int outfile) {
  for (uint64_t i = index + 1; i < 4096; i++) {
    bufferCode[i] = 0;
  }
  write_bytes(outfile, bufferCode, (index / 8));
}
