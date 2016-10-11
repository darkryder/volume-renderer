CC=g++
OPTIX_PATH=/home/darkryder/dev/NVIDIA-OptiX-SDK-4.0.1-linux64
CUDA_PATH=/usr/local/cuda-8.0
INCS=-I headers -I $(OPTIX_PATH)/include -I $(OPTIX_PATH)/include/optixu -I $(CUDA_PATH)/include \
	 -I $(OPTIX_PATH)/include/internal -I $(OPTIX_PATH)/SDK/sutil
SYMBOLS=-D DEBUG
CFLAGS=-Wall -Werror -std=c++11 $(SYMBOLS)
EXTERNAL_LIBS= -L $(CUDA_PATH)/lib64 -L $(OPTIX_PATH)/lib64 -L $(OPTIX_PATH)/SDK/build/lib
LIBS=-lglut -lGLU -lGL -loptix -loptixu -lsutil_sdk

all: lib display optix
	$(CC) $(CFLAGS) src/main.cpp -o 3sat \
		$(INCS) \
		-lcst_reader  -lcst_utils -lcst_volumedata -lcst_displaymanager -lcst_optixapp \
		-L bin $(LIBS) $(EXTERNAL_LIBS)

lib: base data.o utils.o reader.o

display: optix displaymanager.o

optix: optixapp.o

optixapp.o:
	$(CC) $(CFLAGS) $(INCS) -c -o bin/libcst_optixapp.so src/OptixApp.cpp

displaymanager.o:
	$(CC) $(CFLAGS) $(INCS) -c -o bin/libcst_displaymanager.so src/DisplayManager.cpp

utils.o:
	$(CC) $(CFLAGS) $(INCS) -c -o bin/libcst_utils.so src/utils.cpp

reader.o: data.o
	$(CC) $(CFLAGS) $(INCS) -c -o bin/libcst_reader.so src/VolumeReader.cpp

data.o: utils.o
	$(CC) $(CFLAGS) $(INCS) -c -o bin/libcst_volumedata.so src/VolumeData.cpp

base:
	mkdir -p bin/

clean:
	rm -rf bin/*
	rm -f 3sat
