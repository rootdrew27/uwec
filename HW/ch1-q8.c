#include <stdio.h>
#include <math.h>
int main(int argc, char *argv[])
{
  double n;
	double sq_root;
	int nearest_wNumber;

	while(1)
	{
		printf("Enter a positive whole number: ");
		scanf("%lf", &n);
		if (n <= 0)
			break;

		sq_root = sqrt(n);
		double rounded_root = round(sq_root);
		nearest_wNumber = (int)(rounded_root*rounded_root);

		printf("The nearest integer with a whole number square root is %d", nearest_wNumber);
		return 0;
	}

}