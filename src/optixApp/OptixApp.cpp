#include "OptixApp.h"
#include <string>

void OptixApp::initialize(VolumeData3UC &read_volume_data_) {
    this->read_volume_data = read_volume_data_;

    context = optix::Context::create();
    context->setEntryPointCount(1);
    context->setRayTypeCount(1);

#ifdef DEBUG
    context->setPrintEnabled(true);
    context->setPrintBufferSize(2048);
#endif

    optix::Buffer output_buffer = this->create_output_buffer();
    optix::TextureSampler mapped_volume_texture = this->map_volume_data();
    optix::Geometry top_geometry = this->construct_top_geometry();

    hook_camera_program();
    hook_bb_program(top_geometry);
    hook_intersection_program(top_geometry);
    hook_miss_program();
    hook_exception_program();

    context["stepping_distance"]->setFloat(0.3f);

    /*
    context["radiance_ray_type"]->setUint( 0u );
    context["shadow_ray_type"  ]->setUint( 1u );
    context["scene_epsilon"    ]->setFloat( 1.e-4f );

    optix::GeometryGroup geometry_group = context->createGeometryGroup();
    geometry_group->setAcceleration( context->createAcceleration( "Trbvh" ) );
    context[ "top_object"   ]->set( geometry_group );
    */

    LOG("Validating context.")
    context->validate();
    LOG("Context validated.")

}

optix::Buffer OptixApp::create_output_buffer() {
    optix::Buffer output_buffer = sutil::createOutputBuffer(context, RT_FORMAT_UNSIGNED_BYTE4, width, height, use_pbo);
    context["output_buffer"]->set(output_buffer);
    return output_buffer;
}

void OptixApp::update_camera(optix::float3 &eye, optix::float3 &U, optix::float3 &V, optix::float3 &W) {
    context["eye"]->setFloat(eye);
    context["U"]->setFloat(U);
    context["V"]->setFloat(V);
    context["W"]->setFloat(W);
}

optix::TextureSampler OptixApp::map_volume_data() {
    int *dims = this->read_volume_data.sizes;
    int width = dims[0],
        height = dims[1],
        depth = dims[2];

    optix::Buffer mapped_volume_data = context->createBuffer(
            RT_BUFFER_INPUT, // type
            RT_FORMAT_UNSIGNED_BYTE4, // format
            width,
            height,
            depth
    );

    LOG("Mapping volume data to device: " << width << "x" << height << "x" << depth)
    float *h_mapped_ptr = static_cast<float *>(mapped_volume_data->map());
    for (int z = 0; z < depth; z++) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                *h_mapped_ptr++ = (float)read_volume_data.get(x, y, z)/255.0f;
            }
        }
    }
    mapped_volume_data->unmap();

    optix::TextureSampler texture_sampler = context->createTextureSampler();
    texture_sampler->setWrapMode(0, RT_WRAP_CLAMP_TO_BORDER);
    texture_sampler->setWrapMode(1, RT_WRAP_CLAMP_TO_BORDER);
    texture_sampler->setWrapMode(2, RT_WRAP_CLAMP_TO_BORDER);
    texture_sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_NONE);
    texture_sampler->setIndexingMode(RT_TEXTURE_INDEX_ARRAY_INDEX/*RT_TEXTURE_INDEX_NORMALIZED_COORDINATES*/);
    texture_sampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
    texture_sampler->setMaxAnisotropy(1.0f);
    texture_sampler->setMipLevelCount(1);
    texture_sampler->setArraySize(1);
    texture_sampler->setBuffer(0, 0, mapped_volume_data);

    context["volume_texture"]->setTextureSampler(texture_sampler);
    context["volume_data"]->setBuffer(mapped_volume_data);

    context["volume_width"]->setUint(width);
    context["volume_height"]->setUint(height);
    context["volume_depth"]->setUint(depth);

    return texture_sampler;
}

optix::Geometry OptixApp::construct_top_geometry() {

    optix::Geometry volume_geometry = context->createGeometry();
    volume_geometry->setPrimitiveCount(1);

    optix::GeometryInstance g_inst = context->createGeometryInstance();
    g_inst->setGeometry(volume_geometry);

    optix::Material material = context->createMaterial();
    g_inst->addMaterial(material);

    optix::GeometryGroup geometry_group = context->createGeometryGroup();
    geometry_group->addChild(g_inst);
    geometry_group->setAcceleration(context->createAcceleration("Sbvh"));

    context["top_object"]->set(geometry_group);

    return volume_geometry;
}

void OptixApp::hook_exception_program() {
    context["exception_colour"]->setFloat(255.0f, 0.0f, 255.0f);
    context->setExceptionProgram(
        ENTRY_POINT_DEFAULT,
        cst_utils::get_ptx_program(context, EXCEPTION_PTX_FILENAME, "exception")
    );
}

void OptixApp::hook_miss_program() {
    context["bg_colour"]->setFloat(255.0f, 255.0f, 255.0f);
    context->setMissProgram(
        ENTRY_POINT_DEFAULT,
        cst_utils::get_ptx_program(context, MISS_PTX_FILENAME, "miss")
    );
}

void OptixApp::hook_intersection_program(optix::Geometry &geometry) {
    geometry->setIntersectionProgram(
        cst_utils::get_ptx_program(context, INTERSECTION_PTX_FILENAME, "check_intersection"));
}

void OptixApp::hook_bb_program(optix::Geometry &geometry) {
    geometry->setBoundingBoxProgram(
        cst_utils::get_ptx_program(context, BB_PTX_FILENAME, "bounding_box"));
}

void OptixApp::hook_camera_program() {
    context->setRayGenerationProgram(
        ENTRY_POINT_DEFAULT,
        cst_utils::get_ptx_program(context, CAMERA_PTX_FILENAME, "pinhole_camera")
    );
}

void OptixApp::transfer_fn_changed() {
    LOG("Caught sigusr1")
}

void OptixApp::kill() {
    if (!destroyed && context) {
        LOG("Destroying context");
        context->destroy();
        context = 0;
        destroyed = true;
        LOG("Destroyed context");
    }
}

OptixApp::~OptixApp() {
    kill();
}
