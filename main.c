
#include <stdio.h> 
#include "spi.h"

void main(void)
{
	printf("rf-transfer\n");

	//
	system("socat -d -d -lf socat.log pty,raw,echo=0 pty,raw,echo=0 & ");
	printf("init socat\r\n");
	sleep(2);

	system( "ln -s /dev/pts/1 /dev/ttyGW0");
	system( "ln -s /dev/pts/2 /dev/ttyGW1");
	

	com_init();
	spi_init();
	//gpio_init();
	while(1);
}