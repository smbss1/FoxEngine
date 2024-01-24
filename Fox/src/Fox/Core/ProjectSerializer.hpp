//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_PROJECTSERIALIZER_HPP
#define FOXENGINE_PROJECTSERIALIZER_HPP

#include "Project.hpp"

#include <string>

namespace fox
{
    class ProjectSerializer
    {
    public:
        ProjectSerializer(Ref<Project> project);

        bool Serialize(const fs::path& filepath);
        bool Deserialize(const fs::path& filepath);
    private:
        Ref<Project> m_Project;
    };

}



#endif //FOXENGINE_PROJECTSERIALIZER_HPP
