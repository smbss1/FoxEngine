#pragma once

#include "FMonoAssembly.hpp"

#include <mono/metadata/metadata.h>

#include <unordered_map>
#include <string>

namespace fox::mono
{
    class FMonoString;

    class FMonoDomain final
    {
    public:
        explicit FMonoDomain(const std::string& name);

        ~FMonoDomain();

        FMonoAssembly GetAssembly(const std::string& path) const;

        auto NewString(const std::string& str) const -> FMonoString;

        static void SetCurrentDomain(const FMonoDomain& domain);

        static const FMonoDomain& GetCurrentDomain();

        auto GetInternalPtr() const -> MonoDomain*;

    private:
        mutable std::unordered_map<std::string, FMonoAssembly> assemblies_;
        MonoDomain* domain_ = nullptr;
    };

} // namespace fox::mono