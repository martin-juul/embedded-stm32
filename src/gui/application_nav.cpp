#include "application_nav.h"
#include <stdlib.h>
#include <vector>

// Private class member variables
ApplicationNav *ApplicationNav::instance = 0;
std::vector<__AppNavItem> nav_items;

///// Private Methods //////

ApplicationNav::ApplicationNav()
{}

///// Public Methods //////

/**
 * @brief Returns a static instance
 * 
 * @return ApplicationNav* 
 */
ApplicationNav *ApplicationNav::getInstance()
{
    if (instance == 0)
    {
        instance = new ApplicationNav();
    }

    return instance;
}

/**
 * @brief Initializes the navigation buttons
 * 
 * @param items 
 */
void Init(AppNavItem items[])
{
    // This is used for spacing the icons vertically
    unsigned int lastItemPos = 0;

    for (unsigned int i = 0; i < sizeof(items); i++)
    {
        AppNavItem item = items[i];
        lastItemPos = lastItemPos + APP_NAV_ITEM_MARGIN_TOP;

        __AppNavItem nav_item = {
            &item,
            APP_NAV_WIDTH / 2,
            lastItemPos
        };

        nav_items.push_back(nav_item);
    }
};

/**
 * @brief Get the Nav Items vector
 * 
 * @return std::vector< __AppNavItem > 
 */
std::vector< __AppNavItem > getNavItems()
{
    return nav_items;
}