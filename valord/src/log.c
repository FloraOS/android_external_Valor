#include "log.h"

#include "util.h"

#include <stdarg.h>

#include <valor/config.h>

#ifdef ANDROID
#include <android/log.h>
#else
#include <stdio.h>
#endif


void debug(const char *format, ...){
#if DEBUG
    va_list args;
    va_start(args, format);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_DEBUG, MODNAME, format, args);
#else
    fprintf(stdout, "DEBUG |");
    fprintf(stdout, format, args);
    fprintf(stdout, "\n");
#endif
    va_end(args);
#else
    UNUSED(format);
#endif
}

void info(const char *format, ...){
    va_list args;
    va_start(args, format);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_INFO, MODNAME, format, args);
#else
    fprintf(stdout, "INFO  |");
    fprintf(stdout, format, args);
    fprintf(stdout, "\n");
#endif
    va_end(args);
}

void warn(const char *format, ...){
    va_list args;
    va_start(args, format);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_WARN, MODNAME, format, args);
#else
    fprintf(stdout, "WARN  |");
    fprintf(stdout, format, args);
    fprintf(stdout, "\n");
#endif
    va_end(args);
}

void error(const char *format, ...){
    va_list args;
    va_start(args, format);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_ERROR, MODNAME, format, args);
#else
    fprintf(stdout, "ERROR |");
    fprintf(stdout, format, args);
    fprintf(stdout, "\n");
#endif
    va_end(args);
}

void fatal(const char *format, ...){
    va_list args;
    va_start(args, format);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_ERROR, MODNAME, format, args);
#else
    fprintf(stdout, "FATAL |");
    fprintf(stdout, format, args);
    fprintf(stdout, "\n");
#endif
    va_end(args);
}
