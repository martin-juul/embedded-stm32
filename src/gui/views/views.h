#pragma once
#include "stdlib.h"
#include <vector>
#include "gui/gui_action.h"
#include "gui/gui_textbox.h"

class AbstractView
{
    private:
        std::vector< GuiAction > actions;

    public:
        void Tick();

};

class HomeView : AbstractView
{};

class MusicView : AbstractView
{};

class SystemView : AbstractView
{};