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
        virtual void Tick();

};

class HomeView : AbstractView
{
    private:
    std::vector< GuiAction > actions;

    public:
        void Tick();
};

class MusicView : AbstractView
{
    private:
    std::vector< GuiAction > actions;

    public:
        void Tick();
};

class SystemView : AbstractView
{
    private:
    std::vector< GuiAction > actions;

    public:
        void Tick();
};