export CC=arm-openwrt-linux-gnueabi-gcc
rf-transfer:
TARGET=rf-transfer
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
SOURCES = $(wildcard *.c *.cpp)
OBJS = $(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(SOURCES)))
$(TARGET) : $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)	-lpthread
	rm -rf *.o
clean:
	rm -rf *.o