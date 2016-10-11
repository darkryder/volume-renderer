#include "OptixApp.h"
#include "utils.h"

#include <string>

void OptixApp::initialize() {
    context = optix::Context::create();

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
    context->validate();

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
