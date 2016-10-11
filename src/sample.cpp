#include "VolumeReader.h"

#include <iostream>

int main(int argc, char const *argv[])
{
    VolumeReader volume_reader("bonsai");
    VolumeData3UC volumedata = volume_reader.extract();
}