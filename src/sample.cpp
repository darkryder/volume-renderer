#include "VolumeReader.h"

#include <iostream>

int main(int argc, char const *argv[])
{
    VolumeReader volume_reader("bonsai");
    volume_reader.print_meta();
}