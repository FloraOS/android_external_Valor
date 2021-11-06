#include "checksum.h"

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "crc32/crc32.h"

//This file is middleware between *some* 
//checksum alogrithm and a main part of 
//program

int checksum_buffer(char* buf, size_t bufsize){
    // Stub in case if there would be need
    // to calculate buffers checksums.
    errno = 45; //ENOTSUP
    return 0;
}

int checksum_file(FILE* file, checksum_t* checksum){
  _Static_assert(sizeof(checksum_t) == sizeof(unsigned long), "Wrong declaration of checksum_t!");
  return Crc32_ComputeFile(file, checksum);
}
