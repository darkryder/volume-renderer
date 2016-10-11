#include <string>

namespace cst_utils{
    bool starts_with(const std::string& s1, const std::string& s2) {
        return s2.size() <= s1.size() && s1.compare(0, s2.size(), s2) == 0;
    }

    std::string get_ptx_path(const char *program_name) {
        std::string str_program_name = std::string(program_name);
        return "bin/" + str_program_name + ".ptx";
    }
}
