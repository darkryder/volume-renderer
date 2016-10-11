CC=g++
OPTIX_PATH=/home/darkryder/dev/NVIDIA-OptiX-SDK-4.0.1-linux64
CUDA_PATH=/usr/local/cuda-8.0
INCS=-I headers -I $(OPTIX_PATH)/include -I $(OPTIX_PATH)/include/optixu -I $(CUDA_PATH)/include -I $(OPTIX_PATH)/include/internal
SYMBOLS=-D DEBUG
CFLAGS=-Wall -Werror -std=c++11 $(SYMBOLS)
EXTERNAL_LIBS= -L $(CUDA_PATH)/lib64 -L $(OPTIX_PATH)/lib64
LIBS=-lglut -lGLU -lGL -loptix -loptixu

all: lib display optix
	$(CC) $(CFLAGS) src/main.cpp -o 3sat -L bin $(INCS) \
		-l:reader.so  -l:utils.so -l:volumedata.so -l:displaymanager.so -l:optixapp.so \
		$(LIBS) $(EXTERNAL_LIBS)

lib: base data.o utils.o reader.o

display: optix displaymanager.o

optix: optixapp.o

optixapp.o:
	$(CC) $(CFLAGS) $(INCS) -c -o bin/optixapp.so src/OptixApp.cpp

displaymanager.o:
	$(CC) $(CFLAGS) $(INCS) -c -o bin/displaymanager.so src/DisplayManager.cpp

utils.o:
	$(CC) $(CFLAGS) $(INCS) -c -o bin/utils.so src/utils.cpp

reader.o: data.o
	$(CC) $(CFLAGS) $(INCS) -c -o bin/reader.so src/VolumeReader.cpp

data.o: utils.o
	$(CC) $(CFLAGS) $(INCS) -c -o bin/volumedata.so src/VolumeData.cpp

base:
	mkdir -p bin/

clean:
	rm -rf bin/*
	rm -f 3sat
