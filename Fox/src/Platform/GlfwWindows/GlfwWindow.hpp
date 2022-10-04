//
// Created by samuel on 27/06/2021.
//

#ifndef FOXENGINE_GLFWWINDOW_HPP
#define FOXENGINE_GLFWWINDOW_HPP

#include <cstdint>
#include <Core/Window.hpp>
#include <Renderer/GraphicsContext.hpp>
#include <Core/Application.hpp>
#include "GLFW/glfw3.h"

static std::unordered_map<int, fox::KeyCode> GLFW_TO_KEYBOARD_KEYS = {
        { GLFW_KEY_A, fox::Key::A},
        { GLFW_KEY_B, fox::Key::B},
        { GLFW_KEY_C, fox::Key::C},
        { GLFW_KEY_D, fox::Key::D},
        { GLFW_KEY_E, fox::Key::E},
        { GLFW_KEY_F, fox::Key::F},
        { GLFW_KEY_G, fox::Key::G},
        { GLFW_KEY_H, fox::Key::H},
        { GLFW_KEY_I, fox::Key::I},
        { GLFW_KEY_J, fox::Key::J},
        { GLFW_KEY_K, fox::Key::K},
        { GLFW_KEY_L, fox::Key::L},
        { GLFW_KEY_M, fox::Key::M},
        { GLFW_KEY_N, fox::Key::N},
        { GLFW_KEY_O, fox::Key::O},
        { GLFW_KEY_P, fox::Key::P},
        { GLFW_KEY_Q, fox::Key::Q},
        { GLFW_KEY_R, fox::Key::R},
        { GLFW_KEY_S, fox::Key::S},
        { GLFW_KEY_T, fox::Key::T},
        { GLFW_KEY_U, fox::Key::U},
        { GLFW_KEY_V, fox::Key::V},
        { GLFW_KEY_W, fox::Key::W},
        { GLFW_KEY_X, fox::Key::X},
        { GLFW_KEY_Y, fox::Key::Y},
        { GLFW_KEY_Z, fox::Key::Z},
        {GLFW_KEY_0, fox::Key::D0},
        { GLFW_KEY_1, fox::Key::D1},
        { GLFW_KEY_2, fox::Key::D2},
        { GLFW_KEY_3, fox::Key::D3},
        { GLFW_KEY_4, fox::Key::D4},
        { GLFW_KEY_5, fox::Key::D5},
        { GLFW_KEY_6, fox::Key::D6},
        { GLFW_KEY_7, fox::Key::D7},
        { GLFW_KEY_8, fox::Key::D8},
        { GLFW_KEY_9, fox::Key::D9},
        { GLFW_KEY_KP_0, fox::Key::KP0},
        { GLFW_KEY_KP_1, fox::Key::KP1},
        { GLFW_KEY_KP_2, fox::Key::KP2},
        { GLFW_KEY_KP_3, fox::Key::KP3},
        { GLFW_KEY_KP_4, fox::Key::KP4},
        { GLFW_KEY_KP_5, fox::Key::KP5},
        { GLFW_KEY_KP_6, fox::Key::KP6},
        { GLFW_KEY_KP_7, fox::Key::KP7},
        { GLFW_KEY_KP_8, fox::Key::KP8},
        { GLFW_KEY_KP_9, fox::Key::KP9},
        { GLFW_KEY_F1, fox::Key::F1},
        { GLFW_KEY_F2, fox::Key::F2},
        { GLFW_KEY_F3, fox::Key::F3},
        { GLFW_KEY_F4, fox::Key::F4},
        { GLFW_KEY_F5, fox::Key::F5},
        { GLFW_KEY_F6, fox::Key::F6},
        { GLFW_KEY_F7, fox::Key::F7},
        { GLFW_KEY_F8, fox::Key::F8},
        { GLFW_KEY_F9, fox::Key::F9},
        { GLFW_KEY_F10, fox::Key::F10},
        { GLFW_KEY_F11, fox::Key::F11},
        { GLFW_KEY_F12, fox::Key::F12},
        { GLFW_KEY_F13, fox::Key::F13},
        { GLFW_KEY_F14, fox::Key::F14},
        { GLFW_KEY_F15, fox::Key::F15},
        { GLFW_KEY_F16, fox::Key::F16},
        { GLFW_KEY_F17, fox::Key::F17},
        { GLFW_KEY_F18, fox::Key::F18},
        { GLFW_KEY_F19, fox::Key::F19},
        { GLFW_KEY_F20, fox::Key::F20},
        { GLFW_KEY_F21, fox::Key::F21},
        { GLFW_KEY_F22, fox::Key::F22},
        { GLFW_KEY_F23, fox::Key::F23},
        { GLFW_KEY_F24, fox::Key::F24},
        { GLFW_KEY_F25, fox::Key::F25},

        { GLFW_KEY_ESCAPE, fox::Key::Escape},
        { GLFW_KEY_BACKSPACE, fox::Key::Backspace},
        { GLFW_KEY_SPACE, fox::Key::Space},
        { GLFW_KEY_LEFT, fox::Key::Left},
        { GLFW_KEY_RIGHT, fox::Key::Right},
        { GLFW_KEY_UP, fox::Key::Up},
        { GLFW_KEY_DOWN, fox::Key::Down},
        { GLFW_KEY_LEFT_SHIFT, fox::Key::LeftShift},
        { GLFW_KEY_RIGHT_SHIFT, fox::Key::RightShift},
        { GLFW_KEY_LEFT_CONTROL, fox::Key::LeftControl},
        { GLFW_KEY_RIGHT_CONTROL, fox::Key::RightControl},
        { GLFW_KEY_LEFT_ALT, fox::Key::LeftAlt},
        { GLFW_KEY_RIGHT_ALT, fox::Key::RightAlt},
        { GLFW_KEY_PAGE_UP, fox::Key::PageUp},
        { GLFW_KEY_PAGE_DOWN, fox::Key::PageDown},
        { GLFW_KEY_DELETE, fox::Key::Delete},
        { GLFW_KEY_INSERT, fox::Key::Insert},
        { GLFW_KEY_END, fox::Key::End},
        { GLFW_KEY_SEMICOLON, fox::Key::Semicolon},
        { GLFW_KEY_EQUAL, fox::Key::Equal},
        { GLFW_KEY_COMMA, fox::Key::Comma},
        { GLFW_KEY_MINUS, fox::Key::Minus},
        { GLFW_KEY_SLASH, fox::Key::Slash},
        { GLFW_KEY_PERIOD, fox::Key::Period},
        { GLFW_KEY_LEFT_BRACKET, fox::Key::LeftBracket},
        { GLFW_KEY_RIGHT_BRACKET, fox::Key::RightBracket},
        { GLFW_KEY_TAB, fox::Key::Tab},
        { GLFW_KEY_HOME, fox::Key::Home},
        { GLFW_KEY_KP_MULTIPLY, fox::Key::KPMultiply},
        { GLFW_KEY_PAUSE, fox::Key::Pause},
        { GLFW_KEY_KP_ADD, fox::Key::KPAdd},
        { GLFW_KEY_KP_DIVIDE, fox::Key::KPDivide},
        { GLFW_KEY_KP_DECIMAL, fox::Key::KPDecimal },
        { GLFW_KEY_KP_ENTER, fox::Key::KPEnter },
        { GLFW_KEY_KP_EQUAL, fox::Key::KPEqual },
        { GLFW_KEY_KP_SUBTRACT, fox::Key::KPSubtract },
        { GLFW_KEY_RIGHT_SUPER, fox::Key::RightSuper},
        { GLFW_KEY_LEFT_SUPER, fox::Key::LeftSuper},
        { GLFW_KEY_APOSTROPHE, fox::Key::Apostrophe},
        { GLFW_KEY_BACKSLASH, fox::Key::Backslash },
        { GLFW_KEY_CAPS_LOCK, fox::Key::CapsLock },
        { GLFW_KEY_SCROLL_LOCK, fox::Key::ScrollLock },
        { GLFW_KEY_NUM_LOCK, fox::Key::NumLock },
        { GLFW_KEY_WORLD_1, fox::Key::World1 },
        { GLFW_KEY_WORLD_2, fox::Key::World2 },
        { GLFW_KEY_GRAVE_ACCENT, fox::Key::GraveAccent },
        { GLFW_KEY_PRINT_SCREEN, fox::Key::PrintScreen },
        { GLFW_KEY_MENU, fox::Key::Menu },
};

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

    bool IsKeyPressed(const fox::KeyCode key) override;
    bool IsMouseButtonPressed(const fox::Mouse button) override;
    glm::vec2 GetMousePosition() override;

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
    fox::scope<fox::GraphicsContext> m_pContext;
};


#endif //FOXENGINE_GLFWWINDOW_HPP
