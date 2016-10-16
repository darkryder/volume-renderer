#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <sutil.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "OptixApp.h"

#define DEFAULT_WIDTH 512
#define DEFAULT_HEIGHT 512

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
        void (*)(int, int, int, int),               // mousepress callback
        void (*)(void)                              // exit handler

    );

    void callback_keyboard(unsigned char key, int x, int y);
    void callback_mousepress(int, int, int, int);
    void display_frame();
    void exit_handler();

private:
    OptixApp app;

    int width = DEFAULT_WIDTH;
    int height = DEFAULT_HEIGHT;
    const char* const TITLE = "optixVolumeMarcher";

    // variables for callback states.
    int mouse_button;
    optix::int2 mouse_prev_pos;
};

namespace DisplayManagerWrapper {
     // static object that allows method callbacks
    static DisplayManager *display_manager = new DisplayManager();

    DisplayManager *create_singleton();
    void display_frame_wrapper();
    void callback_keyboard_wrapper(unsigned char key, int x, int y);
    void callback_mousepress_wrapper(int, int, int, int);
    void exit_handler_wrapper();

    void registerCallbacksWrapper();
}

#endif