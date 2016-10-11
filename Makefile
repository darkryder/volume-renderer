CC=g++
INC_DIR = headers
CFLAGS=-Wall -Werror -I$(INC_DIR) -std=c++11
OBJS = utils.o reader.o

all: sample

sample: $(OBJS)
	$(CC) $(CFLAGS) src/sample.cpp -o sample -L bin -l:reader.so  -l:utils.so

utils.o: base
	$(CC) $(CFLAGS) -c -o bin/utils.so src/utils.cpp

reader.o: base
	$(CC) $(CFLAGS) -c -o bin/reader.so src/VolumeReader.cpp

base:
	mkdir -p bin/

clean:
	rm -rf bin/*
	rm -f sample