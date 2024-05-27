#include "util.h"

#include "log.h"

#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>


/**
 * Verifies that given string contains only digits
 * @param str string to check
 * @return true if string is numeric, false otherwise
 */
bool is_int(char *str) {
    size_t i;
    for (i = 0; i < strlen(str); ++i) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}


/**
 * Prints error to log. Does not exits from program
 * @param _name name of call where error may occur
 * @param _file name of file of valor_perror call(passed by macro)
 * @param _line number of line of valor_perror call(passed by macro)
 * @return whether there was an error
 */
bool perror_internal(const char *_name, const char *_file, int _line) {
    if (errno) {
        error("%s: %s(%d)  [%s:%d]", _name, strerror(errno), errno, _file,
              _line);
        errno = 0;
        return true;
    } else {
        return false;
    }
}

