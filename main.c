

#include "include.h"

void main(void)
{
	char str_buf[100];
	
	printf("rf-transfer\n");

	
	// system("socat -d -d -lf socat.log pty,raw,echo=0 pty,raw,echo=0 & ");
	// printf("init socat\r\n");
	// sleep(2);
	// system( "ln -s /dev/pts/1 /dev/ttyGW0");
	// system( "ln -s /dev/pts/2 /dev/ttyGW1");

	if(socat_declare()==1){
	  sprintf(str_buf,"ln -s /dev/pts/%d /dev/ttyGW0",pts_num0);
	  printf("system (%s)\n", str_buf);
		// system("ln -s /dev/pts/3 /dev/ttyGW0");
		system( str_buf);
	  sprintf(str_buf,"ln -s /dev/pts/%d /dev/ttyGW1",pts_num1);
	  printf("system (%s)\n", str_buf);
		// system("ln -s /dev/pts/4 /dev/ttyGW1");
		system( str_buf);
		//system("rm string.log");		
	}
	else{
		printf("open dev error !\n");
		exit(0);
	}


	com_init();
	spi_init();
	//gpio_init();
	while(1);
}