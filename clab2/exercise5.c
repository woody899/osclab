#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

#define MIN (-4)
#define MAX (9)
#define FREQUENCY (15)

float temperature;


int main(){

    time_t tm;

    srand(time(NULL));
    
    int i;
    for(i = 0; i < FREQUENCY; i++){
        time(&tm);
        temperature = rand() % MAX + (MIN);
        printf("%1.2f @ %s",temperature, ctime(&tm));
    }

}


