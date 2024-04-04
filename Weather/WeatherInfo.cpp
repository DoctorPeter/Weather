/**************************************************************************************

	Project : Weather

	Module : WeatherInfo.cpp

	Description : The module contains the implementation of the methods of the WeatherInfo class.

**************************************************************************************/

#include "stdafx.h"
#include <stdlib.h>
#include "WeatherInfo.h"

/**************************************************************************************

	Method :  WeatherInfo_Constructor

	Returns : pointer to an object

	Description : Constructor of objects of the WeatherInfo class.

**************************************************************************************/
PWEATHER_INFO WeatherInfo_Constructor(void)
{
	// Memory allocation for the object
	PWEATHER_INFO _this = (PWEATHER_INFO)malloc(sizeof(WEATHER_INFO));

	// Initialization of fields
	_this->cityName[0] = '\0';
	_this->minTempC = 0;
	_this->minTempF = 0;
	_this->maxTempC = 0;
	_this->maxTempF = 0;
	_this->preasure = 0;
	_this->humidity = 0;
	_this->windSpeed = 0;
	_this->windDirection[0] = '\0';
	_this->cloudiness = 0;
	_this->precipitation = 0;

	// Initialization of methods
	_this->Show = WeatherInfo_Show;

	return _this;
}

/**************************************************************************************

	Method :  WeatherInfo_Destructor

	Parameters :
				_this - pointer to an object of the WeatherInfo class

	Description : Destructor of objects of the WeatherInfo class.

**************************************************************************************/
void WeatherInfo_Destructor(PWEATHER_INFO _this)
{
	if (_this) free(_this);
}

/**************************************************************************************

	Method :  WeatherInfo_Show

	Parameters :
				_this - pointer to an object of the WeatherInfo class
				scale - temperature scale

	Description : Display weather forecast data on the screen.

**************************************************************************************/
void WeatherInfo_Show(void * _this, int scale)
{
	PWEATHER_INFO wInfo = (PWEATHER_INFO)_this;

	printf("---------------------------------------------\n");
	printf("City: %s\n", wInfo->cityName);

	if (scale == C_SCALE)
	{
		printf("     Minimal temperature: %d\n", wInfo->minTempC);
		printf("     Maximal temperature: %d\n", wInfo->maxTempC);
	}
	else if (scale == F_SCALE)
	{
		printf("     Minimal temperature: %d\n", wInfo->minTempF);
		printf("     Maximal temperature: %d\n", wInfo->maxTempF);
	}

	printf("     Preasure: %d\n", wInfo->preasure);
	printf("     Humidity: %d\n", wInfo->humidity);
	printf("     Wind speed: %d\n", wInfo->windSpeed);
	printf("     Wind direction: %s\n", wInfo->windDirection);
	printf("     Cloudiness: %d\n", wInfo->cloudiness);
	printf("     Precipitation: %d\n", wInfo->precipitation);
	printf("---------------------------------------------\n");
	printf("\n");
}
