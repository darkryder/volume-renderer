#include "VolumeReader.h"
#include "DisplayManager.h"
#include "OptixApp.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    VolumeReader volume_reader("bonsai");
    VolumeData3UC volumedata = volume_reader.extract();

    int width = DEFAULT_WIDTH,
        height = DEFAULT_HEIGHT;

    LOG("Creating optix app");
    OptixApp optix_app(width, height);
    optix_app.initialize();

    LOG("Creating window");
    DisplayManager *display_manager = DisplayManagerWrapper::create_singleton();
    display_manager->initialize(optix_app, &argc, argv);
    DisplayManagerWrapper::registerCallbacksWrapper();

    LOG("Starting display loop.")
    display_manager->execute();
}