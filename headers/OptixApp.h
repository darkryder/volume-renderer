#ifndef OPTIX_APP_H
#define OPTIX_APP_H

#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_stream_namespace.h>
#include <optix_world.h>

class OptixApp
{
public:
    OptixApp() {};
    void initialize();
    void frame();
};

#endif
