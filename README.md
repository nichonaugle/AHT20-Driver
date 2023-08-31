# AHT20 Sensor Library

This repository contains a C library for interfacing with the AHT20 sensor using ESP32. It can be easily modified to work on other C based microcontrollers such as the arduino, arduino nano, etc. Link to datasheet: https://cdn.sparkfun.com/assets/d/2/b/e/d/AHT20.pdf

## Table of Contents

- [Introduction](#introduction)
- [Setup](#setup)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Introduction

The AHT20 sensor is a digital temperature and humidity sensor. This library provides a set of functions to initialize, calibrate, and read data from the AHT20 sensor. It stores all of the data in one struct containing meassurements for humidity, temperature (celsius), and temperature (farenheight).

## Setup and Usage

To get started with the library, follow these steps:

1. Connect the AHT20 sensor to your ESP32 board as fits your application
2. Change the data rate of I2C if needed as well as the selected pins for the I2C communication. Internal pullups are enabled on the I2C line so none are required unless your application requires it.
3. Create a struct to store your data and run the following functions in order: AHT20_init(), AHT20_calibrate(), and then AHT20_read().

## Contributing

Contributions to this library are welcome! If you find issues or want to enhance the library, please feel free to open an issue or submit a pull request.

##License

This project is licensed under the MIT License.
