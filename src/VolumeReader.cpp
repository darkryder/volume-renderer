#include "VolumeReader.h"
#include "utils.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

VolumeReader::VolumeReader(const std::string &volume_object_name) {
    this->volume_object_name = volume_object_name;
}

struct volume_meta VolumeReader::extract(unsigned char *data_buffer) {
    return this->read_meta();
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

    std::cout << "There are " << meta.dimension << " dimensions" << std::endl;
    std::cout << "Sizes in each dimension are " << meta.sizes[0] << "x" << meta.sizes[1] << "x" << meta.sizes[2] << std::endl;
}