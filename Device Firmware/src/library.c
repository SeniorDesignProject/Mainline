#include "library.h"

char RandNum() {
  return (char)((rand()%10)+48);
}

char RandAlpha() {
  return (char)((rand()%26)+65);
}

void ClearString(char *stringIn) {
  int i;
  for(i=0; i<100; i++) {
    stringIn[i] = '\0';
  }
}