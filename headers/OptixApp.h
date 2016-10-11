#ifndef OPTIX_APP_H
#define OPTIX_APP_H

#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_stream_namespace.h>
#include <optix_world.h>
#include <optixu/optixu_math_namespace.h>
#include <sutil.h>

class OptixApp
{
public:
    OptixApp() :
        use_pbo(true),
        width(256),
        height(256),
        destroyed(false)
        {};
    ~OptixApp();

    void initialize();
    void frame();

    // hooks for display
    inline optix::Buffer getOutputBuffer() { return this->context["output_buffer"]->getBuffer(); }
    void kill();

    // public variables
    optix::Context context;

private:
    bool use_pbo;
    int width;
    int height;
    bool destroyed;

};

#endif
