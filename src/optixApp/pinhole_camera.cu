#include "device_helpers.cuh"

rtDeclareVariable(float3,   eye, , );
rtDeclareVariable(float3,   U, , );
rtDeclareVariable(float3,   V, , );
rtDeclareVariable(float3,   W, , );

rtDeclareVariable(uint,     volume_width, ,);
rtDeclareVariable(uint,     volume_height, ,);
rtDeclareVariable(uint,     volume_depth, ,);

rtDeclareVariable(uint2,    launch_index, rtLaunchIndex, );
rtDeclareVariable(uint2,    launch_dim, rtLaunchDim, );

rtDeclareVariable(rtObject, top_object, ,);

rtBuffer<uchar4, 2>                 output_buffer;

RT_PROGRAM void pinhole_camera() {
    float2 d = make_float2(launch_index) / make_float2(launch_dim) * 2.f - 1.f;
    float3 ray_origin = eye;
    float3 ray_direction = optix::normalize(d.x*U + d.y*V + W);

/*#ifdef DEBUG
    float t = tex3D(
        volume_texture,
        launch_index.x/(float) volume_width,
        launch_index.y/(float) volume_height,
        (frame_idx % volume_depth)/(float) volume_depth
    );
    if (launch_index.x == 128 && launch_index.y == 128) rtPrintf("Setting t to %f for %d %d %d \n", t, launch_index.x, launch_index.y, frame_idx);
#else
    float t = tex3D(volume_texture, launch_index.x, launch_index.y, 0);
    if (launch_index.x == 64 && launch_index.y == 64) rtPrintf("Setting t to %f for %d %d %d\n", t, launch_index.x, launch_index.y, frame_idx);
#endif

*/
    optix::Ray ray = optix::make_Ray(ray_origin, ray_direction, 0, 0.f, RT_DEFAULT_MAX);
    PerRayData_radiance prd;
    prd.result = 0;

    rtTrace(top_object, ray, prd);

    float3 transferred = transfer_function(prd.result);

    // if (launch_index.x == 0 && launch_index.y == 0)
    // rtPrintf("%f\n\n", transferred);

    output_buffer[launch_index] = make_colour(
        // optix::make_float3(1.0f, 1.0f, 1.0f)
        // optix::make_float3(t/255.0f, t/255.0f, t/255.0f)
        // ray_direction*2
        transferred
    );
}
