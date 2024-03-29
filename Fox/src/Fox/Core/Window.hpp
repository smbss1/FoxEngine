//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_WINDOW_HPP
#define FOXENGINE_WINDOW_HPP

#include <string>
#include <functional>
//#include "glm/glm.hpp"
#include "common.hpp"
#include "Core/Input/KeyCodes.hpp"
#include "Core/Input/MouseCodes.hpp"
#include "Events/Event.hpp"
#include <glm/fwd.hpp>

namespace fox
{
    struct WindowProps
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;

        WindowProps(const std::string& title = "Fox Engine",
                    uint32_t width = 1280,
                    uint32_t height = 720)
                : Title(title), Width(width), Height(height)
        {
        }
    };

    // Interface representing a desktop system based Window
    class Window : public RefCounted
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() = default;

        virtual void OnUpdate() = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        // Window attributes

        /**
         * @brief Set the event callback for event with pooling
         * @param callback the callback
         */
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        /**
         * @brief Set the callback for key event without pooling
         * @param callback the callback
         */
        virtual bool GetKey(const KeyCode keycode) = 0;
        virtual bool IsKeyDown(const KeyCode key) = 0;

        virtual bool IsMouseButtonPressed(const Mouse button) = 0;
        virtual glm::vec2 GetMousePosition() = 0;


        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void* GetNativeWindow() const = 0;
        virtual void SetNativeWindow(void* data) = 0;

        static scope<Window> Create(const WindowProps& props);
    };
}

#endif //FOXENGINE_WINDOW_HPP
