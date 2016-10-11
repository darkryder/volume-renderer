CC=g++
INC_DIR = headers
CFLAGS=-Wall -Werror -I$(INC_DIR) -std=c++11
OBJS = utils.o reader.o data.o
SYMBOLS =-D DEBUG

all: $(OBJS)
	$(CC) $(CFLAGS) $(SYMBOLS) src/sample.cpp -o sample -L bin -l:reader.so  -l:utils.so -l:volumedata.so

utils.o: base
	$(CC) $(CFLAGS) $(SYMBOLS) -c -o bin/utils.so src/utils.cpp

reader.o: base data.o
	$(CC) $(CFLAGS) $(SYMBOLS) -c -o bin/reader.so src/VolumeReader.cpp

data.o: base utils.o
	$(CC) $(CFLAGS) $(SYMBOLS) -c -o bin/volumedata.so src/VolumeData.cpp

base:
	mkdir -p bin/

clean:
	rm -rf bin/*
	rm -f sample