#include "VolumeData.h"
#include "utils.h"

template <typename T>
VolumeData<T>::VolumeData(int size_x, int size_y, int size_z) {
    this->sizes[0] = size_x;
    this->sizes[1] = size_y;
    this->sizes[2] = size_z;

    this->data = new T[size_x*size_y*size_z];
}

template <typename T>
VolumeData<T>::~VolumeData(){
    delete this->data;
}

template <typename T>
T VolumeData<T>::get(int x, int y, int z) const {
    int size_y = this->sizes[1];
    int size_z = this->sizes[2];

    return this->data[x + size_y*(y + size_z * z)];
}

template <typename T>
void VolumeData<T>::set(int x, int y, int z, T value) {
    int size_y = this->sizes[1];
    int size_z = this->sizes[2];

    this->data[x + size_y*(y + size_z * z)] = value;
}

template <typename T>
VolumeData<T> *VolumeData<T>::slice(int x, int y, int z) {
    VolumeData<T> *sliced_volume = new VolumeData<T>();
    sliced_volume->sizes[0] = x;
    sliced_volume->sizes[1] = y;
    sliced_volume->sizes[2] = z;
    sliced_volume->data = this->data;
    return sliced_volume;
}
