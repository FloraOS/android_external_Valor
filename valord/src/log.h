#ifndef VALOR_LOG_H
#define VALOR_LOG_H
#ifndef ANDROID
#warning "You are building for non-android target, logging would be redirect to stdout"
#endif

void debug(const char *format, ...);

void info(const char *format, ...);

void warn(const char *format, ...);

void error(const char *format, ...);

void fatal(const char *format, ...);

#endif
