#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[]) {
    int n,i;
    int d2,count;
    double d1;

    while (1){
      printf("Enter a positive integer number (or 0 to quit): ");
      scanf("%d",&n);
      if (n <= 0){ //Any non-positive number quits
        break;
      }
      count=0;
      for (i=0; i<(n); i++){
        d1=(double)n/(double)i;
        d2=n/i;
        if (fabs(d1-(double)d2) < 0.00001){
          count++; //Counts number of divisors
        }
      }
      if (count == 2){
        //When a positive integer has two distinct factors
        //  (one and itself), it is prime.
        printf("%d is prime\n",n);
      }
      else{
        //Otherwise, the positive integer is composite
        printf("%d is not prime\n",n);
      }
    }
    return 0;
}