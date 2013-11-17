#include <stdio.h>
#include "../gpio/oa20_gpio.h" 

inline void msleep(long int t)
{
   usleep(t*1000);
}
void main(void)
{
int i=0;
long int t=0;
if (SETUP_OK==sunxi_gpio_init())
{
  oa20_gpio_set_cfgpin(3,7,OUTPUT);
  while (1){
   if (i==0) i=1;
   else i=0;
   oa20_gpio_output(3,7,i);
   msleep(1000
);
   printf("set =%d\n",i);
  }
  sunxi_gpio_cleanup();
}

}
