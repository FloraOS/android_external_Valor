#include "util.h"

#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>


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

