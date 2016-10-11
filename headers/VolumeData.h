#ifndef VOLUME_DATA_H
#define VOLUME_DATA_H

template <typename T>
class VolumeData {
public:
    VolumeData(int size_x, int size_y, int size_z);

    T get(int x, int y, int z) const;
    void set(int x, int y, int z, T value);

    int sizes[3];

    ~VolumeData();

private:
    // data holder;
    T *data;
};

template class VolumeData<unsigned char>;
typedef VolumeData<unsigned char> VolumeData3UC;

#endif