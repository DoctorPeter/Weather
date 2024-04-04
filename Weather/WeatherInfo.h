/**************************************************************************************

	Project : Weather

	Module : WeatherInfo.h

	Description : This module contains type definitions and function declarations for the WeatherInfo class,
				  which contains information about the weather forecast for tomorrow
				  for a specific city.

**************************************************************************************/

#ifndef WEATHER_INFO_H
#define WEATHER_INFO_H

// Temperature scale types
#define C_SCALE 0
#define F_SCALE 1

// Definition of string data sizes
#define CITY_NANE_SIZE 64
#define WIND_DIR_SIZE 5

// Definition of the data type for the Show method of the WeatherInfo class
typedef void(*WeatherInfo_Show_Method)(void*, int);

// Structure definition of the WeatherInfo class
typedef struct _weatherInfo
{
	// City name
	char cityName[CITY_NANE_SIZE];

	// Minimum and maximum temperature in Celsius
	int minTempC;
	int maxTempC;

	// Minimum and maximum temperature in Fahrenheit
	int minTempF;
	int maxTempF;

	// Atmospheric pressure
	int preasure;

	// Humidity
	int humidity;

	// Wind speed
	int windSpeed;

	// Wind direction
	char windDirection[WIND_DIR_SIZE];

	// Cloudiness
	int cloudiness;

	// Precipitation
	int precipitation;

	// Show method - display weather forecast data on the screen
	WeatherInfo_Show_Method Show;
} WEATHER_INFO, *PWEATHER_INFO;

// Constructor for the WeatherInfo class
PWEATHER_INFO WeatherInfo_Constructor(void);

// Destructor for the WeatherInfo class
void WeatherInfo_Destructor(PWEATHER_INFO _this);

// Display weather forecast data on the screen
void WeatherInfo_Show(void * _this, int scale);

#endif
