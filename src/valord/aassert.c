#include "aassert.h"
#include "config.h"

#include <android/log.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

void __aassert(bool e, const char* fname, int line){
     if(!e){
        __android_log_print(ANDROID_LOG_FATAL, MODNAME, "Assertion failed at %s:%d", fname, line);
        exit(-1);
     }
}

void __cerror(const char* _name, const char* _file, int _line){
     if(errno){
        __android_log_print(ANDROID_LOG_FATAL, MODNAME, "%s: %s(%d)  [%s:%d]",_name,strerror(errno),errno, _file, _line);
        exit(-1);
     }
}

