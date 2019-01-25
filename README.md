# Embedded Course

[![Build Status](https://travis-ci.org/martin-juul/embedded-stm32.svg?branch=master)](https://travis-ci.org/martin-juul/embedded-stm32) | [Docs](https://martin-juul.github.io/embedded-stm32/) | [Repository](https://github.com/martin-juul/embedded-stm32)

## Overview

The interface is currently divided into 3 views.

### Device Setup

Features

+ Enter building number
+ Enter room number

These are displayed in the top nav.

### Home

Features

+ Counter
+ Temp sensor reading
+ Humidity reading
+ Dew point
+ Earthquake detection
+ Noise warning

#### Counter

Counts from 0 to 9999 & resets.

#### Temperature & Humidity

These readings arrives from the Grove DHT sensor. It is read faster than the sensor can keep up with, therefore it handles errors - so it does not disturb the display.

#### Dew point

The dew point is the temperature to which air must be cooled to become saturated with water vapour. When further cooled, the airborne water vapor will condense to form liquid water. When air cools to its dew point through contact with a surface that is colder than the air, water will condense on the surface. - Wikipedia

#### Earthquake detection

This works via the Piezo Vibration Sensor. The amount of hz needed for earthquake detection is only for show, and not an actual real-life scale. On detection `Is Earthquake` displays 1, and the external `LED SOCKET KIT` with a red LED lights up.

### System

Features

+ I2C Clock speed display
+ SD Card detection display
+ Set temperature scale to Celsius, Fahrenheit & Kelvin

This view shows the I2C clock speed, converted to kHZ.

The SD card state is read from `BSP_SD_IsDetected()`

## Connecting external sensors

On the Arduino base shield:

| Sensor  | Pin  |
|---|---|
| LED SOCKET KIT  | D2  |
| Grove - Buzzer  |  D4 |
| Grove - Temperature & Humidity Sensor Pro v1.2  |  A0 |
| Piezo Vibration Sensor  | A1  |
| Grove - Light Sensor  | A2  |
| Grove - Sound Sensor  | A2  |

## Roadmap

+ Split logic into its own seperate classes. Preferably with a ViewComposer, that abstracts the LCD and Touch logic away.

+ SD Card Filesystem

+ Sound output from onboard `CN10 OUT`

+ MP3 Player

+ Ethernet with IPv4 & DHCP Client registration

+ HTTP Webserver, serving a page for controlling the MP3 player.

## Libraries used in project

+ BSP_DISCO_F746NG@df2ea349c3

+ LCD_DISCO_F746NG@d44525b1de

+ DHT_ID2256

+ TS_DISCO_746NG_ID2804
