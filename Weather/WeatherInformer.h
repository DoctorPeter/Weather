/**************************************************************************************
   
	������ : Weather

	������ : WeatherInformer.h

	�������� : ������ �������� ����������� ����� � ������� ��� ������ WeatherInformer,
	           ������� ��������� ��������� ������� ������ �� ������ ��� ������������ 
			   ������ �� ������������ ���������.

**************************************************************************************/

#ifndef WEATHER_INFORMER_H
#define WEATHER_INFORMER_H

#include "json.h"

#include "WeatherInfo.h"

//  ���������� �������� ��������� ������
#define REQUEST_SIZE 256
#define SOURCE_NAME_SIZE 128

// ����������� ����� ������ ��� ������� ������ WeatherInformer.
// ������������� ����������� ������� ��� ����������� ������ 
// ��������� �������������� � ��� ��������� ������������� ����.
typedef int (*WeatherInformer_DataWriter_Method)(char *, size_t, size_t, char **);
typedef int (*WeatherInformer_GetData_Method)(void *);
typedef void (*WeatherInformer_SetCityName_Method)(void*, char *);
typedef void (*WeatherInformer_SetSourceName_Method)(void*, char *);
typedef void (*WeatherInformer_Show_Method)(void*);
typedef void (*WeatherInformer_CelsiusToFahrenheit_Method)(void *);
typedef void (*WeatherInformer_FahrenheitToCelsius_Method)(void *);
typedef void (*WeatherInformer_CreateHttpRequest_Method)(void *);
typedef void (*WeatherInformer_ParseJsonData_Method)(void *);

// ����������� ��������� ����� WeatherInformer
typedef struct _WeatherInformer
{
   // �������� �������� ������
   char sourceName[SOURCE_NAME_SIZE];
   
   // ������ �� ��������� ������
   // �������� ������
   char httpReq[REQUEST_SIZE];

   // ����� ��� �������� ������
   // �������� ������
   char * buffer;

   // ������ ������
   int bufSize;

   // ����� ����������
   int scale;

   // ��� ������
   int error;
   
   // ���������� � �������� ������
   // ����� ������� ������ �� ������
   PWEATHER_INFO wInfo;

   // ����������� �������

   // ������ ������ �� ��������� � �����
   WeatherInformer_DataWriter_Method DataWriter;

   // ����������� � ��������� � ���������� ������ �������� ������
   WeatherInformer_GetData_Method GetData;

   // ��������� ������� ������
   WeatherInformer_SetCityName_Method SetCityName;

   // ��������� ����� ���������
   WeatherInformer_SetSourceName_Method SetSourceName;

   // ����������� ������ �������� ������ �� �����
   WeatherInformer_Show_Method Show;

   // �������������� ������ ����������� �� ����� ������� � ����� ����������
   WeatherInformer_CelsiusToFahrenheit_Method CelsiusToFahrenheit;
   
   // �������������� ������ ����������� �� ����� ���������� � ����� �������
   WeatherInformer_FahrenheitToCelsius_Method FahrenheitToCelsius;

   // ������������ ������� � ��������� ������
   WeatherInformer_CreateHttpRequest_Method CreateHttpRequest;

   // ������ ������ ���������� �� ��������� � ������� JSON
   WeatherInformer_ParseJsonData_Method ParseJsonData;
} WEATHER_INFORMER, *PWEATHER_INFORMER;

// ����������� �������� ������ WeatherInformer
PWEATHER_INFORMER WeatherInformer_Constructor(int scale, char * cityName,  char *sourceName, WeatherInformer_CreateHttpRequest_Method CreateHttpRequest, WeatherInformer_ParseJsonData_Method ParseJsonData);

// ���������� �������� ������ WeatherInformer
void WeatherInformer_Denstructor(PWEATHER_INFORMER _this);

 // ������ ������ �� ��������� � �����
int WeatherInformer_DataWriter(char *data, size_t size, size_t nMemb, char ** buffer);

// ����������� � ��������� � ���������� ������ �������� ������
int WeatherInformer_GetData(void * _this);

// ��������� ������� ������
void WeatherInformer_SetCityName(void* _this, char *cityName);

// ��������� ����� ���������
void WeatherInformer_SetSourceName(void* _this, char *sourceName);

// ����������� ������ �������� ������ �� �����
void WeatherInformer_Show(void* _this);

// �������������� ������ ����������� �� ����� ������� � ����� ����������
void WeatherInformer_CelsiusToFahrenheit(void *_this);

// �������������� ������ ����������� �� ����� ���������� � ����� �������
void WeatherInformer_FahrenheitToCelsius(void *_this);

// ������������ ������� � ��������� ������ �� worldweatheronline
void WeatherInformer_CreateHttpRequest_WWO(void *_this);

// ������ ������ ���������� �� ��������� � ������� JSON �� worldweatheronline
void WeatherInformer_ParseJsonData_WWO(void *_this);

// ������������ ������� � ��������� ������ �� open-weather.ru
void WeatherInformer_CreateHttpRequest_OW(void *_this);

// ������ ������ ���������� �� ��������� � ������� JSON �� open-weather.ru
void WeatherInformer_ParseJsonData_OW(void *_this);

// ������������ ������� � ��������� ������ �� pogoda.bilets.org
void WeatherInformer_CreateHttpRequest_PBO(void *_this);

// ������ ������ ���������� �� ��������� � ������� JSON �� pogoda.bilets.org
void WeatherInformer_ParseJsonData_PBO(void *_this);

// ������������ ������� � ��������� ������ �� api.wunderground.com
void WeatherInformer_CreateHttpRequest_WG(void *_this);

// ������ ������ ���������� �� ��������� � ������� JSON �� api.wunderground.com
void WeatherInformer_ParseJsonData_WG(void *_this);

// ������������ ������� � ��������� ������ �� www.google.com
void WeatherInformer_CreateHttpRequest_GA(void *_this);

// ������ ������ ���������� �� ��������� � ������� JSON �� www.google.com
void WeatherInformer_ParseJsonData_GA(void *_this);

// �������� �������� ���������� ������ ����� 
// ��������������� ������� JSON
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

// �������������� ������ JSON ������
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

// �������������� ������ JSON ������
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