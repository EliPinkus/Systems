#include <stdio.h>


int count_onebits(unsigned int n);


int main(int argc, char **argv) {
    int i, res;
    unsigned int n;

    if (argc == 1) {
        printf("usage:  %s N1 [N2 ...]\n\n", argv[0]);
        printf("\tN1, N2, ... are positive integers\n\n");
        printf("\tFor each argument, the program prints out the number of\n"
               "\tbits that are 1 in the number.\n\n");
        return 1;
    }

    for (i = 1; i < argc; i++) {
        res = sscanf(argv[i], "%u", &n);
        if (res == 1)
            printf("Input:  %u\tOne-bits:  %u\n\n", n, count_onebits(n));
        else
            printf("Unparseable input \"%s\".  Skipping.\n\n", argv[i]);
    }

    return 0;
}

/*
 * Given an unsigned integer n, this function returns the number of bits in n
 * that are 1.
 */
int count_onebits(unsigned int n) {


	/* initialize variables */
	int high_power;
	high_power= 0;
	int i;
	int result;
	result = 0;

	/* find lowest power of 2 that is greater than n */
	while ((1 << high_power) <= n)
	{
		high_power += 1;
	}

	/* subtract 1 to find the highest power of 2 in n */
	high_power -= 1;


	/* check if a shift of given size results in loss of a bit */
	for (i=0; i <= high_power; i++)
	{
		if (!(2 * (n >> (i+1)) == n >> i))
		{
			result += 1;
		}
	}
  
  return result;
}
