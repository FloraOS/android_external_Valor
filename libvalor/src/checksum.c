#include <valor/checksum.h>

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <valor/config.h>
#include <valor/crc32/crc32.h>


//This file is middleware between *some*
//checksum alogrithm and a main part of
//program

void checksum_buffer(char *buf, size_t bufsize, checksum_t *checksum) {
    // Stub in case if there would be need
    // to calculate buffers checksums.
    errno = 45; //ENOTSUP
    buf++;
    bufsize++; //Stub for AOSP's build system -Werror flag
    checksum = NULL;
}

void checksum_file(FILE *file, checksum_t *checksum) {
    _Static_assert(sizeof(checksum_t) == sizeof(CHECKSUM_RETURN_TYPE), "Wrong declaration of checksum_t!");

    checksum_t chksum = 0;
    char *buffer = (char *) malloc(sizeof(char) * CHECKSUM_BUFSIZE);
    size_t read_size = -1;

    for (;;) {
        read_size = fread(buffer, sizeof(char), CHECKSUM_BUFSIZE, file);

        if (!read_size) {
            if (ferror(file)) {
                fprintf(stderr, "ferror: %s(%d)\n", strerror(errno), errno);
                return;
            }
            break;
        }

        chksum = crc32(buffer, read_size, chksum);
    }

    *checksum = chksum;

    free(buffer);
}
