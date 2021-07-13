//
// Created by samuel on 13/07/2021.
//

#ifndef FOXENGINE_SUPPORTEDFILEFORMAT_HPP
#define FOXENGINE_SUPPORTEDFILEFORMAT_HPP

#include <vector>

namespace fox
{
    class Texture2D;
    namespace ex
    {
        template <typename T>
        const std::vector<std::string>& GetSupportedFormats();

        template <>
        inline const std::vector<std::string>& GetSupportedFormats<Texture2D>()
        {
            static std::vector<std::string> formats = {".png", ".jpg", ".jpeg", ".tga", ".dds", ".ktx", ".pvr"};
            return formats;
        }
//
//        template <>
//        inline const std::vector<std::string>& GetSupportedFormats<mesh>()
//        {
//            static std::vector<std::string> formats = {".obj", ".fbx", ".dae", ".blend", ".3ds"};
//            return formats;
//        }
//
//        template <>
//        inline const std::vector<std::string>& get_suported_formats<audio::sound>()
//        {
//            static std::vector<std::string> formats = {".ogg", ".wav"};
//            return formats;
//        }
//
//        template <>
//        inline const std::vector<std::string>& get_suported_formats<gfx::shader>()
//        {
//            static std::vector<std::string> formats = {".sc"};
//            return formats;
//        }
//
//        template <>
//        inline const std::vector<std::string>& get_suported_formats<material>()
//        {
//            static std::vector<std::string> formats = {".mat"};
//            return formats;
//        }
//
//        template <>
//        inline const std::vector<std::string>& get_suported_formats<runtime::animation>()
//        {
//            static std::vector<std::string> formats = {".anim"};
//            return formats;
//        }
//
//        template <>
//        inline const std::vector<std::string>& get_suported_formats<prefab>()
//        {
//            static std::vector<std::string> formats = {".pfb"};
//            return formats;
//        }
//
//        template <>
//        inline const std::vector<std::string>& get_suported_formats<scene>()
//        {
//            static std::vector<std::string> formats = {".sgr"};
//            return formats;
//        }
    }
}

#endif //FOXENGINE_SUPPORTEDFILEFORMAT_HPP
