CC = g++
CPPFLAGS = -g -I/home/xiaqian/wasm-udf/pgsql/docker_udf/include -I/home/xiaqian/wasm-udf/performance/flatbuffer/include -L/home/xiaqian/wasm-udf/pgsql/lib
CPPFLAGS += -L/home/xiaqian/wasm-udf/pgsql/libdocker
SRC = test1.cpp
TARGET = test1

$(TARGET) : $(SRC)
	$(CC) $(CPPFLAGS) -o $@ $^ -l:libflatcc.a -ldocker

clean:
	rm -f $(TARGET)