#pragma once

class GuiManager {
    private:
        void displayStringAt(int yPos, int xPos);
        void drawIconAt(int icon, int yPos, int xPos);

    public:
        void Init();
};