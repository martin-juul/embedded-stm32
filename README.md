# Embedded Course

[![Build Status](https://travis-ci.org/martin-juul/embedded-stm32.svg?branch=master)](https://travis-ci.org/martin-juul/embedded-stm32) | [Docs](https://martin-juul.github.io/embedded-stm32/)

## Overview

The interface is currently divided into 3 views.

### Home

Features

+ Counter
+ Temp sensor reading
+ Humidity reading
+ Earthquake detection

#### Counter

The counter can be reset using the on-board USER_B1 button (the blue one).

#### Temperature & Humidity

These readings arrives from the Grove DHT sensor. It is read faster than the sensor can keep up with, therefore it handles errors - so it does not disturb the display.

#### Earthquake detection

This works via the Piezo Vibration Sensor. The amount of hz needed for earthquake detection is only for show, and not an actual real-life scale. On detection `Is Earthquake` displays 1, and the external `LED SOCKET KIT` with a red LED lights up.

### System

Features

+ I2C Clock speed display
+ SD Card detection display

This view shows the I2C clock speed, converted to kHZ.

The SD card state is read from `BSP_SD_IsDetected()`

### Radio Player

This was meant to play tones aligning with the songs `Danger Zone` & `The Final Countdown`. The buzzer was annoyingly loud, and needs a resistor for being bearable.

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