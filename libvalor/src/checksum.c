#include <valor/checksum.h>
#include <valor/array.h>

#include <stdlib.h>
#include <string.h>

/**
 * Calculate and print the CRC32 checksums for chunks of a file.
 * @param file Pointer to FILE opened in binary read mode.
 * @param chunk_size Size of each chunk to process.
 */
array_t *calculate_checksum_chunks(FILE *file, size_t chunk_size) {
    char *buffer = malloc(chunk_size);
    if (buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory for the buffer.\n");
        return NULL;
    }

    array_t *array = array_create(128);

    uint32_t bytes_read;
    uint32_t crc;
    size_t i = 0;

    while ((bytes_read = fread(buffer, 1, chunk_size, file)) > 0) {
        if (bytes_read)
            crc = CHECKSUM_FUNCTION(buffer, bytes_read, 0);
        uint32_t *crc_ptr = (uint32_t *) malloc(sizeof(uint32_t));
        memcpy(crc_ptr, &crc, sizeof(uint32_t));
        array_add(array, crc_ptr);
    }

    free(buffer);

    if (ferror(file)) {
        fprintf(stderr, "Error reading the file.\n");
        for (; i < array->sz; ++i) {
            free(array->base[i]);
        }
        array_free(array);
        return NULL;
    }
    return array;
}


