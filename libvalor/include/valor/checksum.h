#ifndef VALOR_CHECKSUM_H
#define VALOR_CHECKSUM_H

#include <stdio.h>
#include <stdint.h>

#define CHECKSUM_RETURN_TYPE uint32_t
#define CHECKSUM_FUNCTION crc32

typedef uint32_t checksum_t;

void checksum_buffer(char *buf, size_t bufsize, checksum_t *checksum);

void checksum_file(FILE *file, checksum_t *checksum);


#endif
