#pragma once
#include "mbed.h"

#define FONT_SIZE_SMALL 8
#define FONT_SIZE_NORMAL 12
#define FONT_SIZE_LARGE 16
#define FONT_SIZE_HUGE 20

struct Font_t {
    uint8_t *text;
    int size;
};