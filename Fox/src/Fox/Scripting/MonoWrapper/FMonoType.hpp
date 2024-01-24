#ifndef _FOX_MONOTYPE_HPP_
#define _FOX_MONOTYPE_HPP_

#include "mono/metadata/metadata.h"
#include <mono/metadata/object.h>
#include <mono/metadata/class.h>
#include <mono/metadata/mono-gc.h>
#include <string>
#include <vector>

namespace fox::mono
{
    class FMonoAssembly;
    class FMonoMethod;
    class FMonoField;
    class FMonoProperty;
    class FMonoObject;

    class FMonoType
    {
    public:
        FMonoType();

        explicit FMonoType(MonoImage* image, const std::string& name);
        explicit FMonoType(MonoImage* image, const std::string& name_space, const std::string& name);
        explicit FMonoType(MonoClass* cls);
        explicit FMonoType(MonoType* type);

        auto Valid() const -> bool;

        auto NewInstance() const -> FMonoObject;

        auto GetMethod(const std::string& name_with_args) const -> FMonoMethod;

        auto GetMethod(const std::string& name, int argc) const -> FMonoMethod;

        auto GetField(const std::string& name) const -> FMonoField;

        auto GetProperty(const std::string& name) const -> FMonoProperty;

        auto GetFields() const -> std::vector<FMonoField>;

        auto GetProperties() const -> std::vector<FMonoProperty>;

        auto GetMethods() const -> std::vector<FMonoMethod>;

        auto HasBaseType() const -> bool;

        auto GetBaseType() const -> FMonoType;

        auto GetNestedTypes() const -> std::vector<FMonoType>;

        auto IsDerivedFrom(const FMonoType& type) const -> bool;

        auto GetNamespace() const -> const std::string&;

        auto GetName() const -> const std::string&;

        auto GetFullname() const -> const std::string&;

        auto IsValueType() const -> bool;

        auto GetRank() const -> int;

        auto GetSizeof() const -> std::uint32_t;

        auto GetAlignof() const -> std::uint32_t;

        auto GetInternalPtr() const -> MonoClass*;

    private:
        void GenerateMeta();

        MonoClass* class_ = nullptr;
        MonoType* type_ = nullptr;

        std::string namespace_;

        std::string name_;

        std::string fullname_;

        std::uint32_t sizeof_ = 0;

        std::uint32_t alignof_ = 0;

        int rank_ = 0;

        bool valuetype_ = true;
    };
}

#endif // !_FOX_MONOTYPE_HPP_