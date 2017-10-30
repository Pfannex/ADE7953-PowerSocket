#include "Strings.h"
#include <stdlib.h>

strings::strings() { count = 0; }

strings::~strings() {
  clear();
}

void strings::resize(int n) {
  if (count) {
    string = (char **)realloc(string, n * sizeof(char **));
  } else {
    string = (char **)malloc(n * sizeof(char **));
  }
  count = n;
}

void strings::clear() {
  // we own all the strings, thus we free them
  for(int i= 0; i< count; i++) free(string[i]);
  if (count)
    free(string);
  count= 0;
}

void strings::append(char *s) {
  resize(count+1);
  string[count-1]= s;
}

int strings::getCount() {
  return count;
}
