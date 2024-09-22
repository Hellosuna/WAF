

CC = g++
CPPFLAGS = -g -I/home/xq/Desktop/wasm-udf/pgsql/docker_udf/include -I/home/xiaqian/wasm-udf/performance/flatbuffer/include -L/home/xiaqian/wasm-udf/performance/flatbuffer/lib -lflatcc
SRC = count.cpp
OBJECT = $(SRC:.cpp=.o)
TARGET = count

.PHONY: flatbuffer

$(TARGET): $(OBJECT)
	$(CC) $(CPPFLAGS) -o $@ $(OBJECT)

%.o : %.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $<

flatbuffer: long_array.fbs pair_array.fbs 
	/home/xiaqian/wasm-udf/performance/flatbuffer/bin/flatcc  long_array.fbs -o ../include
	/home/xiaqian/wasm-udf/performance/flatbuffer/bin/flatcc  pair_array.fbs -o ../include

clean:
	rm -f $(TARGET) $(OBJECT)
