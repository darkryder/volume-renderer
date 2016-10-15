#include "device_helpers.cuh"

rtDeclareVariable(float3,   exception_colour, , );
rtDeclareVariable(uint2,    launch_index, rtLaunchIndex, );
rtBuffer<uchar4, 2>         output_buffer;


RT_PROGRAM void exception() {
    rtPrintf("Exception at %d\n", launch_index);
    output_buffer[launch_index] = make_colour(exception_colour);
}
