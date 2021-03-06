#ifndef CST_UTILS_H
#define CST_UTILS_H

#include <string>
#include <signal.h>
#include <unistd.h>
#include <optixu/optixpp_namespace.h>
#include <sutil.h>

#define QUIT(errno) exit(errno)

#ifdef DEBUG
#define LOG(message) {std::cout << message << std::endl;}
#else
#define LOG(message)
#endif

namespace cst_utils{
    bool starts_with(const std::string& s1, const std::string& s2);

    std::string get_ptx_path(const char *program_name);
    std::string get_ptx_path(const std::string program_name);
    optix::Program get_ptx_program(optix::Context &, const std::string filename, const char *function_name);
    void createFpsText(char *, unsigned int frame_counter);
}

#endif