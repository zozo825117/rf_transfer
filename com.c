#include <stdio.h> 
#include <unistd.h> 
#include <termios.h> 
#include <fcntl.h> 
#include <sys/ioctl.h> 
#include "com.h"
#include <sys/types.h>
// 设定波特率
/** 
*@brief  设置串口通信速率 
*@param  fd     类型 int  打开串口的文件句柄 
*@param  speed  类型 int  串口速度 
*@return  void 
*/ 
int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300, 
          B38400, B19200, B9600, B4800, B2400, B1200, B300, }; 
int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,   
          19200,  9600, 4800, 2400, 1200,  300, }; 
void set_speed(int fd, int speed)
{ 
  int   i;  
  int   status;  
  struct termios Opt; 
  tcgetattr(fd, &Opt);  
  for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) {  
    if  (speed == name_arr[i]) {      
      tcflush(fd, TCIOFLUSH);      
      cfsetispeed(&Opt, speed_arr[i]);   
      cfsetospeed(&Opt, speed_arr[i]);    
      status = tcsetattr(fd, TCSANOW, &Opt);   
      if  (status != 0) {         
        perror("tcsetattr fd");   
        return;      
      }     
      tcflush(fd,TCIOFLUSH);    
    }   
  } 
} 

// 设置串口数据位，停止位和效验位
/** 
*@brief   设置串口数据位，停止位和效验位 
*@param  fd     类型  int  打开的串口文件句柄 
*@param  databits 类型  int 数据位   取值 为 7 或者8 
*@param  stopbits 类型  int 停止位   取值为 1 或者2 
*@param  parity  类型  int  效验类型 取值为N,E,O,,S 
*/ 
int set_Parity(int fd,int databits,int stopbits,int parity) 
{  
    struct termios options;  
    if (tcgetattr(fd,&options) != 0) 
    {  
        perror("SetupSerial 1");      
            return(-1);   
    } 
    options.c_cflag &= ~CSIZE;  
    switch (databits) /*设置数据位数*/ 
    {    
        case 7:    
            options.c_cflag |= CS7;  
            break; 
        case 8:      
            options.c_cflag |= CS8; 
            break;    
        default:     
            fprintf(stderr,"Unsupported data size\n"); 
        return (-1);   
    } 
    switch (parity)  
    {    
        case 'n': 
        case 'N':     
        options.c_cflag &= ~PARENB;   /* Clear parity enable */ 
        options.c_iflag &= ~INPCK;     /* Enable parity checking */  
        break;   
        case 'o':    
        case 'O':      
        options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/   
        options.c_iflag |= INPCK;             /* Disnable parity checking */  
        break;   
        case 'e':   
        case 'E':    
        options.c_cflag |= PARENB;     /* Enable parity */     
        options.c_cflag &= ~PARODD;   /* 转换为偶效验*/      
        options.c_iflag |= INPCK;       /* Disnable parity checking */ 
        break; 
        case 'S':  
        case 's':  /*as no parity*/    
        options.c_cflag &= ~PARENB; 
        options.c_cflag &= ~CSTOPB;
        break;   
        default:    
        fprintf(stderr,"Unsupported parity\n");     
        return (-1);   
      }   
    /* 设置停止位*/   
    switch (stopbits) 
    {    
        case 1:     
        options.c_cflag &= ~CSTOPB;   
        break;   
        case 2:     
        options.c_cflag |= CSTOPB;   
        break; 
        default:     
        fprintf(stderr,"Unsupported stop bits\n");   
        return (-1);  
    }  

    /* Set input parity option */  
    if (parity != 'n')
    {
        options.c_iflag |= INPCK; 
    } 
    tcflush(fd,TCIFLUSH); 
    options.c_lflag &= (~ECHO); 
    options.c_lflag &= (~ICANON); 
    options.c_cc[VTIME] = 1; /* 设置超时15 seconds*/    
    options.c_cc[VMIN] = 100; /* Update the options and do it NOW */ 

    //c_cc数组的VSTART和VSTOP元素被设定成DC1和DC3，代表ASCII标准的XON和XOFF字符，如果在传输这两个字符的时
    //候就传不过去，需要把软件流控制屏蔽，即： 
    options.c_iflag &= ~ (IXON | IXOFF | IXANY); 
    //有时候，在用write发送数据时没有键入回车，信息就发送不出去，这主要是因为我们在输入输出时是按照规范模式接收到
    //回车或换行才发送，而更多情况下我们是不必键入回车或换行的。此时应转换到行方式输入，不经处理直接发送，设置如
    //下： 
    options.c_lflag &= ~ (ICANON | ECHO | ECHOE | ISIG); 
    //还存在这样的情况：发送字符0X0d的时候，往往接收端得到的字符是0X0a，原因是因为在串口设置中c_iflag和c_oflag中存
    //在从NL-CR和CR-NL的映射，即串口能把回车和换行当成同一个字符，可以进行如下设置屏蔽之： 
    options.c_iflag &= ~ (INLCR | ICRNL | IGNCR); 
    options.c_oflag &= ~(ONLCR | OCRNL); 
    

    if (tcsetattr(fd,TCSANOW,&options) != 0)    
    {  
        perror("SetupSerial 3");    
        return (-1);   
    }  
    return (0);   
} 

int fd_uart0;
struct timeval uart0_tv;
fd_set uart0_ready;     /* used for select */

int fn_init_uart0(char *uart,int baudrate,int databits,int stopbits,int parity)
{
    char *dev  = "/dev/ttySU0"; 
    //uart0_tv.tv_sec = 0;
    //uart0_tv.tv_usec = 0;
    fd_uart0 = open(uart,O_RDWR);       
    if (-1 == fd_uart0)  
    {        
        perror("Can't Open Serial Port"); 
        return -1;     
    }   
    set_speed(fd_uart0,baudrate); 
    if (set_Parity(fd_uart0,databits,stopbits,parity) == -1)  
    { 
        printf("Set Parity Error\n"); 
        return -1;
    } 
    return 0;   
}


int fn_close_uart0(void)
{
    int result;
    result = close(fd_uart0);       
}

int fn_read_data_block_uart0(char* buff,int len)
{
    int nread;  
    nread = read(fd_uart0, buff, len);
    return nread;   
    /*if(nread<0) 
    {
        perror("read error"); 
    }*/ 
}

int fn_write_data_block_uart0(char* buff,int len)
{
    int nwrite; 
    nwrite = write(fd_uart0,buff,len); //向GPIO设备文件中写数据 
    return nwrite;  
    /*if(nwrite<0) 
    {
        perror("read error"); 
    }*/ 
}

// 如果fd_uart0有可读事件，则返回0,否则返回-1
// 该函数仅仅只是查询，不会等待，所以是立即返回的
int fn_poll_uart0_recv(void)
{
    FD_ZERO(&uart0_ready);
    FD_SET(fd_uart0, &uart0_ready); 
    select(fd_uart0 + 1, &uart0_ready, NULL, NULL, &uart0_tv);
    if (FD_ISSET(fd_uart0, &uart0_ready)) 
    {
        return 0;   
    }
    return -1;
}

void com_init( void)
{
    int result;
    fn_init_uart0("/dev/ttyGW0",115200,8,1,'N');
    if(result < 0) {
        printf("Open uart fail!\n");
        return 0;
    }
    else {
        printf("Open uart ok!\n");
    }

}

