#include <stdlib.h>
#include <vector>
#include "sensors.h"
#include "DHT.h"
#include "rtos.h"
#include "cmsis_os.h"
#include "mbed.h"

/////////////////////////////////////////////////////////
/////// DHT - Dewity Humidy & Temperature Sensor ///////
/////////////////////////////////////////////////////////

DHT dhtSensor(DHT_PIN, DHT_TYPE);
Thread dhtThread;
bool dhtThreadRunning = false;
std::vector<u_int8_t> dhtResult;

void dhtTaskContext(std::vector<u_int8_t> *dhtResult)
{
    std::vector<u_int8_t> result;

    int error = 0;
    float h = 0.0f, c = 0.0f, d = 0.0f;

    dhtThreadRunning = true;

    while (true)
    {
        error = dhtSensor.readData();
        if (0 == error)
        {
            c = dhtSensor.ReadTemperature(CELCIUS);
            h = dhtSensor.ReadHumidity();
            d = dhtSensor.CalcdewPointFast(c, h);

            char temp[32];
            char humid[32];
            char dew_point[32];

            sprintf(temp, "Temperature: %d C", (int)c);
            sprintf(humid, "Humidity: %d %%", (int)h);
            sprintf(dew_point, "Dew Point: %d C", (int)d);

            result.push_back((u_int8_t &)temp);
            result.push_back((u_int8_t &)humid);
            result.push_back((u_int8_t &)dew_point);

            *dhtResult = result;
        }

        dhtThread.wait(1000);
    }
}

void dhtTask()
{
    dhtTaskContext(&dhtResult);
}

osStatus startDhtTask()
{
    return dhtThread.start(dhtTask);
}

std::vector<u_int8_t> getDHT()
{
    return dhtResult;
}

void stopDht()
{
    dhtThread.terminate();
}

/////////////////////////////////
/////// EarthQuake Sensor ///////
/////////////////////////////////
AnalogIn vibrationSensor(EARTH_QUAKE_PIN);
Thread earthQuakeThread;
bool earthQuakeThreadRunning = false;
bool isEarthQuake = false;

/**
 * @brief Task Context for EarthQuake detection
 * 
 * @param earthQuakeRes 
 */
void earthQuakeTaskContext(bool *earthQuakeRes)
{
    earthQuakeThreadRunning = true;

    while (true)
    {
        if (vibrationSensor.read() > 0.06)
        {
            *earthQuakeRes = true;
        }

        *earthQuakeRes = false;
    }
}

/**
 * @brief EarthQuake Task
 * 
 */
void earthQuakeTask()
{
    earthQuakeTaskContext(&isEarthQuake);
}

/**
 * @brief EarthQuake Task Initializer
 * 
 */
osStatus InitEarthQuakeDetection()
{
    if (!earthQuakeThreadRunning)
    {
        return earthQuakeThread.start(&earthQuakeTask);
    }
}
