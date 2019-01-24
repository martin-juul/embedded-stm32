#pragma once
#include "rtos.h"

class GuiManager {
    private:
        Thread _guiThread;

        void displayStringAt(int yPos, int xPos);
        void drawIconAt(int icon, int yPos, int xPos);

    public:
        void Init();
};