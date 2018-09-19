#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "fact.h"

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("usage:  %s N\n\n", argv[0]);
    return 0;
  }

  int num = atoi(argv[1]);
  /* check runtime conditions */
  assert(num >= 0);

  /* return result */
  printf("Result:  %d\n" , fact(num));
  return 0;
}
