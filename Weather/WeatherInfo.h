/**************************************************************************************
   
	������ : Weather

	������ : WeatherInfo.h

	�������� : ������ �������� ����������� ����� � ������� ��� ������ WeatherInfo,
	           ������� �������� � ���� ���������� � �������� ������ �� ������
			   ��� ������������ ������.

**************************************************************************************/

#ifndef WEATHER_INFO_H
#define WEATHER_INFO_H

// ���� ���� ����������
#define C_SCALE 0
#define F_SCALE 1

//  ���������� �������� ��������� ������
#define CITY_NANE_SIZE 64
#define WIND_DIR_SIZE 5


// ����������� ���� ������ ��� ������ Show
// ������ WeatherInfo
typedef void (*WeatherInfo_Show_Method)(void*, int);

// ����������� ��������� ����� WeatherInfo
typedef struct _weatherInfo
{
	// �������� ������
	char cityName[CITY_NANE_SIZE];

	// ���������� � ������������ �����������
	// �� �������
	int minTempC;
	int maxTempC;

	// ���������� � ������������ �����������
	// �� ����������
	int minTempF;
	int maxTempF;
	
	//  ����������� ��������
	int preasure;

	// ���������
	int humidity;

	// �������� �����
	int windSpeed;

	// ����������� �����
	char windDirection[WIND_DIR_SIZE];

	// ����������
	int cloudiness;

	// ������
	int precipitation;
	
	// ����� Show - ����������� ������ ��������
	// ������ �� �����
	WeatherInfo_Show_Method Show;
} WEATHER_INFO, *PWEATHER_INFO;

// ����������� ������ WeatherInfo
PWEATHER_INFO WeatherInfo_Constructor(void);

// ���������� ������ WeatherInfo
void WeatherInfo_Destructor(PWEATHER_INFO _this);

// ����������� ������ �������� ������ �� �����
void WeatherInfo_Show(void * _this, int scale);

#endif