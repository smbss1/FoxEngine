//
// Created by samuel on 07/10/22.
//

#ifndef FOX_LEXER_IMGUIFIELDDRAWER_HPP
#define FOX_LEXER_IMGUIFIELDDRAWER_HPP

#include "Reflection/Core/Any.hpp"
#include "imgui.h"

#include <unordered_map>
#include <functional>
#include <string>

namespace fox
{
    enum class ScriptFieldType;
    namespace Reflect
    {
        class TypeDescriptor;
        class DataMember;
        class AnyRef;
    }

    class ImGuiFieldDrawer
    {
    public:
        static bool Draw(const Reflect::TypeDescriptor* type, std::string name, Reflect::Any& outData);
        static bool DrawField(ScriptFieldType type, std::string name, Reflect::Any& outData);

        static bool DrawInt(std::string name, Reflect::Any& valueFrom);
        static bool DrawBool(std::string name, Reflect::Any& valueFrom);
        static bool DrawFloat(std::string name, Reflect::Any& outData);
        static bool DrawTexture(std::string name, Reflect::Any& outData);

        static bool DrawVec2(std::string name, Reflect::Any& outData);
        static bool DrawVec3(std::string name, Reflect::Any& outData);
        static bool DrawVec4(std::string name, Reflect::Any& outData);
        static bool DrawEntityRef(std::string name, Reflect::Any& valueFrom);

        static bool DrawRigidbody2D_BodyType(std::string name, Reflect::Any& valueFrom);

        static std::unordered_map<const Reflect::TypeDescriptor*, std::function<bool(std::string, Reflect::Any& )>> m_Drawers;
        static std::unordered_map<ScriptFieldType, std::function<bool(std::string, Reflect::Any&)>> m_FieldDrawers;
    };
}

#endif //FOX_LEXER_IMGUIFIELDDRAWER_HPP
