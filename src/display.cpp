#include "display.h"
#include "LCD_DISCO_F746NG.h"

LCD_DISCO_F746NG screen;

Display::Display()
{
  //screen.Init();
}

void Display::String(int line, u_int8_t *text, Text_AlignModeTypdef Mode)
{
  screen.DisplayStringAt(0, LINE(line), text, Mode);
}