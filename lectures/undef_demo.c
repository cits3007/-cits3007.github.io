#include <stdlib.h>

typedef int (*func)();

static func do_something;

static int eraseAll() {
  return system("echo 'deleting all files with rm -rf /...'");
}

void neverCalled() {
  do_something = eraseAll;
}

int main() {
  return do_something();
}
