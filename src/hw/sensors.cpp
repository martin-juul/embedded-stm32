#include <stdlib.h>
#include <vector>
#include "sensors.h"
#include "DHT.h"
#include "rtos.h"
#include "cmsis_os.h"

/////// DHT - Dewity Humidy & Temperature Sensor ///////

DHT dhtSensor(DHT_PIN, DHT_TYPE);
Thread dhtThread;
bool dhtThreadRunning = false;
std::vector<char[32]> dhtResult;

void dhtTaskContext(std::vector<char> *dhtResult)
{
    std::vector<char> result;

    int error = 0;
    float h = 0.0f, c = 0.0f, d = 0.0f;

    dhtThreadRunning = true;

    while(true)
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

            result.push_back(&temp);
            result.push_back(&humid);
            result.push_back(&dew_point);

            *dhtResult = result;
        }

        dhtThread.wait(1000);
    }
    
}

void dhtTask()
{
    dhtTaskContext(&dhtResult);
}

void startDhtTask()
{
    /* osStatus status = */ dhtThread.start(dhtTask);
}

std::vector<char> getDHT()
{
    return dhtResult;
}

void stopDht()
{
    dhtThread.terminate();
}