
#ifndef _COM_H_
#define _COM_H_

#include "include.h"

void com_init(void);
void set_speed(int fd, int speed);
int set_Parity(int fd,int databits,int stopbits,int parity) ;
extern int fn_init_uart0(char *uart,int baudrate,int databits,int stopbits,int parity);
extern int fn_close_uart0(void);
extern int fn_read_data_block_uart0(char* buff,int len);
extern int fn_write_data_block_uart0(char* buff,int len);
extern int fn_poll_uart0_recv(void);

#endif /*_COM_H_*/
