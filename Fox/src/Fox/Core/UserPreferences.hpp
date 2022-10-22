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

    struct UserPreferences : public RefCounted
    {
        bool ShowWelcomeScreen = true;
        std::string StartupProject;
        std::map<time_t, RecentProject, std::greater<time_t>> RecentProjects;

        // Not Serialized
        std::string FilePath;
    };

    class UserPreferencesSerializer
    {
    public:
        UserPreferencesSerializer(const Ref<UserPreferences>& preferences);
        ~UserPreferencesSerializer();

        void Serialize(const std::filesystem::path& filepath);
        void Deserialize(const std::filesystem::path& filepath);

    private:
        Ref<UserPreferences> m_Preferences;
    };

}

#endif //FOX_LEXER_USERPREFERENCES_HPP
