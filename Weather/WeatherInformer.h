/**************************************************************************************

	Project : Weather

	Module : WeatherInformer.h

	Description : This module contains type definitions and function declarations for the WeatherInformer class,
				  which allows extracting weather forecast for tomorrow for a specific
				  city from a specific source.

**************************************************************************************/

#ifndef WEATHER_INFORMER_H
#define WEATHER_INFORMER_H

#include "json.h"

#include "WeatherInfo.h"

// Definition of string data sizes
#define REQUEST_SIZE 256
#define SOURCE_NAME_SIZE 128

// Definition of data types for WeatherInformer class methods.
// Repeated definitions are introduced to maintain a unified 
// design concept and to enhance code readability.
typedef int(*WeatherInformer_DataWriter_Method)(char *, size_t, size_t, char **);
typedef int(*WeatherInformer_GetData_Method)(void *);
typedef void(*WeatherInformer_SetCityName_Method)(void*, char *);
typedef void(*WeatherInformer_SetSourceName_Method)(void*, char *);
typedef void(*WeatherInformer_Show_Method)(void*);
typedef void(*WeatherInformer_CelsiusToFahrenheit_Method)(void *);
typedef void(*WeatherInformer_FahrenheitToCelsius_Method)(void *);
typedef void(*WeatherInformer_CreateHttpRequest_Method)(void *);
typedef void(*WeatherInformer_ParseJsonData_Method)(void *);

// Structure definition of the WeatherInformer class
typedef struct _WeatherInformer
{
	// Weather forecast source
	char sourceName[SOURCE_NAME_SIZE];

	// Request to obtain weather forecast data
	char httpReq[REQUEST_SIZE];

	// Buffer for storing weather forecast data
	char * buffer;

	// Buffer size
	int bufSize;

	// Temperature scale
	int scale;

	// Error code
	int error;

	// Weather forecast information
	// after analyzing data from the buffer
	PWEATHER_INFO wInfo;

	// Method definitions

	// Write data from the source to the buffer
	WeatherInformer_DataWriter_Method DataWriter;

	// Connect to the source and extract weather forecast data
	WeatherInformer_GetData_Method GetData;

	// Set city name
	WeatherInformer_SetCityName_Method SetCityName;

	// Set source name
	WeatherInformer_SetSourceName_Method SetSourceName;

	// Display weather forecast data on the screen
	WeatherInformer_Show_Method Show;

	// Convert temperature data from Celsius scale to Fahrenheit scale
	WeatherInformer_CelsiusToFahrenheit_Method CelsiusToFahrenheit;

	// Convert temperature data from Fahrenheit scale to Celsius scale
	WeatherInformer_FahrenheitToCelsius_Method FahrenheitToCelsius;

	// Form HTTP request to the data source
	WeatherInformer_CreateHttpRequest_Method CreateHttpRequest;

	// Parse JSON data obtained from the source
	WeatherInformer_ParseJsonData_Method ParseJsonData;
} WEATHER_INFORMER, *PWEATHER_INFORMER;

// Constructor for WeatherInformer class objects
PWEATHER_INFORMER WeatherInformer_Constructor(int scale, char * cityName, char *sourceName, WeatherInformer_CreateHttpRequest_Method CreateHttpRequest, WeatherInformer_ParseJsonData_Method ParseJsonData);

// Destructor for WeatherInformer class objects
void WeatherInformer_Denstructor(PWEATHER_INFORMER _this);

// Write data from the source to the buffer
int WeatherInformer_DataWriter(char *data, size_t size, size_t nMemb, char ** buffer);

// Connect to the source and extract weather forecast data
int WeatherInformer_GetData(void * _this);

// Set city name
void WeatherInformer_SetCityName(void* _this, char *cityName);

// Set source name
void WeatherInformer_SetSourceName(void* _this, char *sourceName);

// Display weather forecast data on the screen
void WeatherInformer_Show(void* _this);

// Convert temperature data from Celsius scale to Fahrenheit scale
void WeatherInformer_CelsiusToFahrenheit(void *_this);

// Convert temperature data from Fahrenheit scale to Celsius scale
void WeatherInformer_FahrenheitToCelsius(void *_this);

// Form HTTP request to the data source from worldweatheronline
void WeatherInformer_CreateHttpRequest_WWO(void *_this);

// Parse JSON data obtained from the source from worldweatheronline
void WeatherInformer_ParseJsonData_WWO(void *_this);

// Form HTTP request to the data source from open-weather.ru
void WeatherInformer_CreateHttpRequest_OW(void *_this);

// Parse JSON data obtained from the source from open-weather.ru
void WeatherInformer_ParseJsonData_OW(void *_this);

// Form HTTP request to the data source from pogoda.bilets.org
void WeatherInformer_CreateHttpRequest_PBO(void *_this);

// Parse JSON data obtained from the source from pogoda.bilets.org
void WeatherInformer_ParseJsonData_PBO(void *_this);

// Form HTTP request to the data source from api.wunderground.com
void WeatherInformer_CreateHttpRequest_WG(void *_this);

// Parse JSON data obtained from the source from api.wunderground.com
void WeatherInformer_ParseJsonData_WG(void *_this);

// Form HTTP request to the data source from www.google.com
void WeatherInformer_CreateHttpRequest_GA(void *_this);

// Parse JSON data obtained from the source from www.google.com
void WeatherInformer_ParseJsonData_GA(void *_this);

// Check the value of a specific entry after 
// JSON syntax analysis
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

// JSON array syntax analysis
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

// JSON data syntax analysis
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
