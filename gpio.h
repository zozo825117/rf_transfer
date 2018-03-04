#ifndef _GPIO_H_
#define _GPIO_H_


#include "include.h"

#define MSG(args...) printf(args) 

#define LED_GPIO        53
#define KEY_GPIO        48

//函数声明
int gpio_export(int pin);
int gpio_unexport(int pin);
int gpio_direction(int pin, int dir);
int gpio_write(int pin, int value);
int gpio_read(int pin);
void gpio_init(void);

#endif /*_GPIO_H_*/