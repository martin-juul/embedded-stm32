#include "LCD_DISCO_F746NG.h"

/** 
  * @brief  LCD color  
  */
#define LCD_BACKGROUND ((uint32_t)0xFF011301)
#define LCD_TEXT_BACKGROUND ((uint32_t)0xFF1D3D1C)
#define LCD_TEXT_COLOR ((uint32_t)0xFF3DF67C)

/** 
  * @brief  LCD Line Delegations  
  */
#define LCD_LINE_1 ((int)1)
#define LCD_LINE_2 ((int)2)
#define LCD_LINE_3 ((int)3)
#define LCD_LINE_4 ((int)4)
#define LCD_LINE_COUNTER ((int)5)
#define LCD_LINE_5 ((int)6)
#define LCD_LINE_HUMID ((int)7)
#define LCD_LINE_TEMP ((int)8)
#define LCD_LINE_EARTH_QUAKE ((int)9)

#define VIEW3_BTN_1_Y ((uint16_t)90)
#define VIEW3_BTN_2_Y ((uint16_t)130)
#define VIEW3_BTN_3_Y ((uint16_t)240)

class Display
{
public:
  Display();
  void String(int line, u_int8_t *text, Text_AlignModeTypdef Mode);
};