/*
** EPITECH PROJECT, 2021
** Fox
** File description:
** SceneManager
*/

#ifndef FOX_SCENEMANAGER_HPP_
#define FOX_SCENEMANAGER_HPP_

#include <unordered_map>
#include "Scene.hpp"

namespace fox
{
    using SceneId = std::uint32_t;
    class SceneManager
    {
        public:
            SceneManager();
            ~SceneManager();

            // ProcessInput, Update, LateUpdate, and Draw will simply be
            // pass through methods. They will call the correspondingly
            // named methods of the active scene.
            void start();
            void update(Timestep dt);

            // Adds a scene to the state machine and returns the id of that scene.
            SceneId add(ref<Scene> pScene);

            // Transitions to scene with specified id.
            void switch_to(SceneId id);

            // Removes scene from state machine.
            void remove(SceneId id);

            // Get the current scene.
            ref<Scene> get_active()
            {
                return m_pCurrentScene;
            }

        protected:
        private:
            // Stores all of the scenes associated with this state machine.
            std::unordered_map<SceneId, ref<Scene>> m_vScenes;

            // Stores a reference to the current scene. Used when drawing/updating.
            ref<Scene> m_pCurrentScene;

            // Stores our current scene id. This is incremented whenever
            // a scene is added.
            SceneId insertedSceneID;
    };
} // namespace fox

#endif /* !FOX_SCENEMANAGER_HPP_ */
