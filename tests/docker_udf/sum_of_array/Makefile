

CC = gcc
CFLAGS = -g -I/home/xq/Desktop/wasm-udf/pgsql/docker_udf/include -I/home/xiaqian/wasm-udf/performance/flatbuffer/include
SRC = sum_of_array.c
OBJECT = $(SRC:.c=.o)
TARGET = sum_of_array

.PHONY: flatbuffer

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) -o $@ $(OBJECT)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

flatbuffer: int_array.fbs
	../flatcc/bin/flatcc -a int_array.fbs -o ../include

clean:
	rm -f $(TARGET) $(OBJECT)
