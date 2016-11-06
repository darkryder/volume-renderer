#include "utils.h"
#include <string>

namespace cst_utils{
    static const float FPS_UPDATE_INTERVAL = 0.5;

    bool starts_with(const std::string& s1, const std::string& s2) {
        return s2.size() <= s1.size() && s1.compare(0, s2.size(), s2) == 0;
    }

    std::string get_ptx_path(const char *filename) {
        return get_ptx_path(std::string(filename));
    }

    std::string get_ptx_path(std::string str_program_name) {
        return "bin/" + str_program_name + ".ptx";
    }

    optix::Program get_ptx_program(optix::Context &context, const std::string filename, const char *function_name) {
        std::string ptx_path = get_ptx_path(filename);
        return context->createProgramFromPTXFile(ptx_path, function_name);
    }

    void createFpsText(char *fps_text, unsigned int frame_count)
    {
        static double fps = -1.0;
        static unsigned last_frame_count = 0;
        static double last_update_time = sutil::currentTime();
        static double current_time = 0.0;
        current_time = sutil::currentTime();
        if ( current_time - last_update_time > FPS_UPDATE_INTERVAL ) {
            fps = ( frame_count - last_frame_count ) / ( current_time - last_update_time );
            last_frame_count = frame_count;
            last_update_time = current_time;
        }
        if ( frame_count > 0 && fps >= 0.0 ) {
            sprintf( fps_text, "optixVolumeMarcher | FPS: %7.2f", fps );
        }
    }
}
