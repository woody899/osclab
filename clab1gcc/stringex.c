#include "stdio.h"
#include "string.h"
#define MAX 100
int main(){
    char first[MAX];
    char second[MAX];
    char name[MAX];
    char str[MAX];

    printf("Enter your first name: ");
    scanf("%s",first);
    printf("Hello there, %s",first);

    printf(" Now what is your second name? ");
    scanf("%s",second);

    int i = 0;
    while(second[i] != NULL){
        if(second[i] >= 97 && second[i] <= 122){
            second[i] -= 32;
        }
        str[i] = second[i];
        i++;
    }



    printf("Now testing uppercase functionality: %s \n",second);

    return 0;
}