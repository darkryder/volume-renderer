#include "device_helpers.cuh"

rtDeclareVariable(float3,   bg_colour, , );
rtDeclareVariable(uint2,    launch_index, rtLaunchIndex, );
rtBuffer<uchar4, 2>         output_buffer;

RT_PROGRAM void miss() {
    output_buffer[launch_index] = make_colour(bg_colour);
}
