//
// Created by samuel on 30/10/22.
//

#ifndef FOX_LEXER_PHYSICS2D_HPP
#define FOX_LEXER_PHYSICS2D_HPP

#include <vector>
#include <cstdint>
#include <string>

namespace fox
{
    struct PhysicLayer
    {
        explicit PhysicLayer(const std::string &name, uint32_t layerId)
        {
            Name = name;
            LayerID = layerId;
            BitValue = 1 << LayerID;
        }

        [[nodiscard]] int32_t AsBit() const { return BitValue; }

        std::string Name;
        uint32_t LayerID;
        int32_t BitValue;
        int32_t Collisions = 0;
    };

    class Physics2D
    {
    public:
        static inline uint32_t DefaultLayer = 0;
        static void Init();
        static uint32_t AddLayer(const std::string& name, bool selfCollision = true);
        static void RemoveLayer(uint32_t layerId);
        static void UpdateLayerName(uint32_t layerId, const std::string& newName);

        static void SetCollision(uint32_t layerId, uint32_t otherLayerId, bool shouldCollide);
        static void RemoveCollision(uint32_t layerId, uint32_t otherLayerId);

        static bool GetLayer(uint32_t layerId, PhysicLayer*& result);
        static bool GetLayer(const std::string& name, PhysicLayer*& result);
        static std::vector<PhysicLayer> GetCollisions(uint32_t layerId);

        static const std::vector<PhysicLayer>& GetLayers();
        static uint32_t GetLayerCount();
        static const std::vector<const char*>& GetLayersName();
        static bool LayerIsValid(uint32_t layerId);
    };
}

#endif //FOX_LEXER_PHYSICS2D_HPP
