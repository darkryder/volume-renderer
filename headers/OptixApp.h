#ifndef OPTIX_APP_H
#define OPTIX_APP_H

#include "VolumeReader.h"
#include "utils.h"

#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_stream_namespace.h>
#include <optix_world.h>
#include <optixu/optixu_math_namespace.h>
#include <sutil.h>

#define ENTRY_POINT_DEFAULT 0

class OptixApp
{
public:
    OptixApp() :
        width(256),
        height(256) {};
    OptixApp(int w, int h, char *transfer_fn_filename_) :
        width(w),
        height(h),
        transfer_fn_filename(transfer_fn_filename_) {};
    ~OptixApp();

    void initialize(VolumeData3UC &);
    inline void frame() {
        if (this->transfer_fn_dirty) {
            this->update_transfer_fn();
            this->transfer_fn_dirty = false;
        }
        context->launch(ENTRY_POINT_DEFAULT, width, height);
    }

    // hooks for display
    inline optix::Buffer getOutputBuffer() { return this->context["output_buffer"]->getBuffer(); }
    void kill();
    void update_camera(optix::float3 &, optix::float3 &, optix::float3 &, optix::float3 &);

    // hooks for transfer function change
    void transfer_fn_changed();

    // public variables
    optix::Context context;

private:
    VolumeData3UC read_volume_data;
    bool use_pbo = false;
    int width;
    int height;
    bool destroyed = false;
    char *transfer_fn_filename;
    bool transfer_fn_dirty = false;
    optix::Buffer mapped_transfer_function_buffer = 0;

    // init methods
    optix::Buffer create_output_buffer();
    optix::TextureSampler map_volume_data();
    optix::Geometry construct_top_geometry();

    void update_transfer_fn();

    // ptx programs -- filenames
    const char *CAMERA_PTX_FILENAME = "pinhole_camera";
    const char *INTERSECTION_PTX_FILENAME = "intersection";
    const char *BB_PTX_FILENAME = "bb";
    const char *MISS_PTX_FILENAME = "miss";
    const char *EXCEPTION_PTX_FILENAME = "exception";
    // ptx programs -- hooks
    void hook_intersection_program(optix::Geometry &);
    void hook_bb_program(optix::Geometry &);
    void hook_exception_program();
    void hook_miss_program();
    void hook_camera_program();

    // camera variables
    optix::float3 eye{};
    optix::float3 U{};
    optix::float3 V{};
    optix::float3 W{};
};

#endif
