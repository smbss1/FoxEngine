#include "FMonoDomain.hpp"
#include "FMonoAssembly.hpp"

#include "FMonoString.hpp"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/threads.h>

namespace fox::mono
{
    static const FMonoDomain* current_domain = nullptr;

    auto FMonoDomain::GetInternalPtr() const -> MonoDomain*
    {
        return domain_;
    }

    void FMonoDomain::SetCurrentDomain(const FMonoDomain& domain)
    {
        current_domain = &domain;
    }

    auto FMonoDomain::GetCurrentDomain() -> const FMonoDomain&
    {
        return *current_domain;
    }

    auto FMonoDomain::NewString(const std::string& str) const -> FMonoString
    {
        return FMonoString(*this, str);
    }

    FMonoDomain::FMonoDomain(const std::string& name)
    {
        domain_ = mono_domain_create_appdomain(const_cast<char*>(name.c_str()), nullptr);

        auto res = mono_domain_set(domain_, 0);
        if(res)
        {
            mono_thread_attach(domain_);
        }
    }

    FMonoDomain::~FMonoDomain()
    {
        if(domain_)
        {
            auto root_domain = mono_get_root_domain();
            auto res = mono_domain_set(root_domain, 0);
            if(res)
            {
                mono_domain_unload(domain_);
            }
        }
        mono_gc_collect(mono_gc_max_generation());
    }

    auto FMonoDomain::GetAssembly(const std::string& path) const -> FMonoAssembly
    {
        auto it = assemblies_.find(path);
        if(it != assemblies_.end())
        {
            const auto& assembly = it->second;

            return assembly;
        }
        auto res = assemblies_.emplace(path, FMonoAssembly{*this, path});

        const auto& assembly = res.first->second;

        return assembly;
    }

} // namespace fox::mono