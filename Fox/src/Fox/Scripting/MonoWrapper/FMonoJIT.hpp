#ifndef _FOX_MONOJIT_HPP_
#define _FOX_MONOJIT_HPP_

#include <string>
#include <vector>
#include "Core/Base.hpp"

namespace fox::mono
{
    struct compiler_params
    {
        // Specifies the format of the output assembly
        // Can be one of: exe, winexe, library, module
        std::string output_type = "library";

        // Specifies output assembly name
        std::string output_name;

        // Specifies the input files to compile
        std::vector<std::string> files;

        // Everything below is optional

        // Imports metadata from the specified assemblies
        std::vector<std::string> references;

        // Specifies the location of referenced assemblies
        std::vector<std::string> references_locations;

        // Defines one or more conditional symbols
        std::vector<std::string> defines;
    };

    class FMonoJIT final
    {
    public:
        static bool Init(const std::string& domain, bool enable_debugging = false);
        static void SetAssembliesPath(const fs::path& path);
        static void Shutdown();

    private:
        std::string CreateCompileCommand(const compiler_params& params);
        bool Compile(const compiler_params& params);
    };
}

#endif // !_FOX_MONOJIT_HPP_