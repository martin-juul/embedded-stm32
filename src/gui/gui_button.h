#pragma once

typedef struct GUI_Button
{
    char *label;
    int yPos;
    int xPos;

    void (*callback) (GUI_Button* p);
} GUI_Button_t;

typedef void (*callback_t) (GUI_Button_t* p);