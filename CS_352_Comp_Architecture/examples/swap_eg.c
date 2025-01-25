#include <stdio.h>

void swap (int **a, int **b);

int main(){

    int x = 2;
    int *A = &a;
    int y = 3;
    int *B = &b;

    swap(&A, &B);



    return 0;
}

//swaps the values of the pointers passed (meaning that the memory addresses held by two pointers are swapped between the pointers). 
//'**a' is a reference to a pointer --> deferening 'a' thus produces a pointer 
void swap (int **a, int **b){
    int *temp = *a;
    *a = *b;
    *b = temp;
    return;
}

//swaps the values at a and b
void swap(int *a , int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
    return;
}