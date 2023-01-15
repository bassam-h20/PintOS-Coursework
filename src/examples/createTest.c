#include <stdio.h>
#include <syscall.h>


int main(int argc, char* argv[]) {
  create(argv[1], 5 /*argv[2]*/);

  }