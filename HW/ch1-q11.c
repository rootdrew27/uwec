#include <stdio.h>


int main()
{
int n[5],s,i,j,w;


for(i=0; i<5; i++)
  {
  printf("Enter an integer: ");
  scanf("%d", &(n[i]));
  ; /* find index of the smallest */
  for(j=1; j<=1; j++)
    if (n[j] < n[s])
      s=j;
  w=n[i];
  n[i]=n[s];
  n[s]=w;
  }
  for(i=0; i<5; i++)
    printf("%d\n", n[i]);


}
