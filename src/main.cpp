#include <mbed.h>
#include "LCD_DISCO_F746NG.h"
#include "DHT.h"
#include "display.h"
#include <iostream>
#include "fallout.h"

// Serial USB COM port
Serial usbCom(USBTX, USBRX);

// LCD port
LCD_DISCO_F746NG lcd;

// On-board LED
DigitalOut led1(LED1);
// External LED (red)
DigitalOut led2(D2);
// External button
DigitalOut btn(D3);

// External sensors
DHT dhtSensor(A0, SEN51035P);
AnalogIn vibrationSensor(A1);
AnalogIn soundSensor(A2);

// Counter
int counter_max = 9999;
int counter_current = 0;

/**
 * @brief Increases the current count or resets when counter_max is hit
 * 
 */
void counterTick()
{
  //usbCom.printf("%d max\n", counter_max);
  //usbCom.printf("%d current\n", counter_current);

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
 * @brief Setup for app
 * 
 */
void boot()
{
  led1 = 1;
  lcd.Init();

  clearDisplay();

  //uint8_t *image = (uint8_t *)fallout;
  //lcd.DrawBitmap(0, 0, image);

  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)"Booting Hercules...", CENTER_MODE);
  lcd.DisplayStringAt(0, LINE(4), (uint8_t *)"Please wait", CENTER_MODE);

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

  while (true)
  {
    displayCurrentHT();
    displayEarthQuakeStatus();
    counterTick();
  }
}