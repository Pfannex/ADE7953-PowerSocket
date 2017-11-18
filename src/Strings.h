#pragma once

typedef const char* string;

//
// this is a dynamic array of strings
// the instance owns the strings and frees them at will
class strings {
public:
  strings();
  ~strings();
  unsigned int getCount();
  char** string;
  void resize(unsigned int n);
  void clear();
  void append(char *s);
  void replace(unsigned int i, char *s);
private:
  int count= 0;

};
