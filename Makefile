#CROSS = arm-none-linux-gnueabi-
CROSS = arm-openwrt-linux-gnueabi-
GCC = $(CROSS)gcc
EXEC = app-uart
INC	=./
SRC = ./
OBJ = ./obj/
OBJS	=						\
		$(OBJ)main.o			\
		$(OBJ)thread.o			\
		$(OBJ)com.o				\
		$(OBJ)spi.o				\
		$(OBJ)gpio.o			

$(OBJ)%.o: $(SRC)%.c $(INC)*.h
	$(GCC) -Wall  -I$(INC) -g -c -o $(OBJ)$*.o $<

default:	$(OBJS)
#	$(GCC) -O0 -o $(EXEC) $(OBJS) -lpthread -lcrypt --static

	$(GCC) -O0 -o $(EXEC) $(OBJS) -lpthread -lcrypt
	cp $(EXEC) ../../../tftpboot
	sudo cp $(EXEC) /bin/
clean:
	rm -rf $(EXEC) 
	rm -rf $(OBJ)*.o
