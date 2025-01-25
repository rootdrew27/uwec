#include <stdio.h>
#include <math.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int sum_of_digits = 0;
	int i;
	char whole_number_str[10]; 

	printf("Enter an whole number with 0 to 10 digits: ");
	scanf("%s", whole_number_str);

	if (strlen(whole_number_str) <= 10 && strlen(whole_number_str) > 0){
		{
			for(i=0; i < strlen(whole_number_str); i++)
				{
					
					sum_of_digits += (int)(whole_number_str[i] - '0');
				}
		
			printf("The sum of these digits is %d", sum_of_digits);
		}
	}
	/** loop through string, converting back to ints and add them up **/
	else printf("Invalid Input.\nPlease Restart Program");

return 0;
}