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

        void SwitchPin(PinName Pin, bool OnOff);
    
    public:
        LED();
        void On(PinName pin);
        void Off(PinName pin);
};