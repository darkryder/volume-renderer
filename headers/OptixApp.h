#ifndef OPTIX_APP_H
#define OPTIX_APP_H

#include "VolumeReader.h"
#include "utils.h"

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
        destroyed(false) {};
    OptixApp(int w, int h) :
        use_pbo(false),
        width(w),
        height(h),
        destroyed(false) {};
    ~OptixApp();

    void initialize(VolumeData3UC &);
    void frame();

    // hooks for display
    inline optix::Buffer getOutputBuffer() { return this->context["output_buffer"]->getBuffer(); }
    void kill();

    // public variables
    optix::Context context;

private:
    VolumeData3UC read_volume_data;
    bool use_pbo;
    int width;
    int height;
    bool destroyed;
};

#endif
