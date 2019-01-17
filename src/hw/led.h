#pragma once
#include <mbed.h>

// LED Pinout
#define ONBOARD_LED LED1
#define EXTERNAL_LED D2

class LED
{
    private:
        DigitalOut led1(PinName pin);
        DigitalOut led2(PinName pin);

        int SwitchPin(PinName Pin, bool OnOff);
    
    public:
        LED();
        int On(PinName pin);
        int Off(PinName pin);
};