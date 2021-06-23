/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** SceneManager.cpp
*/

#include "Core/Managers/SceneManager.hpp"

fox::SceneManager::SceneManager(Application& app)
        : m_pCurrentScene(), insertedSceneID(0), m_oApp(app)
{
}

fox::SceneManager::~SceneManager()
{
}

void fox::SceneManager::update()
{
    while (!m_vCommandQueue.empty()) {
        perform(m_vCommandQueue.front(), m_oApp);
        m_vCommandQueue.pop();
    }
    if (m_pCurrentScene)
        m_pCurrentScene->update(m_oApp);
}

void fox::SceneManager::draw()
{
    if (m_pCurrentScene)
        m_pCurrentScene->draw();
}

void fox::SceneManager::switch_to(SceneId id)
{
    auto it = m_vScenes.find(id);
    if(it != m_vScenes.end())
    {
        // If we have a current scene, we call its on_disable method.
        if(m_pCurrentScene) {
            m_pCurrentScene->disable(m_oApp);
            m_pCurrentScene->get_world().clear_entities();
        }

        // Setting the current scene ensures that it is updated and drawn.
        m_pCurrentScene = it->second.get();
        m_pCurrentScene->enable(m_oApp);
    }
}

void fox::SceneManager::switch_to(const std::string& name)
{
    queue({
              CommandType::SWITCH,
              nullptr,
              name,
          });
}

fox::SceneId fox::SceneManager::find(const std::string& strName)
{
    for (auto it : m_vScenes)
    {
        if (it.second->name() == strName)
        {
            return it.first;
        }
    }
}

void fox::SceneManager::remove(const std::string& strName)
{
    queue({
          CommandType::REMOVE,
          nullptr,
          strName,
      });
}

void fox::SceneManager::fix_update()
{
    if (m_pCurrentScene)
        m_pCurrentScene->fix_update();
}

void fox::SceneManager::perform(Command cmd, Application& app)
{
    switch (cmd.type)
    {
//        case SceneManager::CommandType::POP:
//            if (!m_vScenes.empty()) {
//                m_pCurrentScene->on_disable(app);
//                m_vScenes.erase(m_pCurrentScene.);
//            }
//
//            if (!m_states.empty()) {
//                m_states.back()->on_resume(app);
//            }
//            break;
        case SceneManager::CommandType::ADD:
            cmd.state->init_systems();
            cmd.state->on_create(m_oApp);
            m_vScenes.emplace(insertedSceneID, cmd.state);
            m_vScenes[insertedSceneID]->on_create(app);
            insertedSceneID++;
            break;
        case SceneManager::CommandType::QUIT:
            for (auto& scene : m_vScenes) {
                scene.second->on_disable(app);
            }

            m_vScenes.clear();
            break;
        case SceneManager::CommandType::SWITCH:
            for (auto it : m_vScenes)
            {
                if (it.second->name() == cmd.scene_name)
                {
                    // If we have a current scene, we call its on_disable method.
                    if(m_pCurrentScene) {
                        m_pCurrentScene->disable(m_oApp);
                    }

                    // Setting the current scene ensures that it is updated and drawn.
                    m_pCurrentScene = it.second.get();
                    m_pCurrentScene->enable(app);
                }
            }
            break;

        case SceneManager::CommandType::REMOVE:
        {
            auto it = m_vScenes.find(find(cmd.scene_name));
            if (it != m_vScenes.end()) {
                if (m_pCurrentScene == it->second.get()) {
                    // If the scene we are removing is the current scene,
                    // we also want to set that to a null pointer so the scene
                    // is no longer updated.
                    m_pCurrentScene = nullptr;
                }

                // We make sure to call the on_destroy method
                // of the scene we are removing.
                it->second->on_destroy(m_oApp);
                m_vScenes.erase(it);
            }
            break;
        }
        default:
            break;
    }
}

void fox::SceneManager::queue(Command cmd)
{
    m_vCommandQueue.push(std::move(cmd));
}

// fox::ref<fox::Scene> fox::SceneManager::get_active()
// {
//     return m_pCurrentScene;
// }