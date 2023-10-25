#include <stdio.h>
#include <malloc.h>




void swap_pointers(void **x, void **y){
    void* temp;
    temp = *x;
    *x = *y;
    *y = temp;

}


int main() {
    int a = 1;
    int b = 2;
// for testing we use pointers to integers
    int *p = &a;
    int *q = &b;
    printf("address of p = %p and q = %p\n", p, q);
// prints p = &a and q = &b
    swap_pointers(&p, &q);
    printf("address of p = %p and q = %p\n", p, q);
// prints p = &b and q = &a

    return 0;
}