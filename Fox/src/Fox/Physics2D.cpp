//
// Created by samuel on 30/10/22.
//

#include <algorithm>
#include "Physics2D.hpp"
#include "Core/Base.hpp"
#include "Core/Logger/Logger.hpp"

namespace fox
{
    static std::vector<PhysicLayer> Layers;
    static std::vector<const char*> LayersNames;

    void RebuildLayerNames()
    {
        std::vector<const char*> names;
        for (const auto& layer : Layers)
        {
            names.push_back(layer.Name.c_str());
        }
        LayersNames = names;
    }

    void Physics2D::Init()
    {
        Layers.clear();
        LayersNames.clear();

        Physics2D::AddLayer("Default");
        Physics2D::SetCollision(Physics2D::DefaultLayer, Physics2D::DefaultLayer, true);
    }

    uint32_t Physics2D::AddLayer(const std::string& name, bool selfCollision)
    {
        PhysicLayer layer = PhysicLayer(name, Layers.size());
        if (selfCollision)
        {
            // set collision to itself
            layer.Collisions |= layer.LayerID;
        }
        Layers.push_back(layer);
        RebuildLayerNames();
        return layer.LayerID;
    }

    void Physics2D::SetCollision(uint32_t layerId, uint32_t otherLayerId, bool shouldCollide)
    {
        PhysicLayer* layer;
        PhysicLayer* otherLayer;
        if (!GetLayer(layerId, layer) || !GetLayer(otherLayerId, otherLayer))
            return;

        if (shouldCollide)
        {
            layer->Collisions |= otherLayer->AsBit();
            otherLayer->Collisions |= layer->AsBit();
        }
        else
        {
            layer->Collisions &= ~otherLayer->AsBit();
            otherLayer->Collisions &= ~layer->AsBit();
        }
    }

    void Physics2D::RemoveLayer(uint32_t layerId)
    {
        PhysicLayer* layerInfo = nullptr;
        if (!GetLayer(layerId, layerInfo))
            return;

        for (auto& otherLayer : Layers)
        {
            if (otherLayer.LayerID == layerId)
                continue;

            if (otherLayer.Collisions & layerInfo->AsBit())
            {
                otherLayer.Collisions &= ~layerInfo->AsBit();
            }
        }

        Layers.erase(std::remove_if(Layers.begin(), Layers.end(), [layerId](PhysicLayer& l) { return l.LayerID == layerId; }), Layers.end());
        RebuildLayerNames();
    }


    void Physics2D::UpdateLayerName(uint32_t layerId, const std::string& newName)
    {
        for (const auto& layerName : LayersNames)
        {
            if (layerName == newName)
                return;
        }

        PhysicLayer* layer = nullptr;
        if (GetLayer(layerId, layer))
        {
            layer->Name = newName;

            LayersNames.erase(std::remove_if(LayersNames.begin(), LayersNames.end(), [layer](const std::string& name) { return layer->Name == name; }), LayersNames.end());
            LayersNames.insert(LayersNames.begin() + layerId, newName.c_str());
        }
    }

    bool Physics2D::GetLayer(uint32_t layerId, PhysicLayer*& result)
    {
        auto it = std::find_if(Layers.begin(), Layers.end(), [layerId](const PhysicLayer& layer){ return layer.LayerID == layerId; });
        if (it == Layers.end())
        {
            return false;
        }
        result = it.base();
        return true;
    }

    bool Physics2D::GetLayer(const std::string& name, PhysicLayer*& result)
    {
        auto it = std::find_if(Layers.begin(), Layers.end(), [&name](const PhysicLayer& layer){ return layer.Name == name; });
        if (it == Layers.end())
        {
            return false;
        }
        result = it.base();
        return true;
    }

    std::vector<PhysicLayer> Physics2D::GetCollisions(uint32_t layerId)
    {
        PhysicLayer* layer = nullptr;
        if (!GetLayer(layerId, layer))
            return {};

        std::vector<PhysicLayer> collisions;
        for (const auto& otherLayer : Layers)
        {
//            if (otherLayer.LayerID == layerId)
//                continue;

            if (layer->Collisions & otherLayer.AsBit())
                collisions.push_back(otherLayer);
        }

        return collisions;
    }

    const std::vector<PhysicLayer>& Physics2D::GetLayers()
    {
        return Layers;
    }

    uint32_t Physics2D::GetLayerCount()
    {
        return Layers.size();
    }

    const std::vector<const char*>& Physics2D::GetLayersName()
    {
        return LayersNames;
    }

    bool Physics2D::LayerIsValid(uint32_t layerId)
    {
        if (layerId >= 0 && layerId < Physics2D::GetLayers().size())
            return true;
        return false;
    }
}
