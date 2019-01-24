#pragma once
#include <string>

#define WINDOW_HEIGHT 272
#define WINDOW_WIDTH 480

union WindowItemType
{
    char *text;
};

struct WindowItem
{
    int xPos;
    int yPos;
    WindowItemType item;
};

struct ApplicationWindow
{
    char *name;
    int itemCount;
    WindowItem items[];
};