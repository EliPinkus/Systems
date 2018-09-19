#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "gcd.h"

int main(int argc, char **argv)
{

  if (argc != 3)
  {
    printf("Improper number of arguments see usage statement: \n");
    printf("usage:  %s a b\n\n", argv[0]);
    return 0;
  }

  int a = atoi(argv[1]);
  int b = atoi(argv[2]);

  /* check runtime conditions */
  assert(a > 0);
  assert(b > 0);


  /* print result */
  printf("Result:  %d\n" , gcd(a,b));
  return 0;
}
