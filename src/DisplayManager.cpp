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
    void (* exit_handler_wrapper)(void)
    ) {

    // register glut callbacks
    glutDisplayFunc(display_callback_wrapper);
    glutIdleFunc(idle_callback_wrapper);
    glutKeyboardFunc(keyboard_callback_wrapper);
    atexit(exit_handler_wrapper);
    // glutReshapeFunc( glutResize );
    // glutMouseFunc( glutMousePress );
    // glutMotionFunc( glutMouseMotion );
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

void DisplayManager::display_frame()
{
    this->app.frame();

    // sutil::displayBufferGL(app.getOutputBuffer());

    // {
    //   static unsigned frame_count = 0;
    //   sutil::displayFps( frame_count++ );
    // }

    glutSwapBuffers();
}

void DisplayManager::exit_handler() {
    this->app.kill();
}

namespace DisplayManagerWrapper {

    DisplayManager *create_singleton() { return display_manager; }

    void display_frame_wrapper() { display_manager->display_frame(); }
    void callback_keyboard_wrapper(unsigned char key, int x, int y) { display_manager->callback_keyboard(key, x, y); }
    void exit_handler_wrapper() { display_manager->exit_handler(); }

    void registerCallbacksWrapper() {
        display_manager->registerCallbacks(
            display_frame_wrapper,          // display
            display_frame_wrapper,          // idle
            callback_keyboard_wrapper,      // keyboard
            exit_handler_wrapper            // exit handler
        );
    }
}

DisplayManager::~DisplayManager() {
    exit_handler();
}
