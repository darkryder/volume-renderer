#include "VolumeReader.h"
#include "utils.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

VolumeReader::VolumeReader(const std::string &volume_object_name) {
    this->volume_object_name = volume_object_name;
}

VolumeData3UC VolumeReader::extract() {
    struct volume_meta meta = this->read_meta();

    this->print_meta();

    if (meta.dimension != 3) {
        std::cout << "Support for dimensions other than 3 not added." << std::endl;
        QUIT(1);
    }

    int dimx = meta.sizes[0];
    int dimy = meta.sizes[1];
    int dimz = meta.sizes[2];

    VolumeData3UC volume_data(dimx, dimy, dimz);

    std::string datafile_path = DATA_FOLDER + this->volume_object_name + DATA_EXTENSION;
    std::ifstream datafile(datafile_path);
    if (datafile.fail()) {
        std::cout << "Unable to open file: " << datafile_path << std::endl;
        QUIT(1);
    }

    int elem_size = sizeof(unsigned char);
    char copy_buffer;
    for(int x=0; x < dimx; x++) {
        for(int y=0; y < dimy; y++) {
            for(int z=0; z < dimz; z++) {
                datafile.readsome(&copy_buffer, elem_size);
                volume_data.set(x, y, z, copy_buffer);
            }
        }
    }

    return volume_data;
}

struct volume_meta VolumeReader::read_meta() {
    std::string metafile_path = DATA_FOLDER + this->volume_object_name + META_EXTENSION;
    std::ifstream metafile(metafile_path);

    if (metafile.fail()) {
        std::cout << "Unable to open file: " << metafile_path << std::endl;
        QUIT(1);
    }

    struct volume_meta meta = {};

    std::string line;
    while(std::getline(metafile, line)) {
        std::istringstream iss(line);
        std::string _header;
        if (cst_utils::starts_with(line, "sizes")) {
            int x, y, z;
            iss >> _header >> x >> y >> z;

            meta.sizes[0] = x; meta.sizes[1] = y; meta.sizes[2] = z;
        }
        else if (cst_utils::starts_with(line, "dimension")) {
            int x;
            iss >> _header >> x;

            meta.dimension = x;
        }
    }

    return meta;
}

void VolumeReader::print_meta() {
    struct volume_meta meta = this->read_meta();

    std::cout << "-----" << std::endl;
    std::cout << "Parsing metadata. " << std::endl;
    std::cout << "There are " << meta.dimension << " dimensions" << std::endl;
    std::cout << "Sizes in each dimension are " << meta.sizes[0] << "x" << meta.sizes[1] << "x" << meta.sizes[2] << std::endl;
    std::cout << "-----" << std::endl << std::endl;
}