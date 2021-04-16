#include "SceneManager.hpp"


fox::SceneManager::SceneManager()
    : m_pCurrentScene(), insertedSceneID(0)
{    
}

fox::SceneManager::~SceneManager()
{
}

void fox::SceneManager::update(Timestep dt)
{
    if (m_pCurrentScene)
        m_pCurrentScene->update(dt);
}

fox::SceneId fox::SceneManager::add(ref<Scene> pScene)
{
    auto new_scene = m_vScenes.emplace(insertedSceneID, pScene);

    insertedSceneID++;
    new_scene.first->second->init_systems();
    new_scene.first->second->on_create();
    return insertedSceneID - 1;
}

void fox::SceneManager::switch_to(SceneId id)
{
    auto it = m_vScenes.find(id);
    if(it != m_vScenes.end())
    {
		// If we have a current scene, we call its on_disable method.
        if(m_pCurrentScene)
            m_pCurrentScene->disable();
        
		// Setting the current scene ensures that it is updated and drawn.
        m_pCurrentScene = it->second; 
        m_pCurrentScene->enable();
    }
}

void fox::SceneManager::remove(SceneId id)
{
    auto it = m_vScenes.find(id);
    if(it != m_vScenes.end())
    {
        if(m_pCurrentScene == it->second)
        {
            // If the scene we are removing is the current scene, 
            // we also want to set that to a null pointer so the scene 
            // is no longer updated.
            m_pCurrentScene = nullptr;
        }

        // We make sure to call the on_destroy method 
        // of the scene we are removing.
        it->second->destroy_systems();
        it->second->on_destroy();
        m_vScenes.erase(it);
    }
}
