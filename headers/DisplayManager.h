#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "OptixApp.h"
#include <sutil.h>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glew.h>
#  if defined( _WIN32 )
#    include <GL/wglew.h>
#    include <GL/freeglut.h>
#  else
#    include <GL/glut.h>
#  endif
#endif

class DisplayManager
{
public:
    DisplayManager() {};
    ~DisplayManager();

    void initialize(OptixApp &, int *, char **);
    void execute();

    void registerCallbacks(
        void (*)(void),                             // display callback
        void (*)(void),                             // idle callback
        void (*)(unsigned char key, int x, int y),  // keyboard callback
        void (*)(void)                              // exit handler

    );

    void callback_keyboard(unsigned char key, int x, int y);
    void display_frame();
    void exit_handler();

private:
    OptixApp app;

    int width = 256;
    int height = 256;
    const char* const TITLE = "optixVolumeMarcher";
};

namespace DisplayManagerWrapper {
     // static object that allows method callbacks
    static DisplayManager *display_manager = new DisplayManager();

    DisplayManager *create_singleton();
    void display_frame_wrapper();
    void callback_keyboard_wrapper(unsigned char key, int x, int y);
    void exit_handler_wrapper();

    void registerCallbacksWrapper();
}

#endif