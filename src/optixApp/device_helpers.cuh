#ifndef DEVICE_HELPERS_H
#define DEVICE_HELPERS_H

#include <optixu/optixu_math_namespace.h>
#include <optix_world.h>

#define RAY_MISSED_BB RT_EXCEPTION_USER + 0

struct PerRayData_radiance
{
  float r;
  float g;
  float b;
  float alpha;
};

// Convert a float3 in [0,1)^3 to a uchar4 in [0,255]^4 -- 4th channel is set to 255
// Reference: SDK examples
#ifdef __CUDACC__
static __device__ __inline__ optix::uchar4 make_colour(const optix::float3& c)
{
    return optix::make_uchar4( static_cast<unsigned char>(__saturatef(c.z)*255.99f),  /* B */
                               static_cast<unsigned char>(__saturatef(c.y)*255.99f),  /* G */
                               static_cast<unsigned char>(__saturatef(c.x)*255.99f),  /* R */
                               255u);                                                 /* A */
}
#endif

#endif