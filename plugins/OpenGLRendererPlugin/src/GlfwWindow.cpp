//
// Created by samuel on 27/06/2021.
//

#include <Core/Logger/Logger.hpp>
#include "glad/glad.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../include/GlfwWindow.hpp"
#include "Events/ApplicationEvent.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"

static uint8_t s_GLFWWindowCount = 0;

static void GLFWErrorCallback(int error, const char* description)
{
    throw std::runtime_error("GLFW Error (" + std::to_string(error) + "): " + description);
}

extern "C" void* libGetProcAddress(const char *name)
{
    return (void*) glfwGetProcAddress(name);
}

GlfwWindow::GlfwWindow(const fox::WindowProps& props)
{
    m_oData.Title = props.Title;
    m_oData.width = props.Width;
    m_oData.height = props.Height;

    fox::info("Creating window % (%, %)", props.Title, props.Width, props.Height);

    if (s_GLFWWindowCount == 0)
    {
        if (!glfwInit())
            throw std::runtime_error("Could not initialize GLFW!");
        glfwSetErrorCallback(GLFWErrorCallback);
    }

    m_pWindow = glfwCreateWindow((int)m_oData.width, (int)m_oData.height, m_oData.Title.c_str(), nullptr, nullptr);
    ++s_GLFWWindowCount;

    m_pContext = fox::GraphicsContext::Create(m_pWindow);
    m_pContext->Init();

    glfwSetWindowUserPointer(m_pWindow, &m_oData);
    SetVSync(true);

    // Set GLFW callbacks
    glfwSetWindowSizeCallback(m_pWindow, [](GLFWwindow* window, int width, int height)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.width = width;
        data.height = height;

        fox::WindowResizeEvent event(width, height);
        data.EventCallback(event);
    });

    glfwSetWindowCloseCallback(m_pWindow, [](GLFWwindow* window)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        fox::WindowCloseEvent event;
        data.EventCallback(event);
    });

    glfwSetKeyCallback(m_pWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        switch (action)
        {
            case GLFW_PRESS:
            {
                fox::KeyPressedEvent event(GLFW_TO_KEYBOARD_KEYS[key], 0);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                fox::KeyReleasedEvent event(GLFW_TO_KEYBOARD_KEYS[key]);
                data.EventCallback(event);
                break;
            }
            case GLFW_REPEAT:
            {
                fox::KeyPressedEvent event(GLFW_TO_KEYBOARD_KEYS[key], 1);
                data.EventCallback(event);
                break;
            }
        }
    });

    glfwSetCharCallback(m_pWindow, [](GLFWwindow* window, unsigned int keycode)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        fox::KeyTypedEvent event(GLFW_TO_KEYBOARD_KEYS[keycode]);
        data.EventCallback(event);
    });

    glfwSetMouseButtonCallback(m_pWindow, [](GLFWwindow* window, int button, int action, int mods)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        switch (action)
        {
            case GLFW_PRESS:
            {
                fox::MouseButtonPressedEvent event(static_cast<const fox::MouseButton>(button));
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                fox::MouseButtonReleasedEvent event(static_cast<const fox::MouseButton>(button));
                data.EventCallback(event);
                break;
            }
        }
    });

    glfwSetScrollCallback(m_pWindow, [](GLFWwindow* window, double xOffset, double yOffset)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        fox::MouseScrolledEvent event((float)xOffset, (float)yOffset);
        data.EventCallback(event);
    });

    glfwSetCursorPosCallback(m_pWindow, [](GLFWwindow* window, double xPos, double yPos)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        fox::MouseMovedEvent event((float)xPos, (float)yPos);
        data.EventCallback(event);
    });
}

GlfwWindow::~GlfwWindow()
{
    glfwDestroyWindow(m_pWindow);
    --s_GLFWWindowCount;

    if (s_GLFWWindowCount == 0)
    {
        glfwTerminate();
    }
}

void GlfwWindow::OnUpdate()
{
    glfwPollEvents();
    glfwSwapBuffers(m_pWindow);
}

uint32_t GlfwWindow::GetWidth() const
{
    return m_oData.width;
}

uint32_t GlfwWindow::GetHeight() const
{
    return m_oData.height;
}

void GlfwWindow::SetEventCallback(const fox::Window::EventCallbackFn &callback)
{
    m_oData.EventCallback = callback;
}

void GlfwWindow::SetVSync(bool enabled)
{
    if (enabled)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);
    m_oData.bIsVSync = enabled;
}

bool GlfwWindow::IsVSync() const
{
    return m_oData.bIsVSync;
}

void *GlfwWindow::GetNativeWindow() const
{
    return m_pWindow;
}

void GlfwWindow::SetNativeWindow(void *data)
{
    m_pWindow = static_cast<GLFWwindow*>(data);
}

bool GlfwWindow::IsKeyPressed(const fox::Key key)
{
    auto state = glfwGetKey(m_pWindow, static_cast<int32_t>(key));
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool GlfwWindow::IsMouseButtonPressed(const fox::MouseButton button)
{
    auto state = glfwGetMouseButton(m_pWindow, static_cast<int32_t>(button));
    return state == GLFW_PRESS;
}

glm::vec2 GlfwWindow::GetMousePosition()
{
    double xpos, ypos;
    glfwGetCursorPos(m_pWindow, &xpos, &ypos);
    return { (float)xpos, (float)ypos };
}
