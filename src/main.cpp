#include <mbed.h>
#include "LCD_DISCO_F746NG.h"
#include "TS_DISCO_F746NG.h"
#include "DHT.h"
#include "display.h"
#include <iostream>
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery_sdram.h"
#include "rtos.h"

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
AnalogIn lightSensor(A2);
AnalogIn soundSensor(A3);

int currentView = 1;
int viewCount = 2;

int tempScale = 0;

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

int roomNumber;
int buildingNumber;

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

  if (1 == currentView)
  {
    lcd.DisplayStringAt(0, LINE(4), (uint8_t *)display_int, CENTER_MODE);
  }
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
 * @brief Displays temperature & humidity (humidity)
 * 
 */
void displayCurrentHT()
{
  int error = 0;
  float h = 0.0f, c = 0.0f, d = 0.0f;

  error = dhtSensor.readData();
  if (0 == error)
  {
    c = dhtSensor.ReadTemperature((eScale)tempScale);
    h = dhtSensor.ReadHumidity();
    d = dhtSensor.CalcdewPointFast(c, h);

    char temp[32];
    char tempScaleSymbol[8];
    char humid[32];
    char dew_point[32];

    switch (tempScale)
    {
    case 0:
      sprintf(tempScaleSymbol, "C");
      break;
    case 1:
      sprintf(tempScaleSymbol, "F");
      break;
    case 2:
      sprintf(tempScaleSymbol, "K");
      break;

    default:
      sprintf(tempScaleSymbol, "C");
      break;
    }

    sprintf(temp, "Temperature: %d %s", (int)c, tempScaleSymbol);
    sprintf(humid, "Humidity: %d %%", (int)h);
    sprintf(dew_point, "Dew Point: %d %s", (int)d, tempScaleSymbol);

    lcd.DisplayStringAt(0, LINE(6), (uint8_t *)temp, CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(7), (uint8_t *)humid, CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(8), (uint8_t *)dew_point, CENTER_MODE);
  }
}

/**
 * @brief Says "godnat" when dark, "goddag" when light.
 * 
 */
void displayLightStatus()
{
  float luminence = lightSensor.read();

  if (luminence < 0.3)
  {
    lcd.DisplayStringAt(0, LINE(12), (uint8_t *)"Godnat", CENTER_MODE);
  }
  else
  {
    lcd.DisplayStringAt(0, LINE(12), (uint8_t *)"Goddag", CENTER_MODE);
  }
}

Thread noiseThread;
bool noiseThreadStarted = false;

void noiseWarning()
{
  while (noiseThreadStarted)
  {
    float noiseLevel = soundSensor.read();

    if (noiseLevel >= 0.35 && 1 == currentView)
    {
      lcd.DisplayStringAt(0, LINE(13), (uint8_t *)"Noise warning!", CENTER_MODE);
    }
    else
    {
      lcd.SetBackColor(LCD_BACKGROUND);
      lcd.DisplayStringAt(0, LINE(13), (uint8_t *)"              ", CENTER_MODE);
      lcd.SetBackColor(LCD_TEXT_BACKGROUND);
    }

    ThisThread::sleep_for(1000);
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

  lcd.DisplayStringAt(0, LINE(10), (uint8_t *)earth_quake, CENTER_MODE);
}

// Views

int device_setup_building_no = 0;
int device_setup_room_no = 0;

void drawButton(uint16_t xPos, u_int16_t yPos, uint8_t *Text, int dia = 10, int xOffset = 5, int yOffset = 7)
{
  lcd.DrawCircle(xPos, yPos, dia);
  lcd.SetBackColor(LCD_BACKGROUND);
  lcd.DisplayStringAt((xPos - xOffset), (yPos - yOffset), Text, LEFT_MODE);
  lcd.SetBackColor(LCD_TEXT_BACKGROUND);
}

/**
 * @brief Checks if a button has been hit
 * 
 * @param xPos 
 * @param yPos 
 * @param cursor_x 
 * @param cursor_y 
 * @param hitBoxAera 
 * @return true 
 * @return false 
 */
bool buttonHit(uint16_t xPos, uint16_t yPos, uint16_t cursor_x, uint16_t cursor_y, uint16_t hitBoxAera = 20)
{
  int y_hitbox_min = yPos - hitBoxAera;
  int y_hitbox_max = yPos + hitBoxAera;

  int x_hitbox_min = xPos - hitBoxAera;
  int x_hitbox_max = xPos + hitBoxAera;

  if ((cursor_y >= y_hitbox_min && cursor_y <= y_hitbox_max) && (cursor_x >= x_hitbox_min && cursor_x <= x_hitbox_max))
  {
    return true;
  }

  return false;
}

void deviceSetup()
{
  char str_building[32];
  char str_room[32];

  sprintf(str_building, "Building: %d", device_setup_building_no);
  sprintf(str_room, "Room: %d", device_setup_room_no);

  lcd.DisplayStringAt(125, 75, (uint8_t *)str_building, LEFT_MODE);
  lcd.DisplayStringAt(125, 150, (uint8_t *)str_room, LEFT_MODE);

  // Building adjustment
  uint16_t building_incr_xPos = 300;
  uint16_t building_incr_yPos = 80;
  drawButton(building_incr_xPos, building_incr_yPos, (uint8_t *)"+");

  uint16_t building_decr_xPos = 350;
  uint16_t building_decr_yPos = 80;
  drawButton(building_decr_xPos, building_decr_yPos, (uint8_t *)"-");

  // Room adjustment
  uint16_t room_incr_xPos = 300;
  uint16_t room_incr_yPos = 155;
  drawButton(room_incr_xPos, room_incr_yPos, (uint8_t *)"+");

  uint16_t room_decr_xPos = 350;
  uint16_t room_decr_yPos = 155;
  drawButton(room_decr_xPos, room_decr_yPos, (uint8_t *)"-");

  // Save button
  uint16_t save_btn_xPos = 240;
  uint16_t save_btn_yPos = 230;
  drawButton(save_btn_xPos, save_btn_yPos, (uint8_t *)"Save", 32, 22);

  ts.GetState(&TS_State);
  if (TS_State.touchDetected)
  {
    for (idx = 0; idx < TS_State.touchDetected; idx++)
    {
      x = TS_State.touchX[idx];
      y = TS_State.touchY[idx];

      // Building adjustment
      if (buttonHit(building_incr_xPos, building_incr_yPos, x, y))
      {
        device_setup_building_no++;

        wait(0.5f);
      }

      if (buttonHit(building_decr_xPos, building_decr_yPos, x, y))
      {
        device_setup_building_no--;

        wait(0.5f);
      }

      // Room adjustment
      if (buttonHit(room_incr_xPos, room_incr_yPos, x, y))
      {
        device_setup_room_no++;

        wait(0.5f);
      }

      if (buttonHit(room_decr_xPos, room_incr_yPos, x, y))
      {
        device_setup_room_no--;

        wait(0.5f);
      }

      if (buttonHit(save_btn_xPos, save_btn_yPos, x, y))
      {
        buildingNumber = device_setup_building_no;
        roomNumber = device_setup_room_no;

        wait(0.5f);

        cleared = false;
      }
    }
  }
}

Thread counterThread;
bool counterThreadStarted = false;

void counter_thread()
{
  while (counterThreadStarted)
  {
    counterTick();

    ThisThread::sleep_for(1000);
  }
}

/**
 * @brief View 1 logic
 * 
 */
void view1()
{
  displayCurrentHT();
  displayEarthQuakeStatus();
  displayLightStatus();

  if (!counterThreadStarted)
  {
    counterThread.start(counter_thread);

    counterThreadStarted = true;
  }

  if (!noiseThreadStarted)
  {
    noiseThread.start(noiseWarning);

    noiseThreadStarted = true;
  }
}

/**
 * @brief View 2 logic
 * 
 */
void view2()
{
  char i2c_clock[32];
  sprintf(i2c_clock, "I2C Clock: %d KHz", ((int)I2C_SPEED / 1000));
  lcd.DisplayStringAt(0, LINE(3), (uint8_t *)i2c_clock, CENTER_MODE);

  if (BSP_SD_IsDetected())
  {
    lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"SD Card detected", CENTER_MODE);
  }
  else
  {
    lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"SD Card not detected", CENTER_MODE);
  }

  lcd.DisplayStringAt(145, 150, (uint8_t *)"Temperature scale", LEFT_MODE);

  uint16_t tempBtnYpos = 190;
  uint16_t tempBtnCelsiusPos = 170;
  uint16_t tempBtnFahrenheitPos = 230;
  uint16_t tempBtnKelvinPos = 290;

  drawButton(tempBtnCelsiusPos, tempBtnYpos, (uint8_t *)"C");
  drawButton(tempBtnFahrenheitPos, tempBtnYpos, (uint8_t *)"F");
  drawButton(tempBtnKelvinPos, tempBtnYpos, (uint8_t *)"K");

  ts.GetState(&TS_State);
  if (TS_State.touchDetected)
  {
    for (idx = 0; idx < TS_State.touchDetected; idx++)
    {
      x = TS_State.touchX[idx];
      y = TS_State.touchY[idx];

      // Building adjustment
      if (buttonHit(tempBtnCelsiusPos, tempBtnYpos, x, y))
      {
        tempScale = 0;

        usbCom.printf("Temp scale switched to Celcius!");

        wait(0.5f);
      }

      if (buttonHit(tempBtnFahrenheitPos, tempBtnYpos, x, y))
      {
        tempScale = 1;

        usbCom.printf("Temp scale switched to Fahrenheit!");

        wait(0.5f);
      }

      if (buttonHit(tempBtnKelvinPos, tempBtnYpos, x, y))
      {
        tempScale = 2;

        usbCom.printf("Temp scale switched to Kelvin!");

        wait(0.5f);
      }
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
    usbCom.printf("[Toggle]: setView viewNumber %d!\n", viewNumber);
    usbCom.printf("[Toggle]: setView currentView %d!\n", currentView);

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
 * @brief Gui Thread
 * 
 */
void gui_thread()
{
  while (true)
  {
    if (!buildingNumber && !roomNumber)
    {
      deviceSetup();
    }
    else
    {
      setView(currentView);
    }

    ts.GetState(&TS_State);
    if (buildingNumber && roomNumber && TS_State.touchDetected)
    {
      for (idx = 0; idx < TS_State.touchDetected; idx++)
      {
        x = TS_State.touchX[idx];
        y = TS_State.touchY[idx];

        int y_hitbox_min = 240 - 50;
        int y_hitbox_max = 240 + 50;

        int x_hitbox_min = 410 - 50;
        int x_hitbox_max = 410 + 50;

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

        lcd.SetBackColor(LCD_BACKGROUND);
        lcd.DisplayStringAt(10, 12, (uint8_t *)"Hercules v1.1", LEFT_MODE);
        lcd.DrawVLine(80, 10, 11);

        if (buildingNumber && roomNumber)
        {
          char building[32];
          char room[32];

          sprintf(building, "Building: %d", buildingNumber);
          sprintf(room, "Room: %d", roomNumber);

          lcd.DisplayStringAt(85, 12, (uint8_t *)building, LEFT_MODE);
          lcd.DisplayStringAt(150, 12, (uint8_t *)room, LEFT_MODE);
        }

        lcd.SetBackColor(LCD_TEXT_BACKGROUND);

        lcd.DrawHLine(10, 20, 460);
        lcd.SetFont(&Font16);

        if (buildingNumber && roomNumber)
        {
          lcd.DisplayStringAt(10, 240, (uint8_t *)displayView, LEFT_MODE);
          lcd.DisplayStringAt(410, 240, (uint8_t *)"Next", LEFT_MODE);
        }

        cleared = 1;
      }
    }
  }
}

Thread guiThread;

/**
 * @brief Application entry point
 * 
 * @return int 
 */
int main()
{
  boot();

  // The data cache is disabled to avoid screen flickering.
  SCB_CleanDCache();
  SCB_DisableDCache();

  wait(0.25);

  guiThread.start(gui_thread);
}
