/*
 * oa20_gpio.c
 * based on gpio_lib  Copyright 2013 Stefan Mavrodiev <support@olimex.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */


#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>
#include "oa20_gpio.h"

int  connector_pin[3][40]={
                           /* GPIO 1*/
                           {-1,-1,
                            -1,-1,
                            PIN_PG0,-1,
                            PIN_PG1,-1,
                            PIN_PG2,-1,
                            PIN_PG3,-1,
                            PIN_PG4,-1,
                            PIN_PG5,-1,
                            PIN_PG6,-1,
                            PIN_PG7,-1,
                            PIN_PG8,-1,
                            PIN_PG9,-1,
                            PIN_PG10,-1,
                            PIN_PG11,-1,
                            -1,-1,
                            -1,-1,
                            -1,-1,
                            -1,-1,
                            -1,-1,
                            -1,-1},
                           /* GPIO 2 */
                           {-1,-1,
                            -1,-1,
                            -1,PIN_PE0,
                            -1,PIN_PE1,
                            PIN_PI0,PIN_PE2,
                            PIN_PI1,PIN_PE3,
                            PIN_PI2,PIN_PE4,
                            PIN_PI3,PIN_PE5,
                            PIN_PI10,PIN_PE6,
                            PIN_PI11,PIN_PE7,
                            PIN_PC3,PIN_PE8,
                            PIN_PC7,PIN_PE9,
                            PIN_PC16,PIN_PE10,
                            PIN_PC17,PIN_PE11,
                            PIN_PC18,PIN_PI14,
                            PIN_PC23,PIN_PI15,
                            PIN_PC24,-1,
                            -1,-1,
                            -1,-1,
                            -1,-1},
                           /* GPIO 3 */
			   {-1,-1,
                            -1,-1,
                            PIN_PH0,PIN_PB3,
                            PIN_PH2,PIN_PB4,
                            PIN_PH7,PIN_PB5,
                            PIN_PH9,PIN_PB6,
			    PIN_PH10,PIN_PB7,
                            PIN_PH11,PIN_PB8,
                            PIN_PH12,PIN_PB10,
                            PIN_PH13,PIN_PB11,
                            PIN_PH14,PIN_PB12,
                            PIN_PH15,PIN_PB13,
                            PIN_PH16,PIN_PB14,
                            PIN_PH17,PIN_PB15,
                            PIN_PH18,PIN_PB16,
                            PIN_PH19,PIN_PB17,
                            PIN_PH20,PIN_PH24,
                            PIN_PH21,PIN_PH25,
                            PIN_PH22,PIN_PH26,
                            PIN_PH23,PIN_PH27}};

unsigned int SUNXI_PIO_BASE = 0;
static volatile long int *gpio_map = NULL;

int sunxi_gpio_init(void) {
    int fd;
    unsigned int addr_start, addr_offset;
    unsigned int PageSize, PageMask;


    fd = open("/dev/mem", O_RDWR);
    if(fd < 0) {
        return SETUP_DEVMEM_FAIL;
    }

    PageSize = sysconf(_SC_PAGESIZE);
    PageMask = (~(PageSize-1));

    addr_start = SW_PORTC_IO_BASE & PageMask;
    addr_offset = SW_PORTC_IO_BASE & ~PageMask;

    gpio_map = (void *)mmap(0, PageSize*2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, addr_start);
    if(gpio_map == MAP_FAILED) {
        return SETUP_MMAP_FAIL;
    }

    SUNXI_PIO_BASE = (unsigned int)gpio_map;
    SUNXI_PIO_BASE += addr_offset;

    close(fd);
    return SETUP_OK;
}

int sunxi_gpio_set_cfgpin(unsigned int pin, unsigned int val) {

    unsigned int cfg;
    unsigned int bank = GPIO_BANK(pin);
    unsigned int index = GPIO_CFG_INDEX(pin);
    unsigned int offset = GPIO_CFG_OFFSET(pin);

    if(SUNXI_PIO_BASE == 0) {
        return -1;
    }

    struct sunxi_gpio *pio =
        &((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[bank];


    cfg = *(&pio->cfg[0] + index);
    cfg &= ~(0xf << offset);
    cfg |= val << offset;

    *(&pio->cfg[0] + index) = cfg;

    return 0;
}

int sunxi_gpio_get_cfgpin(unsigned int pin) {

    unsigned int cfg;
    unsigned int bank = GPIO_BANK(pin);
    unsigned int index = GPIO_CFG_INDEX(pin);
    unsigned int offset = GPIO_CFG_OFFSET(pin);
    if(SUNXI_PIO_BASE == 0)
    {
        return -1;
    }
    struct sunxi_gpio *pio = &((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[bank];
    cfg = *(&pio->cfg[0] + index);
    cfg >>= offset;
    return (cfg & 0xf);
}
int sunxi_gpio_output(unsigned int pin, unsigned int val) {

    unsigned int bank = GPIO_BANK(pin);
    unsigned int num = GPIO_NUM(pin);

    if(SUNXI_PIO_BASE == 0)
    {
        return -1;
    }
    struct sunxi_gpio *pio =&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[bank];

    if(val)
        *(&pio->dat) |= 1 << num;
    else
        *(&pio->dat) &= ~(1 << num);

    return 0;
}

int sunxi_gpio_input(unsigned int pin) {

    unsigned int dat;
    unsigned int bank = GPIO_BANK(pin);
    unsigned int num = GPIO_NUM(pin);

    if(SUNXI_PIO_BASE == 0)
    {
        return -1;
    }

    struct sunxi_gpio *pio =&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[bank];

    dat = *(&pio->dat);
    dat >>= num;

    return (dat & 0x1);
}
void sunxi_gpio_cleanup(void)
{
    unsigned int PageSize;
    if (gpio_map == NULL)
        return;

    PageSize = sysconf(_SC_PAGESIZE);
    munmap((void*)gpio_map, PageSize*2);
}


int oa20_gpio_input(unsigned char con, unsigned int pin){
    int l_pin=connector_pin[con-1][pin-1];
    assert(l_pin != -1);
    return  sunxi_gpio_input(l_pin);    
}

int oa20_gpio_set_cfgpin(unsigned char con,unsigned int pin, unsigned int val){
    int l_pin=connector_pin[con-1][pin-1];
    assert(l_pin != -1);
    return sunxi_gpio_set_cfgpin(l_pin,val); 
}
int oa20_gpio_get_cfgpin(unsigned char con,unsigned int pin){
    int l_pin=connector_pin[con-1][pin-1];
    assert(l_pin != -1);
    return sunxi_gpio_get_cfgpin(l_pin); 

}

int oa20_gpio_output(unsigned char con,unsigned int pin, unsigned int val){
    int l_pin=connector_pin[con-1][pin-1];
    assert(l_pin != -1);
    return sunxi_gpio_output(l_pin,val);
}

unsigned int oa20_gpio_map(unsigned char con,unsigned int pin){
   return connector_pin[con-1][pin-1];
}

void pinMode(unsigned int pin, unsigned char  mode){
   sunxi_gpio_set_cfgpin(pin, mode);
} 

void digitalWrite(unsigned int pin, unsigned char value){
   sunxi_gpio_output(pin,value);
}

int digitalRead(unsigned int pin){
   return sunxi_gpio_input(pin); 
}
void _delay_ms(unsigned long delay){
  usleep(delay*1000);
}

void delayMicroseconds(unsigned int d)
{
        usleep(d);
}

uint64_t millis(void)
{
  struct timeval current;
  uint64_t elapsed ;

  gettimeofday (&current, NULL) ;
  elapsed  = (uint64_t)current.tv_sec * (uint64_t)1000 + (uint64_t)(current.tv_usec / 1000) ;

  return elapsed ;
}


