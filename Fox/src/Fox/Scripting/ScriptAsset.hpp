//
// Created by samuel on 12/11/22.
//

#ifndef FOX_LEXER_SCRIPTASSET_HPP
#define FOX_LEXER_SCRIPTASSET_HPP

#include "Asset/Asset.hpp"

namespace fox
{
    class ScriptAsset : public Asset
    {
    public:
        ScriptAsset() = default;
        ScriptAsset(uint32_t classID)
            : m_ClassID(classID) {}

        uint32_t GetClassID() const { return m_ClassID; }

        static AssetType GetStaticType() { return AssetType::Script; }
        virtual AssetType GetAssetType() const override { return GetStaticType(); }

    private:
        uint32_t m_ClassID = 0;
    };

//    class ScriptFileAsset : public Asset
//    {
//    public:
//        ScriptFileAsset() = default;
//        ScriptFileAsset(const char* classNamespace, const char* className)
//            : m_ClassNamespace(classNamespace), m_ClassName(className)
//        {
//        }
//
//        const std::string& GetClassNamespace() const { return m_ClassNamespace; }
//        const std::string& GetClassName() const { return m_ClassName; }
//
//        static AssetType GetStaticType() { return AssetType::ScriptFile; }
//        virtual AssetType GetAssetType() const override { return GetStaticType(); }
//
//    private:
//        std::string m_ClassNamespace = "";
//        std::string m_ClassName = "";
//    };
}

#endif //FOX_LEXER_SCRIPTASSET_HPP
