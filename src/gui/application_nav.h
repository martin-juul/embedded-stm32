#pragma once

#include "application_window.h"
#include <stdlib.h>
#include <vector>

////// Constants //////

#define APP_NAV_HEIGHT 272
#define APP_NAV_WIDTH 80

#define APP_NAV_ITEM_MARGIN_TOP 20
#define APP_NAV_ITEM_COLOR_BG 0xFF011301
#define APP_NAV_ITEM_COLOR_ICON 0xFF3DF67C
#define APP_NAV_ITEM_COLOR_LABEL 0xFF3DF67C
#define APP_NAV_ITEM_COLOR_LABEL_BG 0xFF1D3D1C

////// Structures //////

struct AppNavItem
{
    char *label;
    int icon;
    ApplicationWindow *window;
};

struct __AppNavItem
{
    AppNavItem *item;
    int xPos;
    int yPos;
};

/**
 * @brief ApplicationNav Class Definition
 * 
 */
class ApplicationNav
{
  private:
    static std::vector<__AppNavItem> *nav_items;
    static ApplicationNav *instance;

    ApplicationNav();

  public:
    void Init(AppNavItem items[]);

    static ApplicationNav *getInstance();

    std::vector<__AppNavItem> getNavItems();
};