#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include "com.h"
#include "thread.h"

static int gcomfd;

int setTTY(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;
    if  ( tcgetattr( fd,&oldtio)  !=  0) 
    { 
        perror("SetupSerial 1");
        return -1;
    }
    bzero( &newtio, sizeof( newtio ) );
    newtio.c_cflag  |=  CLOCAL | CREAD; 
    newtio.c_cflag &= ~CSIZE; 

    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    switch( nEvent )
    {
    case 'O':                     //奇校验
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':                     //偶校验
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':                    //无校验
        newtio.c_cflag &= ~PARENB;
        break;
    }

switch( nSpeed )
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    if( nStop == 1 )
    {
        newtio.c_cflag &=  ~CSTOPB;
    }
    else if ( nStop == 2 )
    {
        newtio.c_cflag |=  CSTOPB;
    }
    newtio.c_cc[VTIME]  = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd,TCIFLUSH);
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        perror("com set error");
        return -1;
    }
    printf("set done!\n");
    return 0;
}

int open_port(int comport)
{
    char *dev[]={"/dev/ttyO0","/dev/ttyO1","/dev/ttyO2","/dev/ttyO3","/dev/ttyO4","/dev/ttyO5",};
    int fd;
    //long  vdisable;


    fd = open( dev[comport], O_RDWR|O_NOCTTY|O_NDELAY);
    if (-1 == fd)
    {
        perror("Can't Open Serial Port");
        return(-1);
    }
    else 
    {
        printf("open %s .....\n",dev[comport]);
    }

#if 0
    if (comport==1)
    {    
        fd = open( "/dev/ttyO1", O_RDWR|O_NOCTTY|O_NDELAY);
        if (-1 == fd)
        {
            perror("Can't Open Serial Port");
            return(-1);
        }
        else 
        {
            printf("open ttyO1 .....\n");
        }
    }
    else if(comport==4)
    {    
        fd = open( "/dev/ttyO4", O_RDWR|O_NOCTTY|O_NDELAY);
        if (-1 == fd)
        {
            perror("Can't Open Serial Port");
            return(-1);
        }
        else 
        {
            printf("open ttyO4 .....\n");
        }    
    }
    else if (comport==5)
    {
        fd = open( "/dev/ttyO5", O_RDWR|O_NOCTTY|O_NDELAY);
        if (-1 == fd)
        {
            perror("Can't Open Serial Port");
            return(-1);
        }
        else 
        {
            printf("open ttyO5 .....\n");
        }
    }
#endif

    if(fcntl(fd, F_SETFL, 0)<0)
    {
        printf("fcntl failed!\n");
    }
    else
    {
        printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
    }

    if(isatty(STDIN_FILENO)==0)
    {
        printf("standard input is not a terminal device\n");
    }
    else
    {
        printf("isatty success!\n");
    }

    printf("fd-open=%d\n",fd);
    return fd;
}

void com_dev_open(void)
{
    if((gcomfd=open_port(4))<0)
    {
        perror("open_port error");
        //return;
    }
}

void com_para_set(void)
{
    if(setTTY(gcomfd,115200,8,'N',1)<0)
    {
        perror("set_opt error");
        return;
    }
    printf("set fd=%d\n",gcomfd);
}


void com_proc(void)
{

    char rbuff[10]={0,};
    char nread;
    char i;

	printf("start proc fd=%d\n",gcomfd);
    //read buff
    while(1)
    {
        nread=read(gcomfd,rbuff,8);
        printf("com reading ...\n");
        if(nread > 0)
        {
        	printf("nread=%d,%s\n",nread,rbuff);

		    // write data
		    i = write(gcomfd, rbuff, nread);
		    if (i < 0) {
		        printf("write data error!\n");
		        //return -1;
		    }
        }
        
        sleep(1);
    }
}

void com_init(void)
{
	TRD_t com_trd;
	com_dev_open();
	com_para_set();
	trd_create(&com_trd, (void*)&com_proc, NULL);
}
