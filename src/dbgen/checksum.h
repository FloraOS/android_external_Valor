#ifndef CHECKSUM_H
#define CHECKSUM_H
#include <stdio.h>

typedef unsigned long checksum_t;

int checksum_buffer(char* buf, size_t bufsize);
int checksum_file(FILE* file, checksum_t* checksum);

#endif
