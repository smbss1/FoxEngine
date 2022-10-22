//
// Created by samuel on 25/06/2021.
//

#ifndef FOXENGINE_TEXTURE_HPP
#define FOXENGINE_TEXTURE_HPP

#include "common.hpp"
#include "Asset/Asset.hpp"

#include <cstdint>
#include <string>

namespace fox
{
    class Texture : public Asset
    {
    public:
        virtual ~Texture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetRendererID() const = 0;
        virtual bool IsLoaded() const = 0;

        virtual void SetData(void* data, uint32_t size) = 0;
        virtual void Bind(uint32_t slot = 0) const = 0;

        virtual bool operator==(const Texture& other) const = 0;

        virtual const std::string& GetId() const = 0;
    };

    class Texture2D : public Texture
    {
    public:
        static Ref<Texture2D> Create(uint32_t width, uint32_t height);
        static Ref<Texture2D> Create(const std::string& path);
    };
}


#endif //FOXENGINE_TEXTURE_HPP
