#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

#define UNUSED(x) (void)x

/**
 * Prints error to log. Does not exits from program
 * @param c name of call where error may occur
 * @note Resets global errno variable to 0
 * @return whether there was an error
 */
#define perror(c) perror_internal(c, __FILE__, __LINE__)

bool is_int(char *str);
bool perror_internal(const char *_name, const char *_file, int _line);

#endif
