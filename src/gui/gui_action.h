#pragma once
#include "gui_button.h"
#include <string>

union AppAction_t
{
    GUI_Button *button;
};

class GuiAction
{
    private:
        GuiAction();

    public:
        void GetInstance();
};