#include <mbed.h>
#include "LCD_DISCO_F746NG.h"
#include "TS_DISCO_F746NG.h"
#include "DHT.h"
#include "display.h"
#include <iostream>
#include "fallout.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery_sdram.h"

// Serial USB COM port
Serial usbCom(USBTX, USBRX);

// LCD port
LCD_DISCO_F746NG lcd;
// LCD Touch port
TS_DISCO_F746NG ts;

// On-board LED
DigitalOut led1(LED1);
// External LED (red)
DigitalOut led2(D2);

// External buzzer
DigitalOut buzzer(D4);

// External sensors
DHT dhtSensor(A0, SEN51035P);
AnalogIn vibrationSensor(A1);
AnalogIn soundSensor(A2);

InterruptIn viewBtn(D3);
InterruptIn resetCounterBtn(BUTTON1);

// Touch Screen
TS_StateTypeDef TS_State;
uint16_t x, y;
uint8_t text[30];
uint8_t status;
uint8_t idx;
uint8_t cleared = 0;
uint8_t prev_nb_touches = 0;

// Counter
int counter_max = 9999;
int counter_current = 0;

/**
 * @brief Increases the current count or resets when counter_max is hit
 * 
 */
void counterTick()
{
  if (counter_current <= counter_max)
  {
    counter_current++;

    if (counter_current == counter_max)
    {
      counter_current = 0;
    }
  }

  char display_int[32];
  sprintf(display_int, "Count: %04u", counter_current);

  lcd.DisplayStringAt(0, LINE(LCD_LINE_COUNTER), (uint8_t *)display_int, CENTER_MODE);
}

// Display

/**
 * @brief Clears the main display & sets default colors 
 * 
 */
void clearDisplay()
{
  for (int i = 0; i <= 10; i++)
  {
    lcd.DisplayStringAt(0, LINE(i), (uint8_t *)"                         ", LEFT_MODE);
    lcd.DisplayStringAt(0, LINE(i), (uint8_t *)"                         ", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(i), (uint8_t *)"                         ", RIGHT_MODE);

    lcd.ClearStringLine(i);
  }

  lcd.Clear(LCD_BACKGROUND);
  lcd.SetBackColor(LCD_TEXT_BACKGROUND);
  lcd.SetTextColor(LCD_TEXT_COLOR);
}

// Sensors

/**
 * @brief Detects earth quakes. Sets the red led to ON/OFF.
 * 
 * @return int 
 */
int earthQuakeDetected()
{
  if (vibrationSensor.read() > 0.06)
  {
    led2 = 1;

    return 1;
  }

  led2 = 0;

  return 0;
}

/**
 * @brief Displays temperature (celsius) & humidity (humidity)
 * 
 */
void displayCurrentHT()
{
  int error = 0;
  float h = 0.0f, c = 0.0f;

  error = dhtSensor.readData();
  if (0 == error)
  {
    c = dhtSensor.ReadTemperature(CELCIUS);
    h = dhtSensor.ReadHumidity();

    char temp[32];
    char humid[32];

    sprintf(temp, "Temperature: %d C", (int)c);
    sprintf(humid, "Humidity: %d %%", (int)h);

    lcd.DisplayStringAt(0, LINE(LCD_LINE_HUMID), (uint8_t *)temp, CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(LCD_LINE_TEMP), (uint8_t *)humid, CENTER_MODE);
  }
}

/**
 * @brief Displays current earthquake status
 * 
 */
void displayEarthQuakeStatus()
{
  char earth_quake[32];
  sprintf(earth_quake, "Is Earthquake: %d", earthQuakeDetected());

  lcd.DisplayStringAt(0, LINE(LCD_LINE_EARTH_QUAKE), (uint8_t *)earth_quake, CENTER_MODE);
}

/**
 * @brief Resets the counter
 * 
 */
void resetCounter()
{
  usbCom.printf("Counter reset!\n");

  counter_current = 0;
}

// Views
int currentView = 1;
int viewCount = 3;

/**
 * @brief View 1 logic
 * 
 */
void view1()
{
  displayCurrentHT();
  displayEarthQuakeStatus();
  counterTick();
}

/**
 * @brief View 2 logic
 * 
 */
void view2()
{
  char i2c_clock[32];
  sprintf(i2c_clock, "I2C Clock: %d Hz", ((int)I2C_SPEED / 1000));
  lcd.DisplayStringAt(0, LINE(3), (uint8_t *)i2c_clock, CENTER_MODE);

  if (BSP_SD_IsDetected())
  {
    lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"SD Card detected", CENTER_MODE);
  }
  else
  {
    lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"SD Card not detected", CENTER_MODE);
  }
}

void view3()
{
  status = ts.Init(lcd.GetXSize(), lcd.GetYSize());
  if (status != TS_OK)
  {
    usbCom.printf("Touchscreen initialization failed!\n");
  }

  lcd.DisplayStringAt((uint16_t)10, VIEW3_BTN_1_Y, (uint8_t *)"Danger Zone", CENTER_MODE);
  lcd.DisplayStringAt((uint16_t)10, VIEW3_BTN_2_Y, (uint8_t *)"Final Countdown", CENTER_MODE);

  ts.GetState(&TS_State);
  if (TS_State.touchDetected)
  {
    // Clear lines corresponding to old touches coordinates
    if (TS_State.touchDetected < prev_nb_touches)
    {
      for (idx = (TS_State.touchDetected + 1); idx <= 5; idx++)
      {
        lcd.ClearStringLine(idx);
      }
    }
    prev_nb_touches = TS_State.touchDetected;

    cleared = 0;

    for (idx = 0; idx < TS_State.touchDetected; idx++)
    {
      x = TS_State.touchX[idx];
      y = TS_State.touchY[idx];
      sprintf((char *)text, "Touch %d: x=%d y=%d    ", idx + 1, x, y);
      lcd.DisplayStringAt(0, LINE(idx + 1), (uint8_t *)&text, LEFT_MODE);

      int y_hitbox_min = VIEW3_BTN_1_Y - 10;
      int y_hitbox_max = VIEW3_BTN_1_Y + 10;

      if (y >= y_hitbox_min && y <= y_hitbox_max)
      {
        buzzer = 1;
        wait(.5);
      }
    }
  }
  else
  {
    if (!cleared)
    {
      clearDisplay();
      lcd.DisplayStringAt(0, LINE(0), (uint8_t *)&text, LEFT_MODE);
      cleared = 1;
    }
  }
}

/**
 * @brief Set the View object
 * 
 * @param viewNumber 
 */
void setView(int viewNumber)
{
  if (viewNumber != currentView)
  {
    usbCom.printf("setView viewNumber %d!\n", viewNumber);
    usbCom.printf("setView currentView %d!\n", currentView);

    cleared = 0;

    clearDisplay();
  }

  currentView = viewNumber;

  switch (viewNumber)
  {
  case 1:
    view1();
    break;
  case 2:
    view2();
    break;
  case 3:
    view3();
    break;

  default:
    usbCom.printf("Unknown view number: %d\n", viewNumber);
    break;
  }
}

/**
 * @brief Switches the current view
 * 
 */
void switchView()
{
  wait(0.1);

  usbCom.printf("switchView toggled!\n");

  int viewNumber = currentView;

  if (currentView < viewCount)
  {
    viewNumber++;
  }
  else
  {
    viewNumber = 1;
  }

  usbCom.printf("switchView viewNumber %d!\n", viewNumber);
  usbCom.printf("switchView currentView %d!\n", currentView);

  setView(viewNumber);
}

/**
 * @brief Setup for app
 * 
 */
void boot()
{
  led1 = 1;
  lcd.Init();
  buzzer = 0;

  //uint8_t *image = (uint8_t *)fallout;
  //lcd.DrawBitmap(0, 0, image);

  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)"Booting Hercules...", CENTER_MODE);
  lcd.DisplayStringAt(0, LINE(4), (uint8_t *)"Please wait", CENTER_MODE);

  if (BSP_SD_IsDetected())
  {
    BSP_SD_Init();
    lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"SD Card detected", CENTER_MODE);
  }
  else
  {
    lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"SD Card not detected", CENTER_MODE);
  }

  wait(1.0f);

  clearDisplay();
}

/**
 * @brief Application entry point
 * 
 * @return int 
 */
int main()
{
  boot();

  viewBtn.rise(&switchView);
  resetCounterBtn.rise(&resetCounter);

  wait(0.25);

  while (true)
  {
    setView(currentView);

    ts.GetState(&TS_State);
    if (TS_State.touchDetected)
    {
      for (idx = 0; idx < TS_State.touchDetected; idx++)
      {
        x = TS_State.touchX[idx];
        y = TS_State.touchY[idx];

        int y_hitbox_min = 240 - 10;
        int y_hitbox_max = 240 + 10;

        int x_hitbox_min = 410 - 10;
        int x_hitbox_max = 410 + 10;

        if ((y >= y_hitbox_min && y <= y_hitbox_max) && (x >= x_hitbox_min && x <= x_hitbox_max))
        {
          switchView();
        }
      }
    }
    else
    {
      if (!cleared)
      {
        clearDisplay();

        char displayView[32];
        sprintf(displayView, "View: %d", currentView);

        lcd.DrawHLine(10, 10, 460);
        lcd.SetFont(&Font8);
        lcd.DisplayStringAt(10, 12, (uint8_t *)"Hercules v1.0", LEFT_MODE);
        lcd.DrawHLine(10, 20, 460);

        lcd.SetFont(&Font16);

        lcd.DisplayStringAt(10, 240, (uint8_t *)displayView, LEFT_MODE);
        lcd.DisplayStringAt(410, 240, (uint8_t *)"Next", LEFT_MODE);

        cleared = 1;
      }
    }
  }
}
