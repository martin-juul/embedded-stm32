#include <mbed.h>
#include "LCD_DISCO_F746NG.h"
#include "DHT.h"

LCD_DISCO_F746NG lcd;
DigitalOut led1(LED1);

DHT sensor(A0, SEN51035P);
AnalogIn vibrationSensor(A1);
AnalogIn soundSensor(A2);

void boot()
{
  led1 = 1;
  lcd.Init();

  lcd.DisplayStringAt(0, LINE(1), (uint8_t *)"Booting Hercules...", CENTER_MODE);
  lcd.DisplayStringAt(0, LINE(4), (uint8_t *)"Please wait", CENTER_MODE);
  wait(1);
  lcd.Clear(LCD_COLOR_DARKCYAN);
}

int is_earth_quake()
{
  if (vibrationSensor.read() > 0.06)
  {
    return 1;
  }

  return 0;
}

void run_temp_humid()
{

  int error = 0;
  float h = 0.0f, c = 0.0f;

  wait(2.0f);
  error = sensor.readData();
  if (0 == error)
  {
    c = sensor.ReadTemperature(CELCIUS);
    h = sensor.ReadHumidity();

    char temp[32];
    char humid[32];

    sprintf(temp, "Temperature: %f", c);
    sprintf(humid, "Humidity: %f", h);

    lcd.DisplayStringAt(0, LINE(7), (uint8_t *)temp, CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(8), (uint8_t *)humid, CENTER_MODE);
  }
  else
  {
    printf("Error: %d\n", error);
  }

  char earth_quake[32];
  sprintf(earth_quake, "Is Earthquake: %d", is_earth_quake());
  lcd.DisplayStringAt(0, LINE(9), (uint8_t *)earth_quake, CENTER_MODE);
}

int counter_max = 9999;
int counter_current = 0;

void run_counter()
{
  if (counter_max <= counter_current)
  {
    uint32_t count = counter_current;
    char display_int[32];
    sprintf(display_int, "Count: %u", count);

    lcd.DisplayStringAt(0, LINE(6), (uint8_t *)display_int, CENTER_MODE);

    counter_current++;

    if (counter_current == counter_max)
    {
      counter_current = 0;
    }
  }
}

int main()
{
  boot();

  while (1)
  {
    run_temp_humid();
    run_counter();
  }
}
