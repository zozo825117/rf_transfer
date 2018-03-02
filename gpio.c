//#include <stdlib.h>  
//#include <stdio.h > 
#include <string.h>
//#include <unistd.h>
//#include <fcntl.h >
#include <poll.h>

#include <stdint.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include "thread.h"

    
#define MSG(args...) printf(args) 

#define LED_GPIO        53
#define KEY_GPIO        48





int gpio_export(int pin)  
{  

    char buffer[64];  
    int len;  
    int fd;  



    fd = open("/sys/class/gpio/export", O_WRONLY);  
    if (fd < 0) {  
        MSG("Failed to open export for writing!\n");  
        return(-1);  
    }  



    len = snprintf(buffer, sizeof(buffer), "%d", pin);  
    if (write(fd, buffer, len) < 0) {  
        MSG("Failed to export gpio!");  
        return -1;  

    }  


    close(fd);  
    return 0;  

}  



int gpio_unexport(int pin)  
{  

    char buffer[64];  
    int len;  
    int fd;  


    fd = open("/sys/class/gpio/unexport", O_WRONLY);  
    if (fd < 0) {  
        MSG("Failed to open unexport for writing!\n");  
        return -1;  
    }  


    len = snprintf(buffer, sizeof(buffer), "%d", pin);  
    if (write(fd, buffer, len) < 0) {  
        MSG("Failed to unexport gpio!");  
        return -1;  
    }  

    close(fd);  
    return 0;  
} 


//dir: 0-->IN, 1-->OUT
int gpio_direction(int pin, int dir)  
{  
    static const char dir_str[] = "in\0out";  
    char path[64];  
    int fd;  

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);  
    fd = open(path, O_WRONLY);  
    if (fd < 0) {  
        MSG("Failed to open gpio direction for writing!\n");  
        return -1;  
    }  

    if (write(fd, &dir_str[dir == 0 ? 0 : 3], dir == 0 ? 2 : 3) < 0) {  
        MSG("Failed to set direction!\n");  
        return -1;  
    }  

    close(fd);  
    return 0;  
}  


//value: 0-->LOW, 1-->HIGH
int gpio_write(int pin, int value)  
{  
    static const char values_str[] = "01";  
    char path[64];  
    int fd;  

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);  
    fd = open(path, O_WRONLY);  
    if (fd < 0) {  
        MSG("Failed to open gpio value for writing!\n");  
        return -1;  
    }  

    if (write(fd, &values_str[value == 0 ? 0 : 1], 1) < 0) {  
        MSG("Failed to write value!\n");  
        return -1;  
    }  
    close(fd);  
    return 0;  
}

int gpio_read(int pin)  
{  
    char path[64];  
    char value_str[3];  
    int fd;  

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);  
    fd = open(path, O_RDONLY);  
    if (fd < 0) {  
        MSG("Failed to open gpio value for reading!\n");  
        return -1;  
    }  

    if (read(fd, value_str, 3) < 0) {  
        MSG("Failed to read value!\n");  
        return -1;  
    }  
    close(fd);  
    return (atoi(value_str));
}  



// none表示引脚为输入，不是中断引脚
// rising表示引脚为中断输入，上升沿触发
// falling表示引脚为中断输入，下降沿触发
// both表示引脚为中断输入，边沿触发
// 0-->none, 1-->rising, 2-->falling, 3-->both
int gpio_edge(int pin, int edge)
{
    const char dir_str[] = "none\0rising\0falling\0both"; 
    char ptr;
    char path[64];  
    int fd; 
    switch(edge){
        case 0:
            ptr = 0;
        break;

        case 1:
            ptr = 5;
        break;

        case 2:
            ptr = 12;
        break;

        case 3:
            ptr = 20;
        break;

        default:
            ptr = 0;
    } 

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/edge", pin);  
    fd = open(path, O_WRONLY);  
    if (fd < 0) {  
        MSG("Failed to open gpio edge for writing!\n");  
        return -1;  
    }  

    if (write(fd, &dir_str[ptr], strlen(&dir_str[ptr])) < 0) {  
        MSG("Failed to set edge!\n");  
        return -1;  
    }  

    close(fd);  
    return 0;  
}

void gpio_proc(void)
{
    uint16_t cnt=0;
    uint16_t ret;
    uint32_t delay = 0;
    MSG("start gpio_proc \n");

    while(1)
    {
        ret = gpio_read(KEY_GPIO);
        //MSG("key value = %d\n",ret);

        if(ret == 1)
        {
            gpio_write(LED_GPIO,1);
        }
        else if(ret == 0)
        {
            gpio_write(LED_GPIO,0);
        }
        // if(delay >=1000){
        //     gpio_write(CS0_GPIO, cnt%2);
        //     gpio_write(CS1_GPIO, (cnt+1)%2);
        //     cnt++;
        //     delay = 0;
        // }


        delay++;

        usleep(1000);
    }


}



void gpio_init(void)
{
    TRD_t gpio_trd;
//LED引脚初始化
    gpio_export(LED_GPIO);
    gpio_direction(LED_GPIO, 1);
    gpio_write(LED_GPIO, 1);
//按键引脚初始化
    gpio_export(KEY_GPIO);
    gpio_direction(KEY_GPIO, 0);
// // cs0
//     gpio_export(CS0_GPIO);
//     gpio_direction(CS0_GPIO, 1);
//     gpio_write(CS0_GPIO, 0);
// // cs1
//     gpio_export(CS1_GPIO);
//     gpio_direction(CS1_GPIO, 1);
//     gpio_write(CS1_GPIO, 0);

    trd_create(&gpio_trd, (void*)&gpio_proc, NULL);
}


// //GPIO1_17

// int main()  

// {  

//     int gpio_fd, ret;

//     struct pollfd fds[1];

//     char buff[10];

//     unsigned char cnt = 0;

// //LED引脚初始化

//     gpio_export(115);

//     gpio_direction(115, 1);

//     gpio_write(115, 0);

// //按键引脚初始化

//     gpio_export(49);

//     gpio_direction(49, 0);

//     gpio_edge(49,1);

//     gpio_fd = open("/sys/class/gpio/gpio49/value",O_RDONLY);

//     if(gpio_fd < 0){

//         MSG("Failed to open value!\n");  

//         return -1;  

//     }

//     fds[0].fd = gpio_fd;

//     fds[0].events  = POLLPRI;

//     ret = read(gpio_fd,buff,10);

//     if( ret == -1 )

//         MSG("read\n");

//     while(1){

//         ret = poll(fds,1,0);

//         if( ret == -1 )

//             MSG("poll\n");

//         if( fds[0].revents & POLLPRI){

//             ret = lseek(gpio_fd,0,SEEK_SET);

//             if( ret == -1 )

//                 MSG("lseek\n");

//             ret = read(gpio_fd,buff,10);

//             if( ret == -1 )

//                 MSG("read\n");

//             gpio_write(115, cnt++%2);

//         }

//         usleep(100000);

//     }

//     return 0;

// }  