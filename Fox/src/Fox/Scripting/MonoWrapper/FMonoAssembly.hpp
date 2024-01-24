#pragma once

#include <string>
#include <vector>

#include <mono/metadata/image.h>

namespace fox::mono
{
    class FMonoDomain;
    class FMonoType;

    class FMonoAssembly
    {
    public:
        explicit FMonoAssembly(const FMonoDomain& domain, const std::string& path);

        auto GetType(const std::string& name) const -> FMonoType;

        auto GetType(const std::string& name_space, const std::string& name) const -> FMonoType;

        auto DumpReferences() const -> std::vector<std::string>;

    private:
        MonoAssembly* assembly_ = nullptr;
        MonoImage* image_ = nullptr;
    };

} // namespace fox::mono