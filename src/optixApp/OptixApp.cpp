#include "OptixApp.h"

#include <string>

#define ENTRY_POINT_DEFAULT 0

optix::Geometry OptixApp::construct_top_geometry() {
    // TODO: put volume data into a common buffer.
    // set number of primitives to x*y*z
    // TODO: create an intersection program
    // Link intersection program
    // TODO: create bounding box program
    // link bounding box program

    optix::Geometry volume_geometry = context->createGeometry();

    int *dims = this->read_volume_data.sizes;
    int n_primitives = dims[0] * dims[1] * dims[2];

    volume_geometry->setPrimitiveCount(n_primitives);

    volume_geometry->setIntersectionProgram(
        cst_utils::get_ptx_program(context, PTX_FILENAME, "check_intersection"));
    volume_geometry->setBoundingBoxProgram(
        cst_utils::get_ptx_program(context, PTX_FILENAME, "bounding_box"));

    return volume_geometry;
}

void OptixApp::initialize(VolumeData3UC &read_volume_data_) {
    this->read_volume_data = read_volume_data_;

    context = optix::Context::create();

    optix::Geometry top_geometry = this->construct_top_geometry();

    /*
    context->setRayTypeCount(2);
    context->setEntryPointCount(1);

    context["radiance_ray_type"]->setUint( 0u );
    context["shadow_ray_type"  ]->setUint( 1u );
    context["scene_epsilon"    ]->setFloat( 1.e-4f );


    optix::Buffer buffer = sutil::createOutputBuffer(context, RT_FORMAT_UNSIGNED_BYTE4, width, height, use_pbo);
    context["output_buffer"]->set( buffer );

    std::string ptx_path(cst_utils::get_ptx_path("test_ptx"));
    optix::Program miss_program = context->createProgramFromPTXFile(ptx_path, "miss");
    context->setMissProgram(0, miss_program);
    context["bg_color"]->setFloat(0.34f, 0.55f, 0.85f);

    context->setExceptionProgram(0, context->createProgramFromPTXFile(ptx_path, "exception"));
    context["bad_color"]->setFloat(0.89f, 0.22f, 0.22f);



    optix::Program raygen_program = context->createProgramFromPTXFile(ptx_path, "pinhole_camera");
    context->setRayGenerationProgram(0, raygen_program);

    context["eye"]->setFloat(optix::make_float3(0, 0, 0));
    context["U"]->setFloat(optix::make_float3(0, 0, 0));
    context["V"]->setFloat(optix::make_float3(0, 0, 0));
    context["W"]->setFloat(optix::make_float3(0, 0, 0));

    optix::GeometryGroup geometry_group = context->createGeometryGroup();
    geometry_group->setAcceleration( context->createAcceleration( "Trbvh" ) );
    context[ "top_object"   ]->set( geometry_group );
    */

    LOG("Validating context.")
    context->validate();
    LOG("Context validated.")

}

void OptixApp::frame() {
    /* context->launch(0, width, height); */
}

void OptixApp::kill() {
    if (!destroyed && context) {
        context->destroy();
        context = 0;
        destroyed = true;
    }
}

OptixApp::~OptixApp() {
    kill();
}
