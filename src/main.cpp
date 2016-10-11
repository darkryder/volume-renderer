#include "VolumeReader.h"
#include "DisplayManager.h"
#include "OptixApp.h"
#include "utils.h"

#include <iostream>

int main(int argc, char *argv[])
{
    VolumeReader volume_reader("bonsai");
    VolumeData3UC volumedata = volume_reader.extract();

    LOG("Creating app");
    OptixApp optix_app;

    LOG("Creating window");
    DisplayManager *display_manager = DisplayManagerWrapper::display_manager; // static object that allows method callbacks
    display_manager->initialize(optix_app, &argc, argv);
    DisplayManagerWrapper::registerCallbacksWrapper();

    display_manager->execute();
}