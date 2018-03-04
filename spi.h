
#ifndef _SPI_H_
#define _SPI_H_

#include "include.h"

#define ARRAY_SIZE(a) 	(sizeof(a) / sizeof((a)[0]))
#define FRAME_LENTH		(16)
//#define FRAME_LENTH		(12)

#define CS0_GPIO        22
#define CS1_GPIO        26

void spi_init(void);



#endif /*_SPI_H_*/