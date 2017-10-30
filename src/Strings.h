#pragma once

typedef const char* string;

//
// this is a dynamic array of strings
// the instance owns the strings and frees them at will
class strings {
public:
  strings();
  ~strings();
  int getCount();
  char** string;
  void resize(int n);
  void clear();
  void append(char *s);
private:
  int count= 0;

};
