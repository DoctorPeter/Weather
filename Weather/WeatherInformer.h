/**************************************************************************************
   
	Проект : Weather

	Модуль : WeatherInformer.h

	Описание : Модуль содержит определения типов и функций для класса WeatherInformer,
	           который позволяет извлекать прогноз погоды на завтра для определённого 
			   города из определённого источника.

**************************************************************************************/

#ifndef WEATHER_INFORMER_H
#define WEATHER_INFORMER_H

#include "json.h"

#include "WeatherInfo.h"

//  Определние размеров строковых данных
#define REQUEST_SIZE 256
#define SOURCE_NAME_SIZE 128

// Определение типов данных для методов класса WeatherInformer.
// Повторяющиеся определения введены для поддержания единой 
// концепции проектирования и для повышения читабельности кода.
typedef int (*WeatherInformer_DataWriter_Method)(char *, size_t, size_t, char **);
typedef int (*WeatherInformer_GetData_Method)(void *);
typedef void (*WeatherInformer_SetCityName_Method)(void*, char *);
typedef void (*WeatherInformer_SetSourceName_Method)(void*, char *);
typedef void (*WeatherInformer_Show_Method)(void*);
typedef void (*WeatherInformer_CelsiusToFahrenheit_Method)(void *);
typedef void (*WeatherInformer_FahrenheitToCelsius_Method)(void *);
typedef void (*WeatherInformer_CreateHttpRequest_Method)(void *);
typedef void (*WeatherInformer_ParseJsonData_Method)(void *);

// Определение структуры класа WeatherInformer
typedef struct _WeatherInformer
{
   // Источник прогноза погоды
   char sourceName[SOURCE_NAME_SIZE];
   
   // Запрос на получение данных
   // прогноза погоды
   char httpReq[REQUEST_SIZE];

   // Буфер для хранения данных
   // прогноза погоды
   char * buffer;

   // Размер буфера
   int bufSize;

   // Шкала температур
   int scale;

   // Код ошибки
   int error;
   
   // Информация о прогнозе погоды
   // после анализа данных из буфера
   PWEATHER_INFO wInfo;

   // Определение методов

   // Запись данных из источника в буфер
   WeatherInformer_DataWriter_Method DataWriter;

   // Подключение к источнику и извлечение данных прогноза погоды
   WeatherInformer_GetData_Method GetData;

   // Установка навания города
   WeatherInformer_SetCityName_Method SetCityName;

   // Установка имени источника
   WeatherInformer_SetSourceName_Method SetSourceName;

   // Отображение данных прогноза погоды на экран
   WeatherInformer_Show_Method Show;

   // Преобразование данных температуры из шкалы Цельсия в шкалу Фаренгейта
   WeatherInformer_CelsiusToFahrenheit_Method CelsiusToFahrenheit;
   
   // Преобразование данных температуры из шкалы Фаренгейта в шкалу Цельсия
   WeatherInformer_FahrenheitToCelsius_Method FahrenheitToCelsius;

   // Формирование запроса к источнику данных
   WeatherInformer_CreateHttpRequest_Method CreateHttpRequest;

   // Анализ данных полученных от источника в формате JSON
   WeatherInformer_ParseJsonData_Method ParseJsonData;
} WEATHER_INFORMER, *PWEATHER_INFORMER;

// Конструктор объектов класса WeatherInformer
PWEATHER_INFORMER WeatherInformer_Constructor(int scale, char * cityName,  char *sourceName, WeatherInformer_CreateHttpRequest_Method CreateHttpRequest, WeatherInformer_ParseJsonData_Method ParseJsonData);

// Деструктор объектов класса WeatherInformer
void WeatherInformer_Denstructor(PWEATHER_INFORMER _this);

 // Запись данных из источника в буфер
int WeatherInformer_DataWriter(char *data, size_t size, size_t nMemb, char ** buffer);

// Подключение к источнику и извлечение данных прогноза погоды
int WeatherInformer_GetData(void * _this);

// Установка навания города
void WeatherInformer_SetCityName(void* _this, char *cityName);

// Установка имени источника
void WeatherInformer_SetSourceName(void* _this, char *sourceName);

// Отображение данных прогноза погоды на экран
void WeatherInformer_Show(void* _this);

// Преобразование данных температуры из шкалы Цельсия в шкалу Фаренгейта
void WeatherInformer_CelsiusToFahrenheit(void *_this);

// Преобразование данных температуры из шкалы Фаренгейта в шкалу Цельсия
void WeatherInformer_FahrenheitToCelsius(void *_this);

// Формирование запроса к источнику данных от worldweatheronline
void WeatherInformer_CreateHttpRequest_WWO(void *_this);

// Анализ данных полученных от источника в формате JSON от worldweatheronline
void WeatherInformer_ParseJsonData_WWO(void *_this);

// Формирование запроса к источнику данных от open-weather.ru
void WeatherInformer_CreateHttpRequest_OW(void *_this);

// Анализ данных полученных от источника в формате JSON от open-weather.ru
void WeatherInformer_ParseJsonData_OW(void *_this);

// Формирование запроса к источнику данных от pogoda.bilets.org
void WeatherInformer_CreateHttpRequest_PBO(void *_this);

// Анализ данных полученных от источника в формате JSON от pogoda.bilets.org
void WeatherInformer_ParseJsonData_PBO(void *_this);

// Формирование запроса к источнику данных от api.wunderground.com
void WeatherInformer_CreateHttpRequest_WG(void *_this);

// Анализ данных полученных от источника в формате JSON от api.wunderground.com
void WeatherInformer_ParseJsonData_WG(void *_this);

// Формирование запроса к источнику данных от www.google.com
void WeatherInformer_CreateHttpRequest_GA(void *_this);

// Анализ данных полученных от источника в формате JSON от www.google.com
void WeatherInformer_ParseJsonData_GA(void *_this);

// Проверка значения опредеённой записи после 
// синтаксического анализа JSON
void CheckJSONValue(json_object * jobj,
					char * key,
					PWEATHER_INFO wInfo,
					char * minTempCName,
					char * maxTempCName,
					char * minTempFName,
					char * maxTempFName,	
					char * preasurename,
					char * humidityName,
					char * windSpeedName,
					char * windDirectionName,
					char * cloudinessName,
					char *precipitationName);

// Синтаксический анализ JSON масива
void PasreJSONArray(json_object *jobj, char *key,
					PWEATHER_INFO wInfo,
					char * minTempCName,
					char * maxTempCName,
					char * minTempFName,
					char * maxTempFName,	
					char * preasurename,
					char * humidityName,
					char * windSpeedName,
					char * windDirectionName,
					char * cloudinessName,
					char *precipitationName);

// Синтаксический анализ JSON данных
void JSONParse(json_object * jobj,
				PWEATHER_INFO wInfo,
	            char * minTempCName,
				char * maxTempCName,
				char * minTempFName,
				char * maxTempFName,	
				char * preasureName,
				char * humidityName,
				char * windSpeedName,
				char * windDirectionName,
				char * cloudinessName,
				char * precipitationName);
#endif