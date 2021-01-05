//
// Created by besseausamuel on 15/09/2020.
//

#include "UI.h"
#include "SFML/Graphics.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <stdarg.h>
#include "sid.h"

#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define IM_MAX(A, B)            (((A) >= (B)) ? (A) : (B))
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

namespace FoxGUI
{
    std::vector<std::string> split_str(const char *str, const char separator)
    {
        std::vector<std::string> output;
        std::string substring;
        std::istringstream stream{str};

        while (std::getline(stream, substring, separator))
            output.push_back(substring);

        return output;
    }

    void ProcessEvent(UI& ui, sf::Event& oEvent)
    {
        switch (oEvent.type)
        {
        case sf::Event::MouseButtonPressed:
            ui.m_oCurrentInput.bLeftCursorDown = oEvent.mouseButton.button == sf::Mouse::Button::Left ? true : ui.m_oCurrentInput.bLeftCursorDown;
            ui.m_oCurrentInput.bRightCursorDown = oEvent.mouseButton.button == sf::Mouse::Button::Right ? true : ui.m_oCurrentInput.bLeftCursorDown;
            break;
        case sf::Event::MouseButtonReleased:
            ui.m_oCurrentInput.bLeftCursorDown = oEvent.mouseButton.button == sf::Mouse::Button::Left ? false : ui.m_oCurrentInput.bLeftCursorDown;
            ui.m_oCurrentInput.bRightCursorDown = oEvent.mouseButton.button == sf::Mouse::Button::Right ? false : ui.m_oCurrentInput.bLeftCursorDown;
            break;
        
        case sf::Event::Resized:
            ui.m_oCurrentInput.m_uHeight = oEvent.size.height;
            ui.m_oCurrentInput.m_uWidth = oEvent.size.width;
            break;
            
        case sf::Event::MouseMoved:
            ui.m_oCurrentInput.m_fCursorX = oEvent.mouseMove.x;
            ui.m_oCurrentInput.m_fCursorY = oEvent.mouseMove.y;
            break;
        case sf::Event::KeyReleased:
            ui.m_oCurrentInput.bBackspaceDown = oEvent.key.code == sf::Keyboard::BackSpace ? false : ui.m_oCurrentInput.bBackspaceDown;
            break;
        case sf::Event::KeyPressed:
            ui.m_oCurrentInput.bBackspaceDown = oEvent.key.code == sf::Keyboard::BackSpace ? true : ui.m_oCurrentInput.bBackspaceDown;
            break;
        case sf::Event::TextEntered:
            if (oEvent.text.unicode < ' ' && oEvent.text.unicode == 127)
                break;
            ui.strInputChar += oEvent.text.unicode;
            break;
        default:
            break;
        }
    }

    UIID UIIDInit(int primary, int secondary)
    {
        UIID id = { primary, secondary };
        return id;
    }

    UIID UIIDNull()
    {
        UIID id = { 0, 0 };
        return id;
    }

    bool UIIDEqual(UIID id1, UIID id2)
    {
        return id1.primary == id2.primary && id1.secondary == id2.secondary;
    }

    v4 UIGetNextAutoLayoutRect(UI& ui)
    {
        v4 rect = {0};
        UIWindow& window = ui.windows[ui.iWindowCount - 1];

        if (window.auto_layout_stack_pos > 0)
        {
            int i = window.auto_layout_stack_pos - 1;
            rect.x = window.auto_layout_stack[i].position.x;
            rect.y = window.auto_layout_stack[i].position.y;
            rect.width = window.auto_layout_stack[i].size.x;
            rect.height = window.auto_layout_stack[i].size.y;

            if (window.auto_layout_stack[i].isColumn)
            {
                rect.y += window.auto_layout_stack[i].progressY;
                window.auto_layout_stack[i].progressY += rect.height;
            }
            else
            {
                rect.x += window.auto_layout_stack[i].progressX;
                window.auto_layout_stack[i].progressX += rect.width;
            }

            if (rect.x + rect.width > window.position.x + window.size.x)
            {
                float left = IM_MAX(rect.x, window.position.x);
                float width = IM_MIN(rect.x + rect.width, window.position.x + window.size.x) - left;
                rect.width = IM_MAX(0, width);
            }

            if (rect.y + rect.height > window.position.y + window.size.y)
            {
                float top = IM_MAX(rect.y, window.position.y);
                float height = IM_MIN(rect.y + rect.height, window.position.y + window.size.y) - top;
                rect.height = IM_MAX(0, height);
            }
        }
        else
            rect = { 0, 0, 64, 64 };
        return rect;
    }

    void UIBeginFrame(UI& ui, std::string_view strTitle, const v2 position, const v2 size)
    {
        UIWindow& window = ui.windows[ui.iWindowCount++];
        window.border_size = v2{window.size.x, 20};
        window.iWidgetCount = 0;
        window.resizer_size = {15, 15};
        window.resizer_position = {window.position.x + window.size.x - window.resizer_size.x, window.position.y + window.size.y - window.resizer_size.y};

        if (!window.bInitPos)
        {
            window.position = position;
            window.bInitPos = true;
            window.size = size;
        }

        bool bCursorIsOverBorder = ui.m_oCurrentInput.m_fCursorX >= window.position.x && ui.m_oCurrentInput.m_fCursorX <= window.position.x + window.border_size.x &&
                                    ui.m_oCurrentInput.m_fCursorY >= window.position.y && ui.m_oCurrentInput.m_fCursorY <= window.position.y + window.border_size.y;
        
        bool bCursorIsOverResizer = ui.m_oCurrentInput.m_fCursorX >= window.resizer_position.x && ui.m_oCurrentInput.m_fCursorX <= window.resizer_position.x + window.resizer_size.x &&
                                    ui.m_oCurrentInput.m_fCursorY >= window.resizer_position.y && ui.m_oCurrentInput.m_fCursorY <= window.resizer_position.y + window.resizer_size.y;

        if (bCursorIsOverBorder && ui.m_oCurrentInput.bLeftCursorDown && !window.bDragging) {
            window.bDragging = true;
        }
        else if (ui.m_oCurrentInput.bLeftCursorDown && window.bDragging) {
            const v2 mouse_delta{ui.m_oCurrentInput.m_fCursorX - window.m_oPreviousInput.m_fCursorX, ui.m_oCurrentInput.m_fCursorY - window.m_oPreviousInput.m_fCursorY};
            const v2 new_position{window.position.x + mouse_delta.x, window.position.y + mouse_delta.y};

            window.position = new_position;
        }
        else if (!ui.m_oCurrentInput.bLeftCursorDown && window.bDragging) {
            window.bDragging = false;
        }

        if (bCursorIsOverResizer && ui.m_oCurrentInput.bLeftCursorDown && !window.bResizing) {
            window.bResizing = true;
        }
        else if (ui.m_oCurrentInput.bLeftCursorDown && window.bResizing) {
            const v2 mouse_delta{ui.m_oCurrentInput.m_fCursorX - window.m_oPreviousInput.m_fCursorX, ui.m_oCurrentInput.m_fCursorY - window.m_oPreviousInput.m_fCursorY};
            const v2 new_size{window.size.x + mouse_delta.x, window.size.y + mouse_delta.y};

            window.size.x = IM_CLAMP(new_size.x, 20, ui.m_oCurrentInput.m_uWidth);
            window.size.y = IM_CLAMP(new_size.y, 20, ui.m_oCurrentInput.m_uHeight);
        }
        else if (!ui.m_oCurrentInput.bLeftCursorDown && window.bResizing) {
            window.bResizing = false;
        }

        window.m_oPreviousInput.m_fCursorX = ui.m_oCurrentInput.m_fCursorX;
        window.m_oPreviousInput.m_fCursorY = ui.m_oCurrentInput.m_fCursorY;
        window.m_oPreviousInput.bLeftCursorDown = ui.m_oCurrentInput.bLeftCursorDown;
        window.m_oPreviousInput.bRightCursorDown = ui.m_oCurrentInput.bRightCursorDown;
        window.m_oPreviousInput.bBackspaceDown = ui.m_oCurrentInput.bBackspaceDown;
    }

    void Render(UI& ui, sf::RenderTarget& pTarget)
    {
        for (int i = 0; i < ui.iWindowCount; ++i)
        {
            UIWindow& window = ui.windows[i];

            for (int j = 0; j < window.iRenderCount; ++j)
                pTarget.draw(window.renders[j]);

            for (int j = 0; j < window.iTextRenderCount; ++j)
                pTarget.draw(window.text_renders[j]);

            window.iRenderCount = 0;
            window.iTextRenderCount = 0;
        }
        ui.iWindowCount = 0;
        ui.strInputChar.clear();
    }
    
    void DrawFilledRect(UI& ui, v4 color, v2 position, v2 size)
    {
        UIWindow& window = ui.windows[ui.iWindowCount - 1];
        sf::RectangleShape& shape = window.renders[window.iRenderCount++];
        shape.setFillColor(sf::Color(color.r, color.g, color.b, color.a));
        shape.setPosition(position.x, position.y);
        shape.setSize(sf::Vector2f(size.x, size.y));
    }

    void DrawText(UI& ui, const std::string& strText, v4 color, v2 position, v2 size)
    {
        UIWindow& window = ui.windows[ui.iWindowCount - 1];
        sf::Text& text = window.text_renders[window.iTextRenderCount++];
        text.setCharacterSize(20);
        text.setPosition(position.x, position.y);
        text.setFont(ui.oFont);
        text.setColor(sf::Color(color.r, color.g, color.b, color.a));
        text.setString(strText);

        //center text
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width * 0.5f, textRect.top  + textRect.height * 0.5f);
        text.setPosition(position.x + size.x * 0.5f, position.y + size.y * 0.5f);
    }

    float remap(float x, float iInMin, float iInMax, float iOutMin, float iOutMax)
    {
        return (x - iInMin) * (iOutMax - iOutMin) / (iInMax - iInMin) + iOutMin;
    }

    void UIEndFrame(UI& ui)
    {
        UIWindow& window = ui.windows[ui.iWindowCount - 1];

        v4 window_color = {
            64, 130, 81, 255
        };

        v4 border_color = {
            80, 171, 104, 255,
        };

        v4 resizer_color = {
            90, 171, 104, 255,
        };

        DrawFilledRect(ui, window_color, window.position, window.size);
        DrawFilledRect(ui, border_color, window.position, window.border_size);
        DrawFilledRect(ui, resizer_color, window.resizer_position, window.resizer_size);

        for (int i = 0; i < window.iWidgetCount; ++i)
        {
            UIWidget *widget = window.widgets + i;
            widget->t_hot += ((float)(!!UIIDEqual(window.hot, widget->id)) - widget->t_hot) * 0.02f /*<-Delta*/ * 4.f;
            widget->t_active += ((float)(!!UIIDEqual(window.active, widget->id)) - widget->t_active) * 0.02f /*<-Delta*/ * 4.f;

            switch (widget->type)
            {
                case UIWidget_button:
                {
                    const float fIntensityPressed = 0.5f;
                    const float fIntensityHighlight = 0.25f;

                    v4 color = {
                        remap(0.26f + widget->t_hot * fIntensityHighlight - widget->t_active * fIntensityPressed, 0.f, 1.f, 0.f, 255.f),
                        remap(0.42f + widget->t_hot * fIntensityHighlight - widget->t_active * fIntensityPressed, 0.f, 1.f, 0.f, 255.f),
                        remap(0.66f + widget->t_hot * fIntensityHighlight - widget->t_active * fIntensityPressed, 0.f, 1.f, 0.f, 255.f),
                        255,
                    };

                    DrawFilledRect(ui, color, v2{widget->rect.x, widget->rect.y}, v2{widget->rect.width, widget->rect.height});
                    if (!widget->strText.empty() && widget->rect.width != 0 && widget->rect.height != 0)
                        DrawText(ui, widget->strText, ui.m_oTextColor, v2{widget->rect.x, widget->rect.y}, v2{widget->rect.width, widget->rect.height});
                    break;
                }

                case UIWidget_slider:
                {
                    v4 color = {
                        95, 137, 201, 255
                    };

                    DrawFilledRect(ui, ui.m_oBackgroundColor, v2{widget->rect.x, widget->rect.y}, v2{widget->rect.width, widget->rect.height});
                    DrawFilledRect(ui, color, v2{widget->rect.x, widget->rect.y}, v2{widget->rect.width * widget->slider.value, widget->rect.height});

                    if (!widget->strText.empty() && widget->rect.width != 0 && widget->rect.height != 0)
                        DrawText(ui, widget->strText, ui.m_oTextColor, v2{widget->rect.x, widget->rect.y}, v2{widget->rect.width, widget->rect.height});
                    break;
                }

                case UIWidget_checkbox:
                {
                    const float fIntensityPressed = 0.5f;
                    const float fIntensityHighlight = 0.25f;
                    v4 backgroundColor = {
                        remap(0.26f + widget->t_hot * fIntensityHighlight - widget->t_active * fIntensityPressed, 0.f, 1.f, 0.f, 255.f),
                        remap(0.42f + widget->t_hot * fIntensityHighlight - widget->t_active * fIntensityPressed, 0.f, 1.f, 0.f, 255.f),
                        remap(0.66f + widget->t_hot * fIntensityHighlight - widget->t_active * fIntensityPressed, 0.f, 1.f, 0.f, 255.f),
                        255,
                    };

                    const float fColorIntensity = .8f;
                    v4 color = {
                        remap(fColorIntensity * 0.26f, 0.f, 1.f, 0.f, 255.f),
                        remap(fColorIntensity * 0.42f, 0.f, 1.f, 0.f, 255.f),
                        remap(fColorIntensity * 0.66f, 0.f, 1.f, 0.f, 255.f),
                        255,
                    };

                    DrawFilledRect(ui, backgroundColor, v2{widget->rect.x, widget->rect.y}, v2{widget->rect.width, widget->rect.height});
                    if (widget->checkbox.value)
                        DrawFilledRect(ui, color, v2{widget->rect.x, widget->rect.y}, v2{widget->rect.width, widget->rect.height});
                    
                    if (!widget->strText.empty() && widget->rect.width != 0 && widget->rect.height != 0)
                        DrawText(ui, widget->strText, ui.m_oTextColor, v2{widget->rect.x, widget->rect.y}, v2{widget->rect.width, widget->rect.height});
                    break;
                }

                case UIWidget_edit_text:
                {
                    const float fIntensityPressed = 0.5f;
                    const float fIntensityHighlight = 0.25f;

                    v4 color = {
                        remap(0.26f + widget->t_hot * fIntensityHighlight - widget->t_active * fIntensityPressed, 0.f, 1.f, 0.f, 255.f),
                        remap(0.42f + widget->t_hot * fIntensityHighlight - widget->t_active * fIntensityPressed, 0.f, 1.f, 0.f, 255.f),
                        remap(0.66f + widget->t_hot * fIntensityHighlight - widget->t_active * fIntensityPressed, 0.f, 1.f, 0.f, 255.f),
                        255,
                    };

                    DrawFilledRect(ui, color, v2{widget->rect.x, widget->rect.y}, v2{widget->rect.width, widget->rect.height});
                    if (!widget->strText.empty())
                        DrawText(ui, widget->strText, ui.m_oTextColor, v2{widget->rect.x, widget->rect.y}, v2{widget->rect.width, widget->rect.height});
                    break;
                }

                default:
                    break;
            }
        }
    }

    bool UIButtonP(UI& ui, char* pText, v4 rect)
    {
        UIWindow& window = ui.windows[ui.iWindowCount - 1];
        UIID id = UIIDInit(SID_VAL(pText), 0);
        std::vector<std::string> id_split = split_str(pText, '#');

        bool bIsTriggered = false;
        bool bCursorIsOver = window.m_oPreviousInput.m_fCursorX >= rect.x && window.m_oPreviousInput.m_fCursorX <= rect.x + rect.width &&
                            window.m_oPreviousInput.m_fCursorY >= rect.y && window.m_oPreviousInput.m_fCursorY <= rect.y + rect.height;

        if (!UIIDEqual(window.hot, id) && bCursorIsOver)
            window.hot = id;
        else if (UIIDEqual(window.hot, id) && !bCursorIsOver)
            window.hot = UIIDNull();

        if (UIIDEqual(window.active, id))
        {
            if (!window.m_oPreviousInput.bLeftCursorDown)
            {
                bIsTriggered = UIIDEqual(window.hot, id);
                window.active = UIIDNull();
            }
        }
        else
        {
            if (UIIDEqual(window.hot, id))
                if (window.m_oPreviousInput.bLeftCursorDown)
                    window.active = id;
        }

        UIWidget *widget = window.widgets + window.iWidgetCount++;
        widget->type = UIWidget_button;
        widget->rect = rect;
        widget->id = id;
        widget->strText = id_split[0];

        return bIsTriggered;
    }

    bool UIButton(UI& ui, char* pText)
    {
        v4 rect = UIGetNextAutoLayoutRect(ui);
        return UIButtonP(ui, pText, rect);
    }

    std::string format(const std::string& format, ...)
    {
        va_list args;
        va_start(args, format);
        size_t len = std::vsnprintf(NULL, 0, format.c_str(), args);
        va_end(args);
        std::vector<char> vec(len + 1);
        va_start(args, format);
        std::vsnprintf(&vec[0], len + 1, format.c_str(), args);
        va_end(args);
        return &vec[0];
    }

    void UISliderP(UI& ui, char* pText, float& value, v4 rect)
    {
        UIWindow& window = ui.windows[ui.iWindowCount - 1];
        UIID id = UIIDInit(SID_VAL(pText), 0);
        std::vector<std::string> id_split = split_str(pText, '#');
        
        bool bCursorIsOver = window.m_oPreviousInput.m_fCursorX >= rect.x && window.m_oPreviousInput.m_fCursorX <= rect.x + rect.width &&
                            window.m_oPreviousInput.m_fCursorY >= rect.y && window.m_oPreviousInput.m_fCursorY <= rect.y + rect.height;

        if (!UIIDEqual(window.hot, id) && bCursorIsOver)
            window.hot = id;
        else if (UIIDEqual(window.hot, id) && !bCursorIsOver)
            window.hot = UIIDNull();

        if (!UIIDEqual(window.active, id) && UIIDEqual(window.hot, id))
        {
            if (window.m_oPreviousInput.bLeftCursorDown)
                window.active = id;
        }

        if (UIIDEqual(window.active, id))
        {
            if (window.m_oPreviousInput.bLeftCursorDown)
                value = (window.m_oPreviousInput.m_fCursorX - rect.x) / rect.width;
            else
                window.active = UIIDNull();
        }

        if (value < 0)
            value = 0.f;
        else if (value > 1)
            value = 1.0f;

        UIWidget *widget = window.widgets + window.iWidgetCount++;
        widget->type = UIWidget_slider;
        widget->rect = rect;
        widget->id = id;
        widget->slider.value = value;
        widget->strText = format("%s (%.2f)", id_split[0].c_str(), value);
    }

    void UISlider(UI& ui, char* pText, float& value)
    {
        v4 rect = UIGetNextAutoLayoutRect(ui);
        return UISliderP(ui, pText, value, rect);
    }

    void UICheckboxP(UI& ui, char* pText, bool& value, v4 rect)
    {
        UIWindow& window = ui.windows[ui.iWindowCount - 1];
        UIID id = UIIDInit(SID_VAL(pText), 0);
        std::vector<std::string> id_split = split_str(pText, '#');

        bool bCursorIsOver = window.m_oPreviousInput.m_fCursorX >= rect.x && window.m_oPreviousInput.m_fCursorX <= rect.x + rect.width &&
                            window.m_oPreviousInput.m_fCursorY >= rect.y && window.m_oPreviousInput.m_fCursorY <= rect.y + rect.height;

        if (!UIIDEqual(window.hot, id) && bCursorIsOver)
            window.hot = id;
        else if (UIIDEqual(window.hot, id) && !bCursorIsOver)
            window.hot = UIIDNull();

        if (!UIIDEqual(window.active, id) && UIIDEqual(window.hot, id))
        {
            if (window.m_oPreviousInput.bLeftCursorDown)
                window.active = id;
        }

        UIWidget* widget = window.widgets + window.iWidgetCount++;

        if (UIIDEqual(window.active, id))
        {
            if (window.m_oPreviousInput.bLeftCursorDown && !widget->checkbox.bLeftCursorDownPrevState)
                value = !value;
            else
                window.active = UIIDNull();
        }

        widget->type = UIWidget_checkbox;
        widget->rect = rect;
        widget->id = id;
        widget->checkbox.value = value;
        widget->strText = id_split[0];
        widget->checkbox.bLeftCursorDownPrevState = window.m_oPreviousInput.bLeftCursorDown;
    }

    void UICheckbox(UI& ui, char* pText, bool& value)
    {
        v4 rect = UIGetNextAutoLayoutRect(ui);
        return UICheckboxP(ui, pText, value, rect);
    }

    bool UICombo(UI& ui, unsigned int& iChoice, char** someChoices)
    {
        UIWindow& window = ui.windows[ui.iWindowCount - 1];
        UIWidget* widget = window.widgets + window.iWidgetCount++;
        bool& bHandle = widget->combo.bHandle;

        if(UIButton(ui, someChoices[iChoice]))
            bHandle = !bHandle;

        //expanded
        if(bHandle)
        {
            //list
            for (unsigned int i = 0; someChoices[i]; ++i) //terminate on NULL
            {
                if(UIButton(ui, someChoices[i]))
                {
                    iChoice = i;
                    bHandle = false;
                    return true;
                }
            }
        }
        return false;
    }

    bool UIEditTextP(UI& ui, std::string& strText, v4 rect)
    {
        UIWindow& window = ui.windows[ui.iWindowCount - 1];
        UIWidget* widget = window.widgets + window.iWidgetCount++;
        UIID id = UIIDInit(SID_VAL(format("##%d", window.iWidgetCount - 1).c_str()), 0);

        bool bCursorIsOver = window.m_oPreviousInput.m_fCursorX >= rect.x && window.m_oPreviousInput.m_fCursorX <= rect.x + rect.width &&
                            window.m_oPreviousInput.m_fCursorY >= rect.y && window.m_oPreviousInput.m_fCursorY <= rect.y + rect.height;

        if (!UIIDEqual(window.hot, id) && bCursorIsOver)
            window.hot = id;
        else if (UIIDEqual(window.hot, id) && !bCursorIsOver)
            window.hot = UIIDNull();
        if (!UIIDEqual(window.active, id) && UIIDEqual(window.hot, id))
        {
            if (window.m_oPreviousInput.bLeftCursorDown)
                window.active = id;
        }

        bool bIsTyping = false;
        if (UIIDEqual(window.active, id))
        {
            if (window.m_oPreviousInput.bBackspaceDown && !widget->edit_text.prev_state && strText.size() > 0)
            {
                strText.erase(strText.size() - 1, 1);
                bIsTyping = true;
            }
            else if (!window.m_oPreviousInput.bBackspaceDown && !widget->edit_text.prev_state)
            {
                strText += ui.strInputChar;
                bIsTyping = true;
            }

            if (window.m_oPreviousInput.bLeftCursorDown && !UIIDEqual(window.hot, id))
            {
                window.active = UIIDNull();
                ui.strInputChar.clear();
                bIsTyping = false;
            }
        }

        widget->type = UIWidget_edit_text;
        widget->rect = rect;
        widget->id = id;
        widget->strText = strText;
        widget->edit_text.prev_state = window.m_oPreviousInput.bBackspaceDown;
        return bIsTyping;
    }

    bool UIEditText(UI& ui, std::string& strText)
    {
        v4 rect = UIGetNextAutoLayoutRect(ui);
        return UIEditTextP(ui, strText, rect);
    }

    void UIPushColumn(UI& ui, v2 position, v2 size)
    {
        UIWindow& window = ui.windows[ui.iWindowCount - 1];
        int iParentLayout = window.auto_layout_stack_pos - 1;
        int iNewLayout = window.auto_layout_stack_pos++;
        v2 oParentPosition = {0};
        v2 oParentSize = {0};
        v2 oNewPosition = {0};

        // If we have a parent layout
        if (iParentLayout >= 0)
        {
            oParentPosition = window.auto_layout_stack[iParentLayout].position;
            oParentSize = window.auto_layout_stack[iParentLayout].size;
            float fProgressX = window.auto_layout_stack[iParentLayout].progressX;
            float fProgressY = window.auto_layout_stack[iParentLayout].progressY;

            oNewPosition = {oParentPosition.x + position.x, oParentPosition.y + position.y + fProgressY};
        }
        else
            oNewPosition = {window.position.x + position.x, window.position.y + window.border_size.y + position.y};

        window.auto_layout_stack[iNewLayout].position = oNewPosition;
        window.auto_layout_stack[iNewLayout].size = size;
        window.auto_layout_stack[iNewLayout].isColumn = true;
        window.auto_layout_stack[iNewLayout].progressX = size.x;
        window.auto_layout_stack[iNewLayout].progressY = 0;
    }

    void UIPopColumn(UI& ui)
    {
        UIWindow& window = ui.windows[ui.iWindowCount - 1];
        int iParentLayout = window.auto_layout_stack_pos - 2;

        // If we have a parent layout
        if (iParentLayout >= 0)
            window.auto_layout_stack[iParentLayout].progressY += window.auto_layout_stack[window.auto_layout_stack_pos - 1].progressY;

        if (window.auto_layout_stack_pos > 0) {
            window.auto_layout_stack_pos--;
        }
    }

    void UIPushRow(UI& ui, v2 position, v2 size)
    {
        UIWindow& window = ui.windows[ui.iWindowCount - 1];
        int iParentLayout = window.auto_layout_stack_pos - 1;
        int iNewLayout = window.auto_layout_stack_pos++;
        v2 oParentPosition = {0};
        v2 oParentSize = {0};
        v2 oNewPosition = {0};
        
        // If we have a parent layout
        if (iParentLayout >= 0)
        {
            oParentPosition = window.auto_layout_stack[iParentLayout].position;
            oParentSize = window.auto_layout_stack[iParentLayout].size;
            float fProgressX = window.auto_layout_stack[iParentLayout].progressX;
            float fProgressY = window.auto_layout_stack[iParentLayout].progressY;

            oNewPosition = {oParentPosition.x + position.x + fProgressX, oParentPosition.y + position.y};
        }
        else
            oNewPosition = {window.position.x + position.x, window.position.y + window.border_size.y + position.y};
        
        window.auto_layout_stack[iNewLayout].position = oNewPosition;
        window.auto_layout_stack[iNewLayout].size = size;
        window.auto_layout_stack[iNewLayout].isColumn = false;
        window.auto_layout_stack[iNewLayout].progressX = 0;
        window.auto_layout_stack[iNewLayout].progressY = size.y;
    }

    void UIPopRow(UI& ui)
    {
        UIWindow& window = ui.windows[ui.iWindowCount - 1];
        int iParentLayout = window.auto_layout_stack_pos - 2;

        // If we have a parent layout
        if (iParentLayout >= 0)
            window.auto_layout_stack[iParentLayout].progressX += window.auto_layout_stack[window.auto_layout_stack_pos - 1].progressX;

        if (window.auto_layout_stack_pos > 0) {
            window.auto_layout_stack_pos--;
        }
    }

    // void UIPushColumn(UI& ui, v2 size, int flag)
    // {
    //     UIWindow& window = ui.windows[ui.iWindowCount - 1];
    //     int i = window.auto_layout_stack_pos++;
    //     v2 position = {0};
    //     if ((flag & UILayoutAlign_Left) == UILayoutAlign_Left)
    //     {
    //         position = {window.position.x, window.position.y + window.border_size.y};
    //     }
    //     if ((flag & UILayoutAlign_Center) == UILayoutAlign_Center)
    //     {
    //         position = { window.position.x, window.position.y + window.border_size.y + (window.size.y - window.border_size.y) * 0.5f };
    //         position.y -= size.y * 0.5f;
    //     }
    //     if ((flag & UILayoutAlign_Right) == UILayoutAlign_Right)
    //     {
    //         position = { window.position.x, window.position.y + window.border_size.y + (window.size.y - window.border_size.y) - size.y };
    //     }
    //     window.auto_layout_stack[i].position = position;
    //     window.auto_layout_stack[i].size = size;
    //     window.auto_layout_stack[i].isColumn = true;
    //     window.auto_layout_stack[i].progress = 0;
    // }

    // void UIPushRow(UI& ui, v2 size, int flag)
    // {
    //     UIWindow& window = ui.windows[ui.iWindowCount - 1];
    //     int i = window.auto_layout_stack_pos++;

    //     v2 position = {0};
    //     if ((flag & UILayoutAlign_Left) == UILayoutAlign_Left)
    //     {
    //         position = {window.position.x, window.position.y + window.border_size.y};
    //     }
    //     if ((flag & UILayoutAlign_Center) == UILayoutAlign_Center)
    //     {
    //         position = { window.position.x + window.size.x * 0.5f, window.position.y + window.border_size.y };
    //         position.x -= size.x * 0.5f;
    //     }
    //     if ((flag & UILayoutAlign_Right) == UILayoutAlign_Right)
    //     {
    //         position = { window.position.x + window.size.x - size.x, window.position.y + window.border_size.y };
    //     }

    //     window.auto_layout_stack[i].position = position;
    //     window.auto_layout_stack[i].size = size;
    //     window.auto_layout_stack[i].isColumn = false;
    //     window.auto_layout_stack[i].progress = 0;
    // }
}