#include "stdio.h"

int main(){
    char *x;
    printf("The size of integer is %zu \n", sizeof(int));
    printf("The size of pointer is %zu \n ", sizeof(x));
    printf("The size of float is %zu \n", sizeof(float));
    printf("The size of double is %zu \n", sizeof(double));
    printf("The size of void is %zu \n", sizeof(void));
    return 0;
}