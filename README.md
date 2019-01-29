rf_transfer
========


liunx app
--------
spi设备读写，并用socat创建虚拟窗口，实现进程间的通讯


## Features

- [x] spi设备读写
- [x] socat创建虚拟窗口
- [x] pts 串口映射
- [x] thread


## TODO
- [ ] socket机制

## Project Organization
![](http://ww1.sinaimg.cn/large/98df967fgy1fznajt8vf3j20890gg0t5.jpg)

  
## build

- 交叉编译器 CC=arm-openwrt-linux-gnueabi-gcc
  
## Useage

```c
  if(socat_declare()==1){
    sprintf(str_buf,"ln -s /dev/pts/%d /dev/ttyGW0",pts_num0);
    //printf("system (%s)\n", str_buf);
    // system("ln -s /dev/pts/3 /dev/ttyGW0");
    system( str_buf);
    sprintf(str_buf,"ln -s /dev/pts/%d /dev/ttyGW1",pts_num1);
    //printf("system (%s)\n", str_buf);
    // system("ln -s /dev/pts/4 /dev/ttyGW1");
    system( str_buf);
    //system("rm string.log");    
  }
  else{
    printf("open dev error !\n");
    exit(0);
  }
```
创建了两个 pts虚拟串口，并关联的自定义路径

##  Reference

- [使用socat实现Linux虚拟串口](https://blog.csdn.net/rainertop/article/details/26706847)

- [Linux系统自带spi驱动加载及应用程序编写方法详解](https://blog.csdn.net/u010871058/article/details/78141190)

- [linux uart应用程序](https://www.jianshu.com/p/5c1d2bd26294?utm_campaign=maleskine&utm_content=note&utm_medium=seo_notes&utm_source=recommendation)

