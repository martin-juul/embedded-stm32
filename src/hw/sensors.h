#include <stdlib.h>
#include <vector>
#include "DHT.h"

#define DHT_PIN A0
#define DHT_TYPE SEN51035P

#define EARTH_QUAKE_PIN A1

class Sensors
{
    private:

    public:
        std::vector< u_int8_t > getDHT();
        void stopDht();

        bool IsEarthQuake();
};