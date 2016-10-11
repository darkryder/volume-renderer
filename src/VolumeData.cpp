#include "VolumeData.h"

template <typename T>
VolumeData<T>::VolumeData(int size_x, int size_y, int size_z) {
    this->size[0] = size_x;
    this->size[1] = size_y;
    this->size[2] = size_z;

    this->x = new T[size_x];
    this->y = new T[size_y];
    this->z = new T[size_z];
}

template <typename T>
VolumeData<T>::~VolumeData(){
    delete this->x;
    delete this->y;
    delete this->z;
}