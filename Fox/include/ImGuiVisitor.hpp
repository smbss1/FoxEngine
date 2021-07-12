//
// Created by samuel on 12/07/2021.
//

#ifndef FOXENGINE_IMGUIVISITOR_HPP
#define FOXENGINE_IMGUIVISITOR_HPP

#include <rttr/visitor.h>

class ImGuiVisitor : public rttr::visitor
{
public:
    ImGuiVisitor() = default;

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T, typename...Base_Classes>
    void visit_type_begin(const type_info<T>& info)
    {
        using declaring_type_t = typename type_info<T>::declaring_type;
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T, typename...Ctor_Args>
    void visit_constructor(const constructor_info<T>& info)
    {
        using declaring_type_t = typename constructor_info<T>::declaring_type;
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void visit_global_method(const method_info<T>& info)
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

private:

    RTTR_ENABLE(visitor)
};

// RTTR_REGISTER_VISITOR(ImGuiVisitor);

#endif //FOXENGINE_IMGUIVISITOR_HPP
