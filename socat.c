
#include "socat.h"

char pts_num0=0xff,pts_num1=0xff;



char CharToHex(char bChar)
{
    if(bChar >=0x30 && bChar <=0x39)
        bChar-=0x30;
    else if(bChar >=41 && bChar <=46) //大写字母
        bChar-=0x37;
    else if(bChar >=61 && bChar <=66) //大写字母
        bChar-=0x57;
    else
       bChar+=0xff; 
   return bChar;
}

char StringtoInt(char *data,char *str)
{
    char tmp;
    if(str[0]!='\0')
    {
        tmp = CharToHex(str[0]);
        if(tmp>9)
        {
            return 0;
        }
        *data = tmp;
    }
    if(str[1]!='\0')
    {
        tmp = CharToHex(str[1]);
        if(tmp>9)
        {
            return 0;
        }
        *data *= 10;
        *data += tmp;
    }
    if(str[2]!='\0')
    {
        tmp = CharToHex(str[2]);
        if(tmp>9)
        {
            return 0;
        }
        *data *= 10;
        *data += tmp;
    }
    return 1;
    
}

int indexOf( char *s1,char *s2,char len)
{
    char len2,n=0;
    if(len==0)
        return -1;

    if(!(len2=strlen(s2)))   //此种情况下s2不能指向空，否则strlen无法测出长度，这条语句错误
        return -1;

    if(len2 > len)
        return -1;

    while(len--)
    {
        if((*s1 == *s2) && strncmp(s1,s2,len2) == 0)    
          return n;

        s1++;
        n++;
    }

    return -1;
}


int socat_declare(void)
{

  char str_buf[1000];

  int ptr = 0;

  char *str_ptr;



  char *str = "/dev/pts/";

  int index;

  char num0_buf[5],num1_buf[5];
  int i;
  char cnt = 0;


  FILE *pf0 = fopen("socat.log", "r");
  if(pf0 != NULL) {
      printf("file exist rm\n");
      fclose(pf0);
      system("rm socat.log");

  }


	system("socat -d -d -lf socat.log pty,raw,echo=0 pty,raw,echo=0 & ");
	printf("init socat\r\n");
	sleep(2);




  FILE *pf = fopen("socat.log", "r");
  if(pf == NULL) {
      printf("open socat.log file failed!\n");
      return -1;
  }


  do{

		memset(str_buf,0,sizeof(str_buf));
	  fgets(str_buf,sizeof(str_buf),pf);

		ptr = strlen(str_buf);

	  printf("\r\nfile len = %d\n",ptr);

	  printf("\r\nfile = %s\n",str_buf);

		str_ptr = str_buf;

		index = indexOf(str_ptr,str,ptr);

		if(index != -1)
		{
			printf("index%d = %d\n",cnt,index);
			str_ptr += (index + 9);
			//pts_num0 = 0;
			i = 0;
			memset(num0_buf,0,sizeof(num0_buf));
			while(*str_ptr != '\n'){
				num0_buf[i] = *str_ptr;
				i++;
				str_ptr++;
			}

			printf("num0_buf = %s\n",num0_buf);
			if(cnt == 0){
				if(StringtoInt(&pts_num0,num0_buf)) {
					printf("pts0 num = %d\n",pts_num0);
				}				
			}
			else if(cnt == 1){
				if(StringtoInt(&pts_num1,num0_buf)) {
					printf("pts0 num = %d\n",pts_num1);
				}			
			}

			cnt ++;

		}
  }while(cnt<2);

  return 1;



}
