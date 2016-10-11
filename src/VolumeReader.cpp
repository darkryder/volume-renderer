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

    struct volume_meta volume_meta_;

    std::string line;
    while(std::getline(metafile, line)) {
        std::istringstream iss(line);
        if (starts_with(line, "sizes")) {
            int x, y, z;
            std::string _header;
            iss >> _header >> x >> y >> z;

            volume_meta_.sizes[0] = x; volume_meta_.sizes[1] = y; volume_meta_.sizes[2] = z;
        }
    }

    return volume_meta_;
}

void VolumeReader::print_meta() {
    struct volume_meta meta = this->read_meta();

    std::cout << "Dimensions are " << meta.sizes[0] << "x" << meta.sizes[1] << "x" << meta.sizes[2] << std::endl;
}