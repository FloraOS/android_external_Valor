#include <valor/checksum.h>
#include <valor/array.h>
#include <stdlib.h>

/**
 * Calculate and print the CRC32 checksums for chunks of a file.
 * @param file Pointer to FILE opened in binary read mode.
 * @param chunk_size Size of each chunk to process.
 */
array_t* calculate_checksum_chunks(FILE *file, size_t chunk_size) {
    char *buffer = malloc(chunk_size);
    if (buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory for the buffer.\n");
        return NULL;
    }

    array_t* array = array_create(128);

    uint32_t bytes_read;
    uint32_t crc;

    while ((bytes_read = fread(buffer, 1, chunk_size, file)) > 0) {
        crc = CHECKSUM_FUNCTION(buffer, bytes_read, 0);
        array_add(array, &crc);
    }

    free(buffer);

    if (ferror(file)) {
        fprintf(stderr, "Error reading the file.\n");
        array_free(array);
        return NULL;
    }
    return array;
}


