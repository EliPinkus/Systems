#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "ffunc.h"


/* This function takes an array of single-precision floating point values,
 * and computes a sum in the order of the inputs.  Very simple.
 */
float fsum(FloatArray *floats) {
    float sum = 0;
    int i;

    for (i = 0; i < floats->count; i++)
        sum += floats->values[i];

    return sum;
}


/* TODO:  IMPLEMENT my_fsum().  MAKE SURE TO EXPLAIN YOUR CODE IN COMMENTS. */
float my_fsum(FloatArray *floats)
{
  /* initialize result variable */
  float temp = 0;

  /* perform naive sum if array size <= 2 */
  if (floats->count <= 2)
  {
    for (int k = 0; k < floats->count; k++)
    {
      temp += floats->values[k];
    }
  }
  else
  {
    /* initialize temp structs */
    FloatArray floats1;
    FloatArray floats2;

    /* save split point for floats */
    int mid = floats->count / 2;

    /* initialize fields of temp structs */
    floats1.count = mid;
    floats2.count = floats->count - mid;

    floats1.values = malloc(floats1.count * sizeof(float));
    floats2.values = malloc(floats2.count * sizeof(float));

    /* check for problems in allocation */
    if (floats1.values == NULL) {
        printf("ERROR:  couldn't allocate %u bytes!\n",
               (unsigned int) (floats1.count * sizeof(float)));
        exit(1);
    }
    if (floats2.values == NULL) {
        printf("ERROR:  couldn't allocate %u bytes!\n",
               (unsigned int) (floats2.count * sizeof(float)));
        exit(1);
    }

    /* Zero out the newly allocated memory. */
    memset(floats1.values, 0, floats1.count * sizeof(float));
    memset(floats2.values, 0, floats2.count * sizeof(float));



    /* split values according to midpoint */
    for (int i=0; i < floats1.count; i++)
    {
      floats1.values[i] = floats->values[i];
    }

    for (int j=0; j < floats2.count; j++)
    {
      floats2.values[j] = floats->values[j+mid];
    }

    /* recursively call my_sum for sublists and add results */
    temp = my_fsum(&floats1) + my_fsum(&floats2);

    
    free(floats1.values);
    free(floats2.values);

  }
  return temp;
}


int main() {
    FloatArray floats;
    float sum1, sum2, sum3, my_sum;

    load_floats(stdin, &floats);
    printf("Loaded %d floats from stdin.\n", floats.count);

    /* Compute a sum, in the order of input. */
    sum1 = fsum(&floats);

    /* Use my_fsum() to compute a sum of the values.  Ideally, your
     * summation function won't be affected by the order of the input floats.
     */
    my_sum = my_fsum(&floats);

    /* Compute a sum, in order of increasing magnitude. */
    sort_incmag(&floats);
    sum2 = fsum(&floats);

    /* Compute a sum, in order of decreasing magnitude. */
    sort_decmag(&floats);
    sum3 = fsum(&floats);

    /* %e prints the floating-point value in full precision,
     * using scientific notation.
     */
    printf("Sum computed in order of input:  %e\n", sum1);
    printf("Sum computed in order of increasing magnitude:  %e\n", sum2);
    printf("Sum computed in order of decreasing magnitude:  %e\n", sum3);
    printf("Sum computed by pairwise summation:  %e\n", my_sum);

    /* TODO:  UNCOMMENT WHEN READY!
    printf("My sum:  %e\n", my_sum);
    */

    return 0;
}
