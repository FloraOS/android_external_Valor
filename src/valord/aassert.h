#ifndef AASSERT_H
#define AASSERT_H

#include <stdbool.h>

#define aassert(e) __aassert(e, __FILE__, __LINE__)
#define cerror(c) __cerror(c, __FILE__, __LINE__)

void __aassert(bool e, const char* fname, int line);
void __cerror(const char* _name, const char* _file, int _line);

#endif
