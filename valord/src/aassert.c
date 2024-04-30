#include "aassert.h"

#include "log.h"

#include <valor/config.h>

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

void __aassert(bool e, const char *fname, int line) {
    if (!e) {
        fatal("Assertion failed at %s:%d", fname, line);
        exit(-1);
    }
}

void __cerror(const char *_name, const char *_file, int _line) {
    if (errno) {
        fatal("%s: %s(%d)  [%s:%d]", _name, strerror(errno), errno, _file,
                            _line);
        exit(-1);
    }
}

