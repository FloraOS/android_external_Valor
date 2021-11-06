#include "util.h"

#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>


bool is_int(char* str){
  size_t i;
  for(i = 0; i < strlen(str); ++i){
    if(!isdigit(str[i])){
      return false;
    }
  }
  return true;
}

