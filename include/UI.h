//
// Created by besseausamuel on 15/09/2020.
//
#ifndef OWNIMGUI_UI_H
#define OWNIMGUI_UI_H
#include <SFML/Graphics.hpp>
#include <iostream>

namespace FoxGUI
{
    #define UIWindowsMax 10
    #define UIWidgetsMax 128
    #define UIAutoLayoutGroup 16

    #define UIIDGen() UIIDInit(__LINE__ + UI_SRC_ID, 0)
    #define UIIDGenI() UIIDInit(__LINE__ + UI_SRC_ID, i)
    
    typedef union v4
    {
        struct
        {
            float x;
            float y;
            union
            {
                struct
                {
                    float z;
                    float w;
                };

                struct
                {
                    float width;
                    float height;
                };
            };
        };
        struct
        {
            float r;
            float g;
            float b;
            float a;
        };
    } v4;

    typedef union v2
    {
        struct
        {
            float x;
            float y;
        };
    } v2;

    enum
    {
        UIWidget_button,
        UIWidget_slider,
        UIWidget_checkbox,
        UIWidget_edit_text,
    };

    enum
    {
        UILayoutAlign_Left,
        UILayoutAlign_Right,
        UILayoutAlign_Center,
    };

    typedef struct UIID
    {
        int primary;
        int secondary;
    } UIID;

    UIID UIIDNull();
    class UIWidget
    {
    public:
        UIID id;
        int type;
        v4 rect;
        float t_hot;
        float t_active;
        std::string strText;

        union
        {
            struct
            {
                float value;
            } slider;

            struct
            {
                bool value;
                bool bLeftCursorDownPrevState;
            } checkbox;

            struct
            {
                bool bHandle;
            } combo;

            struct
            {
                bool prev_state;
            } edit_text;
        };
    };

    typedef struct UIInput
    {
    public:
        float m_fCursorX;
        float m_fCursorY;
        bool bLeftCursorDown;
        bool bRightCursorDown;
        bool bBackspaceDown;
        unsigned int m_uHeight;
        unsigned int m_uWidth;
    } UIInput;

    class UIWindow
    {
    public:
        explicit UIWindow()
        {
            auto_layout_stack_pos = 0;
            iWidgetCount = 0;
            hot = UIIDNull();
            active = UIIDNull();
        }

        UIWidget widgets[UIWidgetsMax];
        int iWidgetCount;

        sf::RectangleShape renders[UIWidgetsMax];
        int iRenderCount;

        sf::Text text_renders[UIWidgetsMax];
        int iTextRenderCount;

        UIID hot;
        UIID active;

        v2 position;
        v2 size;
        v2 border_size;
        v2 resizer_position;
        v2 resizer_size;
        bool bDragging;
        bool bResizing;
        bool bInitPos;

        UIInput m_oPreviousInput;

        int auto_layout_stack_pos;
        struct
        {
            bool isColumn;
            v2 position;
            v2 size;
            float progressX;
            float progressY;
        } auto_layout_stack[UIAutoLayoutGroup];
    };

    class UI
    {
    public:
        explicit UI()
        {
            iWindowCount = 0;
            oFont.loadFromFile("_fail_.ttf");

            m_oBackgroundColor = {
                66, 106, 168, 255
            };

            m_oTextColor = {
                255, 255, 225, 255
            };
        }

        UIWindow windows[UIWindowsMax];
        int iWindowCount;

        UIInput m_oCurrentInput;

        std::string strInputChar;

        sf::Font oFont;
        v4 m_oBackgroundColor;
        v4 m_oTextColor;
    };

    UIID UIIDInit(int primary, int secondary);
    void UIBeginFrame(UI& ui, std::string_view strTitle, const v2 position, const v2 size);
    // void UIBeginFrame(UI& ui, std::string_view strTitle, const v2 size);
    // void UIBeginFrame(UI& ui);
    void UIEndFrame(UI& ui);

    bool UIButton(UI& ui, char* pText);
    void UISlider(UI& ui, char* pText, float& value);
    void UICheckbox(UI& ui, char* pText, bool& value);
    bool UICombo(UI& ui, unsigned int& iChoice, char** someChoices);
    bool UIEditText(UI& ui, std::string& strText);

    // void UIPushColumn(UI& ui, v2 size, int flag = UILayoutAlign_Left);
    void UIPushColumn(UI& ui, v2 position, v2 size);
    void UIPopColumn(UI& ui);
    // void UIPushRow(UI& ui, v2 size, int flag);
    void UIPushRow(UI& ui, v2 position, v2 size);
    void UIPopRow(UI& ui);

    void ProcessEvent(UI& ui, sf::Event& oEvent);
    void Render(UI& ui, sf::RenderTarget& pTarget);
}

#endif //OWNIMGUI_UI_H