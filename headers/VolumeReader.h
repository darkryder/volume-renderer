#ifndef VOLUME_READER_H
#define VOLUME_READER_H

#include <string>
#include "VolumeData.h"

struct volume_meta {
    std::string filename;
    // std::string type; // TODO(darkryder): Implement different handlings for different types.
    int dimension;
    int sizes[3];
};

class VolumeReader
{
public:
    VolumeReader(const std::string &volume_object_name);
    VolumeData3UC extract();
    void print_meta();

private:
    std::string volume_object_name;
    struct volume_meta read_meta();

    const std::string DATA_FOLDER = "data/NRRD/";
    const std::string META_EXTENSION = ".nhdr";
    const std::string DATA_EXTENSION = ".raw";
};

#endif