#ifndef VOLUME_READER_H
#define VOLUME_READER_H

#include <string>

struct volume_meta {
    std::string filename;
    // std::string type; // TODO(darkryder): Implement different handlings for different types.
    int dimension;
    int sizes[3];
};

template <typename T>
struct volume_data {
    T *x;
    T *y;
    T *z;
};
using volume_data_3UC = volume_data<unsigned char>;

class VolumeReader
{
public:
    VolumeReader(const std::string &volume_object_name);
    struct volume_meta extract(volume_data_3UC &data_buffer);
    void print_meta();

private:
    std::string volume_object_name;
    struct volume_meta read_meta();

    const std::string DATA_FOLDER = "data/NRRD/";
    const std::string META_EXTENSION = ".nhdr";
};

#endif