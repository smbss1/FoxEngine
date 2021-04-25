/*
** EPITECH PROJECT, 2021
** Fox
** File description:
** Scene
*/

#ifndef FOX_SCENE_HPP_
#define FOX_SCENE_HPP_

#include "Timestep.hpp"
#include "Ecs/World.hpp"

namespace fox
{
    class Scene
    {
        public:
            Scene();
            ~Scene();

            // Called when scene initially created. Called once.
            virtual void on_create() = 0;

            // Called when game start. Called once.
            virtual void on_start() = 0;

            // Called when scene destroyed. Called at most once (if a scene
            // is not removed from the game, this will never be called).
            virtual void on_destroy() = 0;

            // Called whenever a scene is transitioned into. Can be 
            // called many times in a typical game cycle.
            virtual void on_enable();
            void enable();

            // Called whenever a transition out of a scene occurs.
	        // Can be called many times in a typical game cycle.
            virtual void on_disable();
            void disable();
    
            virtual void on_update(Timestep dt) = 0;
            void update(Timestep dt);


            void init_systems();
            void destroy_systems();

            World& get_world();

        protected:
            World m_oWorld;
        private:
    };
}
#endif /* !SCENE_HPP_ */
