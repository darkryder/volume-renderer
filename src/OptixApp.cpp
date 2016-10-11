#include "OptixApp.h"

#include <iostream>

void OptixApp::initialize() {
    context = optix::Context::create();

    // optix::Buffer buffer = sutil::createOutputBuffer(context, RT_FORMAT_UNSIGNED_BYTE4, width, height, use_pbo);
    // context["output_buffer"]->set( buffer );
}

void OptixApp::frame() {}

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
