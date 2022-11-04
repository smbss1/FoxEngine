//
// Created by samuel on 09/10/22.
//

#ifndef FOXENGINE_PROPERTYPANEL_HPP
#define FOXENGINE_PROPERTYPANEL_HPP

#include "Scene/Scene.hpp"
#include "Ecs/Entity.hpp"
#include "Components/NativeScript.hpp"
#include "Panel.hpp"

namespace fox
{
    class OnSelectedEntityChangeEvent;
    class OnContextChangeEvent;

    class PropertyPanel : public Panel
    {
    public:
        explicit PropertyPanel();
        explicit PropertyPanel(const Ref<Scene>& context);
        ~PropertyPanel();

        void SetContext(const Ref<Scene>& context);
        void OnImGui(bool& isOpen) override;

    private:

        void OnContextChangeChange(const OnContextChangeEvent& event);
        void OnSelectedEntityChange(const OnSelectedEntityChangeEvent& event);
        void DrawComponents(Entity entity);

        template<typename T>
        void DisplayAddComponentEntry(const std::string& entryName);

    private:
        Ref<Scene> m_pContext = nullptr;
        Entity m_SelectedEntity;
    };
}

#endif //FOX_LEXER_PROPERTYPANEL_HPP
