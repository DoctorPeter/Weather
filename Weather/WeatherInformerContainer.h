/**************************************************************************************

	Project : Weather

	Module : WeatherInformerContainer.h

	Description : This module contains type definitions and function declarations for the
				  WeatherInformerContainer class, which allows working simultaneously with
				  5 weather sources and forming an averaged forecast.

**************************************************************************************/

#ifndef WEATHER_INFORMER_CONTAINER_H
#define WEATHER_INFORMER_CONTAINER_H

#include "WeatherInformer.h"

// Definition of string data sizes
#define WEATHER_INFORMER_CONTAINER_SIZE 5
#define FILE_NAME_SIZE 256

// Definition of data types for WeatherInformerContainer class methods.
// Repeated definitions are introduced to maintain a unified 
// design concept and to enhance code readability.
typedef void(*WeatherInformerContainer_GetData_Method)(void *);
typedef void(*WeatherInformerContainer_ParseData_Method)(void *);
typedef void(*WeatherInformerContainer_CreateTotalData_Method)(void *);
typedef void(*WeatherInformerContainer_Show_Method)(void *);
typedef void(*WeatherInformerContainer_JsonOutput_Method)(void *);
typedef void(*WeatherInformerContainer_XmlOutput_Method)(void *);

// Structure definition of the WeatherInformerContainer class
typedef struct _WeatherInformerContainer
{
	// Array of weather informers
	PWEATHER_INFORMER container[WEATHER_INFORMER_CONTAINER_SIZE + 1];

	// Method definitions

	// Connect to the source and extract weather forecast data 
	// for all weather informers
	WeatherInformerContainer_GetData_Method GetData;

	// Parse JSON format data obtained from the source 
	// for all weather informers
	WeatherInformerContainer_ParseData_Method ParseData;

	// Create an averaged weather forecast
	WeatherInformerContainer_CreateTotalData_Method CreateTotalData;

	// Display weather forecast data on the screen 
	// for all weather informers
	WeatherInformerContainer_Show_Method Show;

	// Display weather forecast data in a file 
	// in JSON format
	WeatherInformerContainer_JsonOutput_Method JsonOutput;

	// Display weather forecast data in a file 
	// in XML format
	WeatherInformerContainer_XmlOutput_Method XmlOutput;
} WEATHER_INFORMER_CONTAINER, *PWEATHER_INFORMER_CONTAINER;

// Constructor for WeatherInformerContainer class objects
PWEATHER_INFORMER_CONTAINER WeatherInformerContainer_Constructor(char * cityName, int scale);

// Destructor for WeatherInformerContainer class objects
void WeatherInformerContainer_Destructor(PWEATHER_INFORMER_CONTAINER _this);

// Connect to the source and extract weather forecast data 
// for all weather informers
void WeatherInformerContainer_GetData(void * _this);

// Parse JSON format data obtained from the source 
// for all weather informers
void WeatherInformerContainer_ParseData(void * _this);

// Create an averaged weather forecast
void WeatherInformerContainer_CreateTotalData(void * _this);

// Display weather forecast data on the screen 
// for all weather informers
void WeatherInformerContainer_Show(void * _this);

// Display weather forecast data in a file 
// in JSON format
void WeatherInformerContainer_JsonOutput(void * _this);

// Display weather forecast data in a file 
// in XML format
void WeatherInformerContainer_XmlOutput(void * _this);

#endif
