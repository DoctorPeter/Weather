/**************************************************************************************
   
	Проект : Weather

	Модуль : WeatherInfo.cpp

	Описание : Модуль содержит реализацию методов класса WeatherInfo.

**************************************************************************************/

#include "stdafx.h"

#include <stdlib.h>

#include "WeatherInfo.h"


/**************************************************************************************
   
    Метод :  WeatherInfo_Constructor
	
	Возвращает : указатель на объект

	Описание : Конструктор объектов класса WeatherInfo.

**************************************************************************************/
PWEATHER_INFO WeatherInfo_Constructor(void)
{
	// Выделение памяти под объект
	PWEATHER_INFO _this = (PWEATHER_INFO) malloc(sizeof(WEATHER_INFO));
	
	// Инициализация полей
	_this -> cityName[0] = '\0';
	_this -> minTempC = 0;
	_this -> minTempF = 0;
	_this -> maxTempC = 0;
	_this -> maxTempF = 0;
    _this -> preasure = 0;
	_this -> humidity = 0;
	_this -> windSpeed = 0;
	_this -> windDirection[0] = '\0';
	_this -> cloudiness = 0;
	_this -> precipitation = 0;
	
	// Инициализация методов
	_this -> Show = WeatherInfo_Show;

	return _this;
}

/**************************************************************************************
   
    Метод :  WeatherInfo_Destructor
	
	Параметры : 
				_this - указатель на объект класса WeatherInfo

	Описание : Деструктор объектов класса WeatherInfo.

**************************************************************************************/
void WeatherInfo_Destructor(PWEATHER_INFO _this)
{
	if (_this) free(_this);
}


/**************************************************************************************
   
    Метод :  WeatherInfo_Show
	
	Параметры : 
				_this - указатель на объект класса WeatherInfo
				scale - шклала температур

	Описание : Отображение данных прогноза погоды на экран

**************************************************************************************/
void WeatherInfo_Show(void * _this, int scale)
{
	PWEATHER_INFO wInfo = (PWEATHER_INFO)_this;

	printf("---------------------------------------------\n");
	printf("City: %s\n", wInfo -> cityName);

	if (scale == C_SCALE)
	{
		printf("     Minimal temperature: %d\n", wInfo -> minTempC);
		printf("     Maximal temperature: %d\n", wInfo -> maxTempC);
	}
	else
	if (scale == F_SCALE)
	{
		printf("     Minimal temperature: %d\n", wInfo -> minTempF);
		printf("     Maximal temperature: %d\n", wInfo -> maxTempF);
	}

	printf("     Preasure: %d\n", wInfo -> preasure);
	printf("     Humidity: %d\n", wInfo -> humidity);
	printf("     Wind speed: %d\n", wInfo -> windSpeed);
	printf("     Wind direction: %s\n", wInfo -> windDirection);
	printf("     Cloudiness: %d\n", wInfo -> cloudiness);
	printf("     Precipitation: %d\n", wInfo -> precipitation);
	printf("---------------------------------------------\n");
	printf("\n");
}

