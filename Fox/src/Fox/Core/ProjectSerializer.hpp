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

        void Serialize(const std::filesystem::path& filepath);
        bool Deserialize(const std::filesystem::path& filepath);
    private:
        Ref<Project> m_Project;
    };

}



#endif //FOXENGINE_PROJECTSERIALIZER_HPP
