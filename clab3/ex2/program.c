#define _GNU_SOURCE
/**
 * \author Bert Lagaisse
 *
 * main method that executes some test functions (without check framework)
 */

#include <stdio.h>
#include "dplist.h"
#include "dplist.c"

char x = 'a';
char y = '\e';
char z = 'b';

char* p1 = &x;
char* p2 = &y;
char* p3 = &z;

void ck_assert_msg(bool result, char * msg){
    if(!result) printf("%s\n", msg);
}
int main(void)
{
    dplist_t *numbers = NULL;
    numbers = dpl_create();

    ck_assert_msg(numbers != NULL, "numbers = NULL, List not created");
    ck_assert_msg(dpl_size(numbers) == 0, "Numbers may not contain elements.");

    dpl_insert_at_index(numbers, p1, 0);
    ck_assert_msg(dpl_size(numbers) == 1, "Numbers must contain 1 element.");

    dpl_insert_at_index(numbers, p2, -1);
    ck_assert_msg(dpl_size(numbers) == 2, "Numbers must contain 2 elements.");

    dpl_insert_at_index(numbers, p3, 100);
    ck_assert_msg(dpl_size(numbers) == 3, "Numbers must contain 3 elements.");

    dpl_free(&numbers);

    return 0;
}