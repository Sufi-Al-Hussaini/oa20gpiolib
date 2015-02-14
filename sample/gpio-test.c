/*
* Compile using: gcc gpio-test.c -o gpio-test -loa20_gpio -lrt
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <oa20_gpio.h>


int main(int argc , char *argv[]){
    
    if (SETUP_OK==sunxi_gpio_init()){
        printf("GPIO initialized!\n");
    }
    
    pinMode(PIN_PD5, INPUT);
    pullUpDnControl(PIN_PD5, 1);
    pinMode(PIN_PD4, INPUT);
    pullUpDnControl(PIN_PD4, 1);
    pinMode(PIN_PD3, INPUT);
    pullUpDnControl(PIN_PD3, 1);
    
    printf("Waiting for input...\n");
    
    while(1){
        usleep(20000);
        printf("Values of pins PIN_PD3 = %d, PIN_PD4 = %d, PIN_PD5 = %d\n", digitalRead(PIN_PD5), digitalRead(PIN_PD4), digitalRead(PIN_PD3));
    }
    
    return 0;
}