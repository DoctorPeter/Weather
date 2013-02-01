/**************************************************************************************
   
	Проект : Weather

	Модуль : WeatherInfo.h

	Описание : Модуль содержит определения типов и функций для класса WeatherInfo,
	           который содержит в себе информацию о прогнозе погоды на завтра
			   для определённого города.

**************************************************************************************/

#ifndef WEATHER_INFO_H
#define WEATHER_INFO_H

// Типы шкал температур
#define C_SCALE 0
#define F_SCALE 1

//  Определние размеров строковых данных
#define CITY_NANE_SIZE 64
#define WIND_DIR_SIZE 5


// Определение типа данных для метода Show
// класса WeatherInfo
typedef void (*WeatherInfo_Show_Method)(void*, int);

// Определение структуры класа WeatherInfo
typedef struct _weatherInfo
{
	// Название города
	char cityName[CITY_NANE_SIZE];

	// Минимальна и максимальная температура
	// по Цельсию
	int minTempC;
	int maxTempC;

	// Минимальна и максимальная температура
	// по Фаренгейту
	int minTempF;
	int maxTempF;
	
	//  Атмосферное давление
	int preasure;

	// Влажность
	int humidity;

	// Скорость ветра
	int windSpeed;

	// Направление ветра
	char windDirection[WIND_DIR_SIZE];

	// Облачность
	int cloudiness;

	// Осадки
	int precipitation;
	
	// Метод Show - отображение данных прогноза
	// погоды на экран
	WeatherInfo_Show_Method Show;
} WEATHER_INFO, *PWEATHER_INFO;

// Конструктор класса WeatherInfo
PWEATHER_INFO WeatherInfo_Constructor(void);

// Деструктор класса WeatherInfo
void WeatherInfo_Destructor(PWEATHER_INFO _this);

// Отображение данных прогноза погоды на экран
void WeatherInfo_Show(void * _this, int scale);

#endif