#include "device_helpers.cuh"

rtBuffer<float, 3>         volume_data;
rtDeclareVariable(uint,    volume_width, , );
rtDeclareVariable(uint,    volume_height, , );

// http://math.stackexchange.com/a/19780
RT_PROGRAM void bounding_box(int prim_idx, float result[6]) {
    optix::Aabb *aabb = (optix::Aabb *) result;
    uint voxel_k = prim_idx / (volume_width*volume_height);
    uint voxel_j = (prim_idx - voxel_k*volume_width*volume_height) / volume_width;
    uint voxel_i = prim_idx - voxel_k*volume_width*volume_height - voxel_j*volume_width;

    aabb->m_min = optix::make_float3(voxel_i, voxel_j, voxel_k);
    aabb->m_max = optix::make_float3(voxel_i + 1, voxel_j + 1, voxel_k + 1);
}
