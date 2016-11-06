#include "DisplayManager.h"

void DisplayManager::initialize(OptixApp &optix_app, int *argc, char **argv) {
    this->app = optix_app;

    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(50, 50);
    glutCreateWindow(TITLE);
    glutHideWindow();
}


void DisplayManager::registerCallbacks(
    void (* display_callback_wrapper)(void),
    void (* idle_callback_wrapper)(void),
    void (* keyboard_callback_wrapper)(unsigned char key, int x, int y),
    void (* callback_mousepress_wrapper)(int, int, int, int),
    void (* callback_mousemotion_wrapper)(int, int),
    void (* exit_handler_wrapper)(void)
    ) {

    // register glut callbacks
    glutDisplayFunc(display_callback_wrapper);
    glutIdleFunc(idle_callback_wrapper);
    glutKeyboardFunc(keyboard_callback_wrapper);
    atexit(exit_handler_wrapper);
    // glutReshapeFunc( glutResize );
    glutMouseFunc(callback_mousepress_wrapper);
    glutMotionFunc(callback_mousemotion_wrapper);
}


void DisplayManager::execute() {
    // Initialize GL state
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glViewport(0, 0, width, height);

    glutShowWindow();
    glutReshapeWindow(width, height);

    glutMainLoop();

}

void DisplayManager::callback_keyboard(unsigned char key, int x, int y) {}

void DisplayManager::callback_mousepress(int button, int state, int x, int y) {
    if( state == GLUT_DOWN ) {
        this->mouse_button = button;
        this->mouse_prev_pos = optix::make_int2( x, y );
    }
}

void DisplayManager::callback_mousemotion(int x, int y) {
    if(this->mouse_button == GLUT_RIGHT_BUTTON) {
        const float dx = static_cast<float>(x - mouse_prev_pos.x) /
                         static_cast<float>(this->width);
        const float dy = static_cast<float>(y - mouse_prev_pos.y) /
                         static_cast<float>(height);
        const float dmax = fabsf(dx) > fabs(dy) ? dx : dy;
        const float scale = fminf(dmax, 0.9f);
        camera_eye = camera_eye + (camera_lookat - camera_eye)*scale;
    }
    else if( mouse_button == GLUT_LEFT_BUTTON ) {
        const optix::float2 from = { static_cast<float>(mouse_prev_pos.x),
                                     static_cast<float>(mouse_prev_pos.y) };
        const optix::float2 to   = { static_cast<float>(x),
                                     static_cast<float>(y) };

        const optix::float2 a = { from.x / width, from.y / height };
        const optix::float2 b = { to.x   / width, to.y   / height };

        camera_rotate = arcball.rotate( b, a );
    }
    mouse_prev_pos = optix::make_int2(x, y);
}

void DisplayManager::calc_camera_uvw() {
    const float vfov = 35.0f;
    const float aspect_ratio = static_cast<float>(width) /
                               static_cast<float>(height);

    sutil::calculateCameraVariables(
            camera_eye, camera_lookat, camera_up, vfov, aspect_ratio,
            camera_u, camera_v, camera_w, true);

    const optix::Matrix4x4 frame = optix::Matrix4x4::fromBasis(
            normalize(camera_u),
            normalize(camera_v),
            normalize(-camera_w),
            camera_lookat);
    const optix::Matrix4x4 frame_inv = frame.inverse();
    // Apply camera rotation twice to match old SDK behavior
    const optix::Matrix4x4 trans     = frame*camera_rotate*camera_rotate*frame_inv;

    camera_eye    = optix::make_float3( trans*make_float4( camera_eye,    1.0f ) );
    camera_lookat = optix::make_float3( trans*make_float4( camera_lookat, 1.0f ) );
    camera_up     = optix::make_float3( trans*make_float4( camera_up,     0.0f ) );

    sutil::calculateCameraVariables(
            camera_eye, camera_lookat, camera_up, vfov, aspect_ratio,
            camera_u, camera_v, camera_w, true );

    camera_rotate = optix::Matrix4x4::identity();
}

void DisplayManager::display_frame() {
    this->calc_camera_uvw();
    this->app.update_camera(camera_eye, camera_u, camera_v, camera_w);
    this->app.frame();
    sutil::displayBufferGL(app.getOutputBuffer());
    {
        static char fps_text[64] = {0};
        static unsigned frame_count = 0;
        cst_utils::createFpsText(fps_text, frame_count++);
        glutSetWindowTitle(fps_text);
    }
    glutSwapBuffers();
}

void DisplayManager::exit_handler() {
    this->app.kill();
}

namespace DisplayManagerWrapper {

    DisplayManager *create_singleton() { return display_manager; }

    void display_frame_wrapper() { display_manager->display_frame(); }
    void callback_keyboard_wrapper(unsigned char key, int x, int y) { display_manager->callback_keyboard(key, x, y); }
    void callback_mousepress_wrapper(int button, int state, int x, int y) { display_manager->callback_mousepress(button, state, x, y); }
    void callback_mousemotion_wrapper(int x, int y) { display_manager->callback_mousemotion(x, y); }
    void exit_handler_wrapper() { display_manager->exit_handler(); }

    void registerCallbacksWrapper() {
        display_manager->registerCallbacks(
            display_frame_wrapper,          // display
            display_frame_wrapper,          // idle
            callback_keyboard_wrapper,      // keyboard
            callback_mousepress_wrapper,    // mousepress
            callback_mousemotion_wrapper,   // mousemotion
            exit_handler_wrapper            // exit handler
        );
    }
}

DisplayManager::~DisplayManager() {
    exit_handler();
}
