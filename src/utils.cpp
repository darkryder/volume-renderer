#include "utils.h"
#include <string>

namespace cst_utils{
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
}
