//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_GLFWWINDOW_HPP
#define FOXENGINE_GLFWWINDOW_HPP

#include <cstdint>
#include <Core/Window.hpp>
#include "GLFW/glfw3.h"

class GlfwWindow : public fox::Window
{
public:
    explicit GlfwWindow(const fox::WindowProps& props);
    ~GlfwWindow() override;

    void OnUpdate() override;

    [[nodiscard]] uint32_t GetWidth() const override;
    [[nodiscard]] uint32_t GetHeight() const override;

    // Window attributes
    void SetEventCallback(const EventCallbackFn& callback) override;
    void SetVSync(bool enabled) override;
    [[nodiscard]] bool IsVSync() const override;

    [[nodiscard]] void* GetNativeWindow() const override;
    void SetNativeWindow(void* data) override;

private:

    GLFWwindow* m_pWindow = nullptr;
    struct WindowData
    {
        std::string Title;
        uint32_t width, height;
        bool bIsVSync;
        EventCallbackFn EventCallback;
    };

    WindowData m_oData;
};


#endif //FOXENGINE_GLFWWINDOW_HPP
