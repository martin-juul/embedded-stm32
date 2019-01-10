#include <mbed.h>
#include "LCD_DISCO_F746NG.h"
#include "DHT.h"
#include <iostream>

LCD_DISCO_F746NG lcd;
DigitalOut led1(LED1);
DigitalOut led2(D2);

DHT sensor(A0, SEN51035P);
AnalogIn vibrationSensor(A1);
AnalogIn soundSensor(A2);

Serial usbCom(USBTX, USBRX);

uint32_t LCD_BACKGROUND = 0xFF011301;
uint32_t LCD_TEXT_BACKGROUND = 0xFF1D3D1C;
uint32_t LCD_TEXT_COLOR = 0xFF3DF67C;

void clearDisplay()
{
  lcd.Clear(LCD_BACKGROUND);
  lcd.SetBackColor(LCD_TEXT_BACKGROUND);
  lcd.SetTextColor(LCD_TEXT_COLOR);
}

void boot()
{
  led1 = 1;
  lcd.Init();

  clearDisplay();

  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)"Booting Hercules...", CENTER_MODE);
  lcd.DisplayStringAt(0, LINE(4), (uint8_t *)"Please wait", CENTER_MODE);

  wait(1);

  clearDisplay();
}

int is_earth_quake()
{
  if (vibrationSensor.read() > 0.06)
  {
    led2 = 1;

    return 1;
  }

  led2 = 0;

  return 0;
}

void run_temp_humid()
{

  int error = 0;
  float h = 0.0f, c = 0.0f;

  //wait(2.0f);
  error = sensor.readData();
  if (0 == error)
  {
    c = sensor.ReadTemperature(CELCIUS);
    h = sensor.ReadHumidity();

    char temp[32];
    char humid[32];

    sprintf(temp, "Temperature: %d C", (int)c);
    sprintf(humid, "Humidity: %d %%", (int)h);

    lcd.DisplayStringAt(0, LINE(7), (uint8_t *)temp, CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(8), (uint8_t *)humid, CENTER_MODE);
  }
  else
  {
    //usbCom.printf("Error: %d\n", error);
  }

  char earth_quake[32];
  sprintf(earth_quake, "Is Earthquake: %d", is_earth_quake());
  lcd.DisplayStringAt(0, LINE(9), (uint8_t *)earth_quake, CENTER_MODE);
}

int counter_max = 9999;
int counter_current = 0;

void run_counter()
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
  sprintf(display_int, "Count: %u", counter_current);

  lcd.DisplayStringAt(0, LINE(5), (uint8_t *)display_int, CENTER_MODE);
}

int main()
{
  boot();

  while (true)
  {
    run_temp_humid();
    run_counter();
  }
}
