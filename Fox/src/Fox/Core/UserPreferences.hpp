//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_USERPREFERENCES_HPP
#define FOXENGINE_USERPREFERENCES_HPP


#include "Logger/Logger.hpp"
#include "Project.hpp"

#include <map>

namespace fox
{
    struct RecentProject
    {
        std::string Name;
        std::string FilePath;
        time_t LastOpened;
    };

    class UserPreferencesSerializer;
    struct UserPreferences : public RefCounted
    {
        bool ShowWelcomeScreen = true;
        std::string EnginePath;
        std::string StartupProject;
        std::map<time_t, RecentProject, std::greater<time_t>> RecentProjects;

        // Not Serialized
        std::string FilePath;

        void Save(const std::filesystem::path& filepath = "");
        void ConstructFrom(const std::filesystem::path& filepath);
        void AddRecentProject(const std::string& name, const std::filesystem::path& filepath);
        void AddRecentProject(std::map<time_t, RecentProject, std::greater<time_t>>::iterator it);
    };

    class UserPreferencesSerializer
    {
    public:
        UserPreferencesSerializer(UserPreferences& preferences);
        ~UserPreferencesSerializer();

        void Serialize(const std::filesystem::path& filepath);
        void Deserialize(const std::filesystem::path& filepath);

    private:
        UserPreferences& m_Preferences;
    };

}

#endif //FOXENGINE_USERPREFERENCES_HPP
