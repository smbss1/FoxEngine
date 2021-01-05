
#include "FileSystem.hpp"
#include "Core/World.hpp"
#include "json_struct.h"
#include "Engine.hpp"
#include "SimpleSignal.hpp"

namespace Fox
{
    namespace Save
    {
        /**
        * @brief Save scene into a file
        * @param strFile the path of the file
        * @param[out] oWorld the object who will be save
        * @return true if the load success, otherwise false + print an error
        */
        inline void SaveSceneToFile(const std::string& strFile, const World& oWorld)
        {
            std::string strData = JS::serializeStruct(oWorld);
            Fox::File::WriteFile(strFile, strData);
        }
        
        /**
        * @brief Load scene from a file to it's object
        * @param strFile the path of the file
        * @param[out] oWorld the object who will set all the loaded data
        * @return true if the load success, otherwise false + print an error
        */
        inline bool LoadSceneFromFile(const std::string& strFile, World& oWorld)
        {
            // Read file
            std::string strFileContent;
            Fox::File::ReadFile(strFile, strFileContent);

            // Set the content to be parse
            JS::ParseContext oParseContext(strFileContent);

            // If parse not success, print error and return false
            if (oParseContext.parseTo(oWorld) != JS::Error::NoError)
            {
                std::string errorStr = oParseContext.makeErrorString();
                fprintf(stderr, "Error parsing: %s\n", errorStr.c_str());
                return false;
            }

            Fox::Ecs::Event oLoadSceneEvent(Fox::Event::Engine::Scene::OnLoadScene);
	        oLoadSceneEvent.SetParam(Fox::Event::Engine::Scene::LOADED_SCENE, &oWorld);
            oWorld.SendEvent(oLoadSceneEvent);
            return true;
        }
    } // namespace Save
    
} // namespace Fox
