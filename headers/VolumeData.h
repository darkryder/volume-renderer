#ifndef VOLUME_DATA_H
#define VOLUME_DATA_H

template <typename T>
class VolumeData {
public:
    VolumeData(int size_x, int size_y, int size_z);
    int sizes[3];

    // data arrays;
    T *x;
    T *y;
    T *z;

    ~VolumeData();
private:
};

typedef VolumeData<unsigned char> VolumeData3UC;

#endif