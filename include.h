


#include <string.h>
#include <poll.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <termios.h> 
#include <sys/ioctl.h> 
#include <sys/types.h>
#include<sys/time.h>

#include <linux/types.h>
#include <linux/spi/spidev.h>


#include "gpio.h"
#include "spi.h"
#include "thread.h"
#include "com.h"
#include "socat.h"