//
// Created by samuel on 12/07/2021.
//

#ifndef FOXENGINE_IMGUIVISITOR_HPP
#define FOXENGINE_IMGUIVISITOR_HPP

#include <rttr/visitor.h>
#include <Core/Logger/Logger.hpp>
#include "ImGuiExtension.hpp"

class ImGuiVisitor : public rttr::visitor
{
public:
    ImGuiVisitor(rttr::instance& obj) : m_obj(obj) { }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T, typename...Base_Classes>
    void visit_type_begin(const type_info<T>& info)
    {
    }

    template<typename T, typename...Base_Classes>
    void visit_type_end(const type_info<T>& info)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T, typename...Ctor_Args>
    void visit_constructor(const constructor_info<T>& info)
    {
    }

    template<typename T>
    void visit_constructor_function(const constructor_function_info<T>& info)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void visit_global_method(const method_info<T>& info)
    {
    }

    template<typename T>
    void visit_global_property(const property_info<T>& info)
    {
    }

    template<typename T>
    void visit_global_getter_setter_property(const property_getter_setter_info<T>& info)
    {
    }

    template<typename T>
    void visit_global_readonly_property(const property_info<T>& info)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void visit_method(const method_info<T>& info)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void visit_property(const property_info<T>& info)
    {
        using declaring_type_t = typename property_info<T>::declaring_type;

        fox::info("Property Name: %", typeid(declaring_type_t).name());

        if (info.property_item.get_type().template is_derived_from<glm::vec3>())
            fox::imgui::DrawVec3Control("dvdv", info.property_item.get_value(m_obj).template get_value<glm::vec3>());
    }

    template<typename T>
    void visit_getter_setter_property(const property_getter_setter_info<T>& info)
    {
    }

    template<typename T>
    void visit_readonly_property(const property_info<T>& info)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////

private:
    template<typename T>
    static std::string get_type_name()
    {
        return rttr::type::template get<T>().get_name().to_string();
    }

    rttr::instance& m_obj;

private:

    RTTR_ENABLE(visitor)
};

RTTR_REGISTER_VISITOR(ImGuiVisitor);

#endif //FOXENGINE_IMGUIVISITOR_HPP
