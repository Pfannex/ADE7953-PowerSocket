#include "Debug.h"
#include "framework/Utils/Strings.h"
#include <stdlib.h>
#include <string.h>

strings::strings() { count = 0; }

strings::~strings() { clear(); }

void strings::resize(unsigned int n) {
  if (count) {
    string = (char **)realloc(string, n * sizeof(char*));
  } else {
    string = (char **)malloc(n * sizeof(char*));
  }
  /*if(string == NULL) {
    D("@@@@@@@@@@@@@@ NULL");
  }*/
  count = n;
}

void strings::clear() {
  // we own all the strings, thus we free them
  for (unsigned int i = 0; i < count; i++)
    free(string[i]);
  if (count)
    free(string);
  count = 0;
  freeJoined();
}

void strings::append(char *s) {
  //Di("strings::append, count=", count);
  //Ds("append string:", s);
  resize(count + 1);
  string[count - 1] = s;
  //DF("appended");
  freeJoined();
}

void strings::replace(unsigned int i, char *s) {
  if (i < count) {
    free(string[i]);
    string[i] = s;
    freeJoined();
  }
}

unsigned int strings::getCount() { return count; }

void strings::freeJoined() {
  if (joined) {
    // D("freeJoined");
    free(joined);
    joined = 0;
  }
}

char *strings::join(const char *joiner) {
  if (!joined) {
    int len = 1;
    if (count> 1)
      len += (count - 1) * strlen(joiner);
    for (unsigned int i = 0; i < count; i++) {
      len += strlen(string[i]);
    }
    joined = (char *)malloc(len * sizeof(char));
    *joined = '\0';
    //Di("count", count);
    //Di("size", len);
    for (unsigned int i = 0; i < count; i++) {
      strcat(joined, string[i]);
      if (i < count - 1)
        strcat(joined, joiner);
    }
  }
  //Ds("result", joined);
  return (joined);
}
