#include "device_helpers.cuh"
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(PerRayData_radiance, prd, rtPayload, );
rtDeclareVariable(uint2,    launch_index, rtLaunchIndex, );

rtDeclareVariable(uint, volume_width, ,);
rtDeclareVariable(uint, volume_height, ,);
rtDeclareVariable(uint, volume_depth, ,);

rtDeclareVariable(float, stepping_distance, ,);

rtTextureSampler<float, 3>  volume_texture;
rtTextureSampler<float4, 1> transfer_fn_texture;

#define MAX_STEPS 50000
#define kassert( X ) if ( !(X) ) {\
 return ;}

#define EPS 5.0f


static __device__ bool get_aabb_ray_intersection(float &tmin, float &tmax) {
    optix::float3 orig = ray.origin;
    optix::float3 dir = ray.direction;
    optix::float3 invdir = 1/dir;
    int sign[3];
    sign[0] = (invdir.x < 0);
    sign[1] = (invdir.y < 0);
    sign[2] = (invdir.z < 0);

    float tymin, tymax, tzmin, tzmax;

    optix::float3 bounds[2] = {
        optix::make_float3(0, 0, 0),
        optix::make_float3(volume_width, volume_height, volume_depth)
    };

    tmin = (bounds[sign[0]].x - orig.x) * invdir.x;
    tmax = (bounds[1-sign[0]].x - orig.x) * invdir.x;
    tymin = (bounds[sign[1]].y - orig.y) * invdir.y;
    tymax = (bounds[1-sign[1]].y - orig.y) * invdir.y;

    if ((tmin > tymax) || (tymin > tmax))
        return false;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    tzmin = (bounds[sign[2]].z - orig.z) * invdir.z;
    tzmax = (bounds[1-sign[2]].z - orig.z) * invdir.z;

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    return true;
}

RT_PROGRAM void check_intersection(int prim_index /*There's always 1 primitive*/) {
    float tmin = 0.f, tmax = 0.f;
    bool intersected = get_aabb_ray_intersection(tmin, tmax);
    if (!intersected) {
        // rtPrintf("Nope ");
        rtThrow(RAY_MISSED_BB);
        return;
    }
    if ((tmax - tmin) < EPS) {
        return;
    }

    float n_steps = min((tmax - tmin)/stepping_distance, (float)MAX_STEPS);

    prd.r = prd.g = prd.b = 0.2;

    for(float curr_t = tmin, steps = 0; curr_t < tmax && steps < n_steps; curr_t += stepping_distance, steps++) {
        float3 point = ray.origin + curr_t*ray.direction;
        {
            int isovalue = (int) (tex3D(
                volume_texture,
                (int)(point.x + .5f),/// (float) volume_width),
                (int)(point.y + .5f),/// (float) volume_height),
                (int)(point.z + .5f )/// (float) volume_depth)
            ) * 255.99f);

            optix::float4 color = tex1D(transfer_fn_texture, isovalue) / 255.99f;
            prd.r += (1 - prd.alpha) * optix::getByIndex(color, 0)/n_steps;
            prd.g += (1 - prd.alpha) * optix::getByIndex(color, 1)/n_steps;
            prd.b += (1 - prd.alpha) * optix::getByIndex(color, 2)/n_steps;
            prd.alpha += (1 - prd.alpha) * optix::getByIndex(color, 3)/n_steps;
            if (prd.alpha > 1) {
                // break;
            }
        }
    }
}