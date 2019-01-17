#include <mbed.h>
#include "hw/led.h"

// Private
DigitalOut led1(ONBOARD_LED);
DigitalOut led2(EXTERNAL_LED);

/**
 * @brief LED Constructor
 * 
 */
void LED(){};

/**
 * @brief Set Pin to On
 * 
 * @param Pin 
 */
void On(PinName Pin)
{
    SwitchPin(Pin, 1);
};

/**
 * @brief Set Pin to Off
 * 
 * @param Pin 
 */
void Off(PinName Pin)
{
    SwitchPin(Pin, 0);
};

/**
 * @brief Sets Pin state
 * 
 * @param Pin 
 * @param OnOff 
 */
void SwitchPin(PinName Pin, bool OnOff)
{
    switch (Pin)
    {
    case ONBOARD_LED:
        led1 = OnOff;
        return;
    case EXTERNAL_LED:
        led2 = OnOff;
        return;
    default:
        return;
    }
};