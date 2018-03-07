

#include "spi.h"


static const char *device = "/dev/spidev1.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;
static int gspifd;


static uint8_t tx[2000] = {0,};
static uint8_t rx[2000] = {0, };
uint8_t ReadBuf[2000] = {0, };
uint8_t SpiDateBuf[3000] = {0,};

static struct spi_ioc_transfer tr;



void spi_dev_open(void)
{
	//parse_opts(argc, argv);

	gspifd = open(device, O_RDWR);

	if (gspifd < 0)
		printf("can't open device");
}
void spi_dev_set(void)
{
	int ret = 0;

	/*
	 * spi mode
	 */
	ret = ioctl(gspifd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		printf("can't set spi mode");

	ret = ioctl(gspifd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		printf("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(gspifd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		printf("can't set bits per word");

	ret = ioctl(gspifd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		printf("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(gspifd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		printf("can't set max speed hz");

	ret = ioctl(gspifd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		printf("can't get max speed hz");

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
}

static int16_t spi_read_num(void)
{
	int16_t ret;

	gpio_write(CS0_GPIO, 0);
	tr.len = 1;
	tx[0] = 0x05;
	ret = ioctl(gspifd, SPI_IOC_MESSAGE(1), &tr);
	usleep(1);

	tr.len = 1;
	tx[0] = 0x00;
	ret = ioctl(gspifd, SPI_IOC_MESSAGE(1), &tr);

	gpio_write(CS0_GPIO, 1);

	if(ret > 0)
	{
		if(rx[0] != 0 && rx[0] != 255)
		{
			//printf("num = %d \r\n", rx[0]); 
		}
		
	}
	else
	{
		printf("can't send spi message");                                        
	}
	return rx[0];

}

uint8_t spi_read_data(uint16_t size)
{
    uint8_t ChkSum;
    int16_t  rssi = 0;
    uint16_t i=0,j=size;
    uint8_t ret;
	gpio_write(CS0_GPIO, 0);
	tr.len = 1;
	tx[0] = 0x03;
	ret = ioctl(gspifd, SPI_IOC_MESSAGE(1), &tr);

    usleep(1);

	// tr.len = 1;
	// tx[0] = 0x00;
	// ret = ioctl(gspifd, SPI_IOC_MESSAGE(1), &tr);

	// usleep(20);


	i=0;
  //   while(size > 0)
  //   {
		// tr.len = 1;
		// tx[0] = 0x00;
		// ret = ioctl(gspifd, SPI_IOC_MESSAGE(1), &tr);
		// if(ret > 0)
		// {
		// 	ReadBuf[i++] = rx[0];
		// }
  //       size--; 
  //       //usleep(2);
  //   }
		tr.len = size;
		memset(tx,0,sizeof(tx));
		//tx[0] = 0x00;
		ret = ioctl(gspifd, SPI_IOC_MESSAGE(1), &tr);
		if(ret > 0)
			memcpy(ReadBuf,rx,size);
		else{
			printf("read data error !");
		}

	tr.len = 1;
	tx[0] = 0x00;
	ret = ioctl(gspifd, SPI_IOC_MESSAGE(1), &tr);
	ChkSum = rx[0];

	gpio_write(CS0_GPIO, 1);
    // printf("\r\nID DATA=");
    // for(i=0;i<j;i++)
    // {
    // // 	if(i % FRAME_LENTH == 0 )
    // // 	{
    // // 		if(i != 0)
    // // 		{
				// // printf("\r\nID DATA=");
    // // 		}
    // // 		else
    // // 		{
    // // 			printf("ID DATA=");
    // // 		}
    // // 	}
    //     printf("%02x ",ReadBuf[i]);
    // }
    
    rssi = ReadBuf[i-1];
    // if(rssi >= 128)
    //     rssi=(((int)rssi-256)/2)-69;
    // else
    //     rssi =((int)rssi/ 2)-69; 
    rssi = -rssi;

    // printf("%d",rssi);
    printf("\r\n");

    return ChkSum;
}

uint8_t CheckSum(uint8_t *buf,uint16_t len)
{
    uint16_t i;
    uint16_t wChkSum = 0;
    for(i = 0; i < len; i++)
    {
        wChkSum += *(buf + i);
    }
    return((uint8_t)(wChkSum & 0x00FF));	
}

uint16_t crc16(uint8_t *ptData, size_t len,uint16_t multinomial){
	uint16_t crc_tmp = 0;
	uint16_t j;
	uint32_t i;

	//做CRC校验
	for (i = 0, crc_tmp = 0xFFFF; i < len; i++) {
		crc_tmp ^= ptData[i];
		for (j = 0; j < 8; j++) {
			if (crc_tmp & 0x01) {
				crc_tmp = (crc_tmp >> 1) ^ multinomial;
			} else {
				crc_tmp >>= 1;
			}
		}
	}

	return crc_tmp;
}

static void spi_proc(void)
{
	int16_t ret;
	int16_t num;
	uint8_t Rxsum,ChkSum;
	uint16_t i,j;
	uint16_t ReCRCnum;

	tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = ARRAY_SIZE(tx);
	tr.delay_usecs = delay;
	tr.speed_hz = speed;
	tr.bits_per_word = bits;

	printf("start spi proc fd=%d\n",gspifd);

	while(1)
	{

		num = spi_read_num();

		if(num == -1 || num == 0 || num > 100)   
		{
			// 100 --> SPIRIT 模块每次最多存100个
		}
		else
		{
		    Rxsum = spi_read_data((uint8_t)num*FRAME_LENTH);
		    ChkSum = CheckSum(ReadBuf,(uint8_t)num*FRAME_LENTH);
			if(Rxsum != ChkSum)
			{
				printf("crc error! Rxsum = 0x%02x,CheckSum=0x%02x\r\n",Rxsum,ChkSum);       
			}
			else
			{
				printf("got id num = %d\r\n",num);

				struct timeval tv;
				struct timezone tz;
				gettimeofday (&tv , &tz);
				printf("tv_sec; %d\n", tv.tv_sec) ;
				printf("tv_usec; %d\n",tv.tv_usec);
				printf("tz_minuteswest; %d\n", tz.tz_minuteswest);
				printf("tz_dsttime, %d\n",tz.tz_dsttime);

				uint16_t w_ptr=0,w_ptr_bk;
				uint16_t r_ptr=0;
				uint8_t secbuf[4] ;
				memcpy(secbuf,(uint8_t *)&tv.tv_sec,sizeof(uint32_t));
				//len = num*FRAME_LENTH;

				for(i=0;i<num;i++){
					for(j=0;j<16;j++)
					{
						if(j==0){
							for(uint8_t k=0;k<4;k++){
								SpiDateBuf[w_ptr+3-k] = secbuf[k];
							}
							w_ptr_bk = w_ptr;
							w_ptr +=4;
							SpiDateBuf[w_ptr++] = ReadBuf[r_ptr++];
						}else if(j < 14){
							SpiDateBuf[w_ptr++] = ReadBuf[r_ptr++];
						}else if(j == 14){

							ReCRCnum = crc16(&SpiDateBuf[w_ptr_bk],18,CRC_MULTI);

							SpiDateBuf[w_ptr++] = (uint8_t)(ReCRCnum>>8);
							SpiDateBuf[w_ptr++] = (uint8_t)(ReCRCnum & 0xff);

							r_ptr+=2;
						}
					}
				}

				printf("w_ptr=%d r_ptr=%d\n",w_ptr,r_ptr );



				ret = fn_write_data_block_uart0(SpiDateBuf,num*20); 
				if(ret < 0) 
					printf("uart send error\n");
				else
					printf("uart send num=%d\n",num*20);


			}
		}


			
		usleep(50*1000);
		//sleep(20);
	}


}

void spi_init(void)
{
	TRD_t spi_trd;
// cs0
  gpio_export(CS0_GPIO);
  gpio_direction(CS0_GPIO, 1);
  gpio_write(CS0_GPIO, 0);
	spi_dev_open();
	spi_dev_set();
	trd_create(&spi_trd, (void*)&spi_proc, NULL);
}

