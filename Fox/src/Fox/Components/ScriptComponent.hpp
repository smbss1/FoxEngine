//
// Created by samuel on 04/10/22.
//

#ifndef FOXENGINE_SCRIPTCOMPONENT_HPP
#define FOXENGINE_SCRIPTCOMPONENT_HPP

#include <string>

namespace fox
{
    struct ScriptComponent
    {
    public:
        std::string ClassName;

        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent&) = default;
    };
}

#endif //FOXENGINE_SCRIPTCOMPONENT_HPP
