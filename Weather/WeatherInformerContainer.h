/**************************************************************************************
   
	Проект : Weather

	Модуль : WeatherInformerContainer.h

	Описание : Модуль содержит определения типов и функций для класса 
	           WeatherInformerContainer, который позволяет одновременно работать
			   с 5-ю источниками погоды и формировать усреднённый прогноз.

**************************************************************************************/

#ifndef WEATHER_INFORMER_CONTAINER_H
#define WEATHER_INFORMER_CONTAINER_H

#include "WeatherInformer.h"

//  Определние размеров строковых данных
#define WEATHER_INFORMER_CONTAINER_SIZE 5
#define FILE_NAME_SIZE 256

// Определение типов данных для методов класса WeatherInformerContainer.
// Повторяющиеся определения введены для поддержания единой 
// концепции проектирования и для повышения читабельности кода.
typedef void (*WeatherInformerContainer_GetData_Method)(void *);
typedef void (*WeatherInformerContainer_ParseData_Method)(void *);
typedef void (*WeatherInformerContainer_CreateTotalData_Method)(void *);
typedef void (*WeatherInformerContainer_Show_Method)(void *);
typedef void (*WeatherInformerContainer_JsonOutput_Method)(void *);
typedef void (*WeatherInformerContainer_XmlOutput_Method)(void *);

// Определение структуры класа WeatherInformerContainer
typedef struct _WeatherInformerContainer
{
	// Массив погодных информеров
	PWEATHER_INFORMER container[WEATHER_INFORMER_CONTAINER_SIZE + 1];
	
	// Определение методов

	// Подключение к источнику и извлечение данных прогноза погоды 
	// для всех погодных информеров
	WeatherInformerContainer_GetData_Method GetData;
	
	// Анализ данных полученных от источника в формате JSON
	// для всех погодных информеров
	WeatherInformerContainer_ParseData_Method ParseData;

	// Создание усреднённого прогноза погоды
	WeatherInformerContainer_CreateTotalData_Method CreateTotalData;

	// Отображение данных прогноза погоды на экран
	// для всех погодных информеров
	WeatherInformerContainer_Show_Method Show;

	// Отображение данных прогноза погоды в файл
	// в формате JSON
	WeatherInformerContainer_JsonOutput_Method JsonOutput;

	// Отображение данных прогноза погоды в файл
	// в формате XML
	WeatherInformerContainer_XmlOutput_Method XmlOutput;
} WEATHER_INFORMER_CONTAINER, *PWEATHER_INFORMER_CONTAINER;

// Конструктор объектов класса WeatherInformerContainer
PWEATHER_INFORMER_CONTAINER WeatherInformerContainer_Constructor(char * cityName, int scale);

// Деструктор объектов класса WeatherInformerContainer
void WeatherInformerContainer_Destructor(PWEATHER_INFORMER_CONTAINER _this);

// Подключение к источнику и извлечение данных прогноза погоды 
// для всех погодных информеров
void WeatherInformerContainer_GetData(void * _this);

// Анализ данных полученных от источника в формате JSON
// для всех погодных информеров
void WeatherInformerContainer_ParseData(void * _this);

// Создание усреднённого прогноза погоды
void WeatherInformerContainer_CreateTotalData(void * _this);

// Отображение данных прогноза погоды на экран
// для всех погодных информеров
void WeatherInformerContainer_Show(void * _this);

// Отображение данных прогноза погоды в файл
// в формате JSON
void WeatherInformerContainer_JsonOutput(void * _this);

// Отображение данных прогноза погоды в файл
// в формате XML
void WeatherInformerContainer_XmlOutput(void * _this);

#endif