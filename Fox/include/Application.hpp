
#ifndef FOX_APP_HPP_
#define FOX_APP_HPP_

#include "FoxEcs.hpp"
#include "SceneManager.hpp"
#include "Core.hpp"

namespace fox
{
    class FOX_API Application
    {
    public:
        Application(int ac, char** av);
        virtual ~Application();

        SceneId add_scene(ref<Scene> pScene);
        void switch_scene(SceneId id);
        void remove_scene(SceneId id);
        void run();

    private:
        bool m_bIsRunning;

    protected:
        scope<SceneManager> m_pSceneManager;
    };
}

#endif