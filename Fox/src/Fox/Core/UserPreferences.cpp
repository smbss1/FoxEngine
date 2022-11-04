//
// Created by samuel on 22/10/22.
//

#include "UserPreferences.hpp"

#include "yaml-cpp/yaml.h"

#include <fstream>
#include <sstream>

namespace fox
{
    void UserPreferences::Save(const std::filesystem::path& filepath)
    {
        std::filesystem::path path = filepath;
        if (path.empty())
            path = FilePath;
        UserPreferencesSerializer serializer(*this);
        serializer.Serialize(path);
    }

    void UserPreferences::ConstructFrom(const std::filesystem::path& filepath)
    {
        UserPreferencesSerializer serializer(*this);
        serializer.Deserialize(filepath);
    }

    void UserPreferences::AddRecentProject(const std::string& name, const std::filesystem::path& filepath)
    {
        RecentProject projectEntry;
        projectEntry.Name = name;
        projectEntry.FilePath = filepath;
        projectEntry.LastOpened = time(nullptr);

        // Remove this project to the list of recent project
        for (auto it = RecentProjects.begin(); it != RecentProjects.end(); it++)
        {
            if (it->second.Name == projectEntry.Name)
            {
                RecentProjects.erase(it);
                break;
            }
        }

        // then re-add it in the top
        RecentProjects[projectEntry.LastOpened] = projectEntry;
    }

    void UserPreferences::AddRecentProject(std::map<time_t, RecentProject, std::greater<time_t>>::iterator it)
    {

        RecentProject projectEntry;
        projectEntry.Name = it->second.Name;
        projectEntry.FilePath = it->second.FilePath;
        projectEntry.LastOpened = time(nullptr);

        // Remove this project to the list of recent project
        RecentProjects.erase(it);
        // then re-add it in the top
        RecentProjects[projectEntry.LastOpened] = projectEntry;
    }

    UserPreferencesSerializer::UserPreferencesSerializer(UserPreferences& preferences)
        : m_Preferences(preferences)
    {
    }

    UserPreferencesSerializer::~UserPreferencesSerializer()
    {
    }

    void UserPreferencesSerializer::Serialize(const std::filesystem::path& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "UserPrefs" << YAML::Value;
        {
            out << YAML::BeginMap;
            out << YAML::Key << "ShowWelcomeScreen" << YAML::Value << m_Preferences.ShowWelcomeScreen;
            out << YAML::Key << "EnginePath" << YAML::Value << m_Preferences.EnginePath;

            if (!m_Preferences.StartupProject.empty())
                out << YAML::Key << "StartupProject" << YAML::Value << m_Preferences.StartupProject;

            {
                out << YAML::Key << "RecentProjects";
                out << YAML::Value << YAML::BeginSeq;
                for (const auto&[lastOpened, projectConfig] : m_Preferences.RecentProjects)
                {
                    out << YAML::BeginMap;
                    out << YAML::Key << "Name" << YAML::Value << projectConfig.Name;
                    out << YAML::Key << "ProjectPath" << YAML::Value << projectConfig.FilePath;
                    out << YAML::Key << "LastOpened" << YAML::Value << projectConfig.LastOpened;
                    out << YAML::EndMap;
                }
                out << YAML::EndSeq;
            }

            out << YAML::EndMap;
        }
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();

        m_Preferences.FilePath = filepath.string();
    }

    void UserPreferencesSerializer::Deserialize(const std::filesystem::path& filepath)
    {
        std::ifstream stream(filepath);
        FOX_CORE_ASSERT(stream);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
        if (!data["UserPrefs"])
            return;

        YAML::Node rootNode = data["UserPrefs"];
        m_Preferences.ShowWelcomeScreen = rootNode["ShowWelcomeScreen"].as<bool>();
        m_Preferences.EnginePath = rootNode["EnginePath"].as<std::string>();
        m_Preferences.StartupProject = rootNode["StartupProject"] ? rootNode["StartupProject"].as<std::string>() : "";

        for (auto recentProject : rootNode["RecentProjects"])
        {
            RecentProject entry;
            entry.Name = recentProject["Name"].as<std::string>();
            entry.FilePath = recentProject["ProjectPath"].as<std::string>();
            entry.LastOpened = recentProject["LastOpened"] ? recentProject["LastOpened"].as<time_t>() : time(NULL);
            m_Preferences.RecentProjects[entry.LastOpened] = entry;
        }

        stream.close();

        m_Preferences.FilePath = filepath.string();
    }
}
