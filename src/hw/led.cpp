#include <mbed.h>
#include "hw/led.h"

// Private vars
DigitalOut led1(ONBOARD_LED);
DigitalOut led2(EXTERNAL_LED);

// Private methods

/**
 * @brief Sets Pin state
 * 
 * @param Pin 
 * @param OnOff 
 */
int SwitchPin(PinName Pin, bool OnOff)
{
    switch (Pin)
    {
    case ONBOARD_LED:
        led1 = OnOff;

        return 0;
    case EXTERNAL_LED:
        led2 = OnOff;

        return 0;
    default:
        return 1;
    }

    return 1;
};

// Public methods

/**
 * @brief LED Constructor
 * 
 */
void LED(){};

/**
 * @brief Set Pin to On, returns 0 when successful - 1 when not
 * 
 * @param Pin 
 * @return int 
 */
int On(PinName Pin)
{
    return SwitchPin(Pin, 1);
};

/**
 * @brief Set Pin to Off, returns 0 when successful - 1 when not
 * 
 * @param Pin 
 * @return int 
 */
int Off(PinName Pin)
{
    return SwitchPin(Pin, 0);
};