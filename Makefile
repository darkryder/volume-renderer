CC=g++
INC_DIR = headers
CFLAGS=-Wall -Werror -I$(INC_DIR) -std=c++11
SYMBOLS =-D DEBUG
LIBS = -lglut -lGLU -lGL

all: lib display optix
	$(CC) $(CFLAGS) $(SYMBOLS) src/main.cpp -o main -L bin \
		-l:reader.so  -l:utils.so -l:volumedata.so -l:displaymanager.so -l:optixapp.so \
		$(LIBS)

lib: base data.o utils.o reader.o

display: displaymanager.o

optix: optixapp.o

optixapp.o:
	$(CC) $(CFLAGS) $(SYMBOLS) -c -o bin/optixapp.so src/OptixApp.cpp

displaymanager.o:
	$(CC) $(CFLAGS) $(SYMBOLS) -c -o bin/displaymanager.so src/DisplayManager.cpp

utils.o:
	$(CC) $(CFLAGS) $(SYMBOLS) -c -o bin/utils.so src/utils.cpp

reader.o: data.o
	$(CC) $(CFLAGS) $(SYMBOLS) -c -o bin/reader.so src/VolumeReader.cpp

data.o: utils.o
	$(CC) $(CFLAGS) $(SYMBOLS) -c -o bin/volumedata.so src/VolumeData.cpp

base:
	mkdir -p bin/

clean:
	rm -rf bin/*
	rm -f main
