#ifndef VALOR_CHECKSUM_H
#define VALOR_CHECKSUM_H

#include <stdio.h>
#include <stdint.h>

#include <valor/array.h>
#include <valor/crc32/crc32.h>
#define CHECKSUM_FUNCTION crc32

typedef uint32_t checksum_t;

array_t *calculate_checksum_chunks(FILE *file, size_t chunk_size);


#endif
