#include <stdio.h>
#include <signal.h>
#include <stdlib.h>


#include "../gpio/oa20_gpio.h" 


void sig_handler(int signo)
{
  if (signo == SIGINT){
       printf("handle SIGINT\n");
       sunxi_gpio_cleanup();
       exit(1);
     }

    
}

inline void msleep(long int t)
{
   usleep(t*1000);
}

void main(void)
{
 int i=0;
 long int t=0;
 if (SETUP_OK==sunxi_gpio_init()){
    if (signal(SIGINT, sig_handler) == SIG_ERR) 
       exit(1);
    oa20_gpio_set_cfgpin(3,7,OUTPUT);
    while (1){
          i=(i+1)%2;
          oa20_gpio_output(3,7,i);
          msleep(1000);
          printf("set =%d\n",i);
  }
 }
}
