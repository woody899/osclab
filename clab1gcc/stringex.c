#include "stdio.h"
#include "string.h"
#define MAX 100
int main(){
    char first[MAX];
    char second[MAX];
    char name[MAX];
    char str[MAX];
    int year = 2001;

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


    printf("Testing concatination via strcat(), %s \n",strcat(first,second));


    printf("From here we use snprintf \n");
    snprintf(name,sizeof(name),"%s %s %n",first,second,&year);

    //Concatenated string via snprintf doesn't work?? Ask TA or Arkid
}