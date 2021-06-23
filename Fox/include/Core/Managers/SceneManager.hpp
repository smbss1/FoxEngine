/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** SceneManager.hpp
*/

#ifndef FOX_SCENEMANAGER_HPP_
#define FOX_SCENEMANAGER_HPP_

namespace fox
{
    class Application;
}

#include <unordered_map>
#include "Core/Scene.hpp"

namespace fox
{
    using SceneId = std::uint32_t;
    class SceneManager
    {
        enum class CommandType {
            ADD,
            REMOVE,
            POP,
            SWITCH,
            QUIT,
        };

        struct Command {
            CommandType type;
            ref<Scene> state;
            std::string scene_name;
        };

    public:
        SceneManager(Application& app);
        ~SceneManager();

        // ProcessInput, Update, LateUpdate, and Draw will simply be
        // pass through methods. They will call the correspondingly
        // named methods of the active scene.
        void update();
        void fix_update();
        void draw();

        /**
         * @brief Add a new scene
         * @tparam T the class of the scene
         * @tparam Args the constructor arguments types
         * @param args the constructor arguments
         */
        template<typename T, typename... Args,
                std::enable_if_t<std::is_base_of<Scene, T>::value, bool> = true>
        T& add(Args&&... args)
        {
            auto new_scene = new_ref<T>(std::forward<Args>(args)...);
            new_scene->set_application(m_oApp);

            auto& scene_ref = *new_scene;

            queue({
                  CommandType::ADD,
                  std::move(new_scene),
              });
            return scene_ref;
        }

        /**
         * @brief Find the scene matching with name given in parameter
         * @param strName the name of the scene to find
         * @return the id the scene
         */
        SceneId find(const std::string& strName);

        /**
         * @brief Transitions to scene with specified id
         * @param id the id of the scene to transit
         */
        void switch_to(SceneId id);

        /**
         * @brief Transitions to scene with specified name
         * @param name the name of the scene to transit
         */
        void switch_to(const std::string& name);

        /**
         * @brief Remove a scene
         * @param name the name of the scene to be removed
         */
        void remove(const std::string& strName);

        /**
         * @brief Do all the task in the queue
         */
        void perform(Command, Application&);

        /**
         * @brief Add a new command to the queue
         */
        void queue(Command);

        /**
         * @brief Get the current scene
         * @return the current scene
         */
        Scene* get_active()
        {
            return m_pCurrentScene;
        }

    protected:
    private:

        std::queue<Command> m_vCommandQueue;

        // Stores all of the scenes associated with this state machine.
        std::unordered_map<SceneId, ref<Scene>> m_vScenes;

        // Stores a reference to the current scene. Used when drawing/updating.
        Scene* m_pCurrentScene;

        // Stores our current scene id. This is incremented whenever
        // a scene is added.
        SceneId insertedSceneID;

        fox::Application& m_oApp;
    };
} // namespace fox

#endif /* !FOX_SCENEMANAGER_HPP_ */