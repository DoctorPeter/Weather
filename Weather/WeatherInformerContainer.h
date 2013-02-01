/**************************************************************************************
   
	������ : Weather

	������ : WeatherInformerContainer.h

	�������� : ������ �������� ����������� ����� � ������� ��� ������ 
	           WeatherInformerContainer, ������� ��������� ������������ ��������
			   � 5-� ����������� ������ � ����������� ���������� �������.

**************************************************************************************/

#ifndef WEATHER_INFORMER_CONTAINER_H
#define WEATHER_INFORMER_CONTAINER_H

#include "WeatherInformer.h"

//  ���������� �������� ��������� ������
#define WEATHER_INFORMER_CONTAINER_SIZE 5
#define FILE_NAME_SIZE 256

// ����������� ����� ������ ��� ������� ������ WeatherInformerContainer.
// ������������� ����������� ������� ��� ����������� ������ 
// ��������� �������������� � ��� ��������� ������������� ����.
typedef void (*WeatherInformerContainer_GetData_Method)(void *);
typedef void (*WeatherInformerContainer_ParseData_Method)(void *);
typedef void (*WeatherInformerContainer_CreateTotalData_Method)(void *);
typedef void (*WeatherInformerContainer_Show_Method)(void *);
typedef void (*WeatherInformerContainer_JsonOutput_Method)(void *);
typedef void (*WeatherInformerContainer_XmlOutput_Method)(void *);

// ����������� ��������� ����� WeatherInformerContainer
typedef struct _WeatherInformerContainer
{
	// ������ �������� ����������
	PWEATHER_INFORMER container[WEATHER_INFORMER_CONTAINER_SIZE + 1];
	
	// ����������� �������

	// ����������� � ��������� � ���������� ������ �������� ������ 
	// ��� ���� �������� ����������
	WeatherInformerContainer_GetData_Method GetData;
	
	// ������ ������ ���������� �� ��������� � ������� JSON
	// ��� ���� �������� ����������
	WeatherInformerContainer_ParseData_Method ParseData;

	// �������� ����������� �������� ������
	WeatherInformerContainer_CreateTotalData_Method CreateTotalData;

	// ����������� ������ �������� ������ �� �����
	// ��� ���� �������� ����������
	WeatherInformerContainer_Show_Method Show;

	// ����������� ������ �������� ������ � ����
	// � ������� JSON
	WeatherInformerContainer_JsonOutput_Method JsonOutput;

	// ����������� ������ �������� ������ � ����
	// � ������� XML
	WeatherInformerContainer_XmlOutput_Method XmlOutput;
} WEATHER_INFORMER_CONTAINER, *PWEATHER_INFORMER_CONTAINER;

// ����������� �������� ������ WeatherInformerContainer
PWEATHER_INFORMER_CONTAINER WeatherInformerContainer_Constructor(char * cityName, int scale);

// ���������� �������� ������ WeatherInformerContainer
void WeatherInformerContainer_Destructor(PWEATHER_INFORMER_CONTAINER _this);

// ����������� � ��������� � ���������� ������ �������� ������ 
// ��� ���� �������� ����������
void WeatherInformerContainer_GetData(void * _this);

// ������ ������ ���������� �� ��������� � ������� JSON
// ��� ���� �������� ����������
void WeatherInformerContainer_ParseData(void * _this);

// �������� ����������� �������� ������
void WeatherInformerContainer_CreateTotalData(void * _this);

// ����������� ������ �������� ������ �� �����
// ��� ���� �������� ����������
void WeatherInformerContainer_Show(void * _this);

// ����������� ������ �������� ������ � ����
// � ������� JSON
void WeatherInformerContainer_JsonOutput(void * _this);

// ����������� ������ �������� ������ � ����
// � ������� XML
void WeatherInformerContainer_XmlOutput(void * _this);

#endif