#include "device_helpers.cuh"

rtDeclareVariable(float3,   eye, , );
rtDeclareVariable(float3,   U, , );
rtDeclareVariable(float3,   V, , );
rtDeclareVariable(float3,   W, , );
rtDeclareVariable(rtObject, top_object, , );

rtDeclareVariable(uint2,    launch_index, rtLaunchIndex, );
rtDeclareVariable(uint2,    launch_dim, rtLaunchDim, );

rtBuffer<uchar4, 2>         output_buffer;

struct PerRayData_radiance
{
  float3 result;
  float  importance;
  int    depth;
};

RT_PROGRAM void pinhole_camera() {
    float2 d = make_float2(launch_index) / make_float2(launch_dim) * 2.f - 1.f;
    float3 ray_origin = eye;
    float3 ray_direction = optix::normalize(d.x*U + d.y*V + W);

    optix::Ray ray = optix::make_Ray(ray_origin, ray_direction, 0, 0.f, RT_DEFAULT_MAX);

    PerRayData_radiance prd;
    prd.importance = 1.f;
    prd.depth = 0;

    rtTrace(top_object, ray, prd);

    output_buffer[launch_index] = make_colour(
        // optix::make_float3(1.0f, 1.0f, 1.0f)
        ray_direction*2
    );
}
