/**************************************************************************************
   
	������ : Weather

	������ : WeatherInformer.cpp

	�������� : ������ �������� ���������� ������� ������ WeatherInformer.

**************************************************************************************/

#include "stdafx.h"

#include "curl/curl.h"
#include <stdlib.h>

#include "WeatherInformer.h"

// ��������� ���������� ���������� 
// ��� ����������� ������� ������
int bufSize = 0;

/**************************************************************************************
   
    ����� :  WeatherInformer_Constructor
	
	��������� : 
	            scale - ����� ����������
				cityName - �������� ������
				sourceName - ��� ��������� ������
				CreateHttpRequest - ����� ��� ������������ �������
				ParseJsonData - ����� ������� ������ �� ���������

	���������� : ��������� �� ������

	�������� : ����������� �������� ������ WeatherInformer.

**************************************************************************************/
PWEATHER_INFORMER WeatherInformer_Constructor(int scale, char * cityName,  char *sourceName, WeatherInformer_CreateHttpRequest_Method CreateHttpRequest, WeatherInformer_ParseJsonData_Method ParseJsonData)
{

	// ��������� ������ ��� ������
	PWEATHER_INFORMER _this = (PWEATHER_INFORMER)malloc(sizeof(WEATHER_INFORMER));
	
	// ������������� �����
	_this -> wInfo = WeatherInfo_Constructor();
	_this -> buffer = NULL;
	_this -> bufSize = 0;
	_this -> scale = scale;
	_this -> error = 0;
	
	// ������������� �������
	_this -> DataWriter = WeatherInformer_DataWriter;
	_this -> GetData = WeatherInformer_GetData;
	_this -> SetCityName = WeatherInformer_SetCityName;
	_this -> SetSourceName = WeatherInformer_SetSourceName;
	_this -> Show = WeatherInformer_Show;
	_this -> CelsiusToFahrenheit = WeatherInformer_CelsiusToFahrenheit;
    _this -> FahrenheitToCelsius = WeatherInformer_FahrenheitToCelsius;
	_this -> CreateHttpRequest = CreateHttpRequest;
	_this -> ParseJsonData = ParseJsonData;

	// ��������� �������� �������� ������ � ����� ���������
	_this -> SetCityName(_this, cityName);
	_this -> SetSourceName(_this, sourceName);

	return _this;
}

/**************************************************************************************
   
    ����� :  WeatherInformer_Denstructor
	
	��������� : 
	            _this - ��������� �� ������ ������ WeatherInformer
				
	�������� : ���������� �������� ������ WeatherInformer.

**************************************************************************************/
void WeatherInformer_Denstructor(PWEATHER_INFORMER _this)
{
	if (_this)
	{
		// ������� ������ WeatherInfo
		WeatherInfo_Destructor(_this -> wInfo);

		// ������� �����
		if (_this -> buffer) free(_this -> buffer);

		// ������� ����
		free(_this);
	}
}

/**************************************************************************************
   
    ����� :  WeatherInformer_DataWriter
	
	��������� : 
				data - ������ ���������� �� ���������
				size - ������ ������
				nMemb - ���������� ��������� ������
				buffer - ����� ��� ��������� ������

	���������� : ���������� ��������� ����

	�������� : ������ ������ �� ��������� � �����

**************************************************************************************/
int WeatherInformer_DataWriter(char *data, size_t size, size_t nMemb, char ** buffer)
{
  if (*buffer == NULL)
  {
	  // ���� ����� ������, �� �������� ��� ���� ������
	  *buffer = (char *)malloc(size*nMemb);
	  bufSize = 0;
  }
  else
  {
	  // ����� ��������� �����
	  *buffer = (char*)realloc(*buffer, bufSize + size*nMemb); 
  }

  // ����������� ������ � �����
  memcpy(*buffer + bufSize, data, size*nMemb);

  // ��������� �������� ���������� ����������� ������
  bufSize += size*nMemb;
  
  // ������� �������� ���������� ����������� ������
  return bufSize;
}

/**************************************************************************************
   
    ����� : WeatherInformer_GetData
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	���������� : ��� ������

	�������� : ����������� � ��������� � ���������� ������ �������� ������.
	           ���������� ���������� ���������� CURL.

**************************************************************************************/
int WeatherInformer_GetData(void * _this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	
	char errorBuffer[CURL_ERROR_SIZE];

	CURL *curl;
    CURLcode result = CURLE_RECV_ERROR;
    
	// �������������� CURL ������
	curl = curl_easy_init();
    
    if (curl)
    {
       // ����������� CURL �����������
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
		curl_easy_setopt(curl, CURLOPT_URL, wInformer -> httpReq);
	    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wInformer -> DataWriter);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &(wInformer -> buffer));

		// ��������� ������
	    result = curl_easy_perform(curl);
      
      if (result == CURLE_OK)
	  {
		  // ���� ������ �������� ���������� � ����� ������ ����
		  wInformer -> buffer = (char*)realloc(wInformer -> buffer, bufSize + 1); 
		  bufSize += 1;
		  wInformer -> buffer[bufSize - 1] = '\0';
		  wInformer -> bufSize = bufSize;
	  }
	}
  
  // ������� CURL ������
  curl_easy_cleanup(curl);

  // ���������� ��� ������
  wInformer -> error = (int)result;
  return wInformer -> error;
}

/**************************************************************************************
   
    ����� : WeatherInformer_SetCityName
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� : ��������� ������� ������

**************************************************************************************/
void WeatherInformer_SetCityName(void* _this, char *cityName)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;

	strcpy(wInformer -> wInfo -> cityName, cityName);
}

/**************************************************************************************
   
    ����� : WeatherInformer_SetSourceName
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� : ��������� ����� ���������

**************************************************************************************/
void WeatherInformer_SetSourceName(void* _this, char *sourceName)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;

	strcpy(wInformer -> sourceName, sourceName);
}

/**************************************************************************************
   
    ����� : WeatherInformer_Show
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� : ����������� ������ �������� ������ �� �����

**************************************************************************************/
void WeatherInformer_Show(void* _this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;

	if(wInformer -> error)
	{
		printf("------------------------------------------------------------------------\n");
		printf("\n No weather forecast data from source: %s\n", wInformer -> sourceName);
		printf("------------------------------------------------------------------------\n\n");
	}
	else
	{
		printf("\n-------Weather forecast for tomorrow: -------\n");
		printf("  Source:  %s\n", wInformer -> sourceName);
		wInformer -> wInfo -> Show(wInformer -> wInfo, wInformer -> scale);
	}
}

/**************************************************************************************
   
    ����� : WeatherInformer_CelsiusToFahrenheit
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� : �������������� ������ ����������� �� ����� ������� � ����� ����������

**************************************************************************************/
void WeatherInformer_CelsiusToFahrenheit(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	wInformer -> wInfo -> minTempF = (wInformer -> wInfo -> minTempC) * (5.0/9.0) + 32.0;
	wInformer -> wInfo -> maxTempF = (wInformer -> wInfo -> maxTempC) * (5.0/9.0) + 32.0;
}

/**************************************************************************************
   
    ����� : WeatherInformer_FahrenheitToCelsius
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� : �������������� ������ ����������� �� ����� ���������� � ����� �������

**************************************************************************************/
void WeatherInformer_FahrenheitToCelsius(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	wInformer -> wInfo -> minTempC = ((wInformer -> wInfo -> minTempF) - 32.0) * (5.0/9.0);
	wInformer -> wInfo -> maxTempC = ((wInformer -> wInfo -> maxTempF) - 32.0) * (5.0/9.0);
}

/**************************************************************************************
   
    ����� : WeatherInformer_CreateHttpRequest_WWO
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� :  ������������ ������� � ��������� ������ �� worldweatheronline

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_WWO(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer -> httpReq, "http://free.worldweatheronline.com/feed/weather.ashx?format=json&date=tomorrow&key=52a9e8e499131425132901&q=");
	strcat(wInformer -> httpReq, wInformer -> wInfo -> cityName);
}

/**************************************************************************************
   
    ����� : WeatherInformer_ParseJsonData_WWO
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� : ������ ������ ���������� �� ��������� � ������� JSON �� worldweatheronline

**************************************************************************************/
void WeatherInformer_ParseJsonData_WWO(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	
	if (wInformer -> error) return;

	char * errorData = "{ \"data\": { \"error\": [ {\"msg\": \"Unable to find any matching weather location to the query submitted!\" } ] }}";
	
	// �������� ��������� ������
	if (strcmp (wInformer -> buffer, errorData) == 0)
	{
		wInformer -> error = CURLE_RECV_ERROR;
		return;
	}

	// ������ JSON ������
	json_object * jobj = json_tokener_parse(wInformer -> buffer);
	JSONParse(jobj, wInformer -> wInfo, "tempMinC", "tempMaxC", "tempMinF", "tempMaxF", "pressure", "humidity", "windspeedKmph", "winddirection", "cloudcover", "precipMM");

	// ��� ������������� ��������������� �������� �����������
	if (wInformer -> scale == C_SCALE)
	{
		if ((wInformer -> wInfo -> minTempC != 0) && (wInformer -> wInfo -> maxTempC != 0) &&
			(wInformer -> wInfo -> minTempF == 0) && (wInformer -> wInfo -> maxTempF == 0))
			   wInformer -> CelsiusToFahrenheit(wInformer);
	}
	else
	if (wInformer -> scale == F_SCALE)
	{
		if ((wInformer -> wInfo -> minTempF != 0) && (wInformer -> wInfo -> maxTempF != 0) &&
			(wInformer -> wInfo -> minTempC == 0) && (wInformer -> wInfo -> maxTempC == 0))
			wInformer -> FahrenheitToCelsius(wInformer);
	}
}


/**************************************************************************************
   
    ����� : WeatherInformer_CreateHttpRequest_OW
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� :  ������������ ������� � ��������� ������ �� open-weather.ru

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_OW(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer -> httpReq, "http://open-weather.ru/api/weather/");
	strcat(wInformer -> httpReq, wInformer -> wInfo -> cityName);
	strcat(wInformer -> httpReq, "/?key=063e5ee07278d771043590a169bc0983645ff814f1ed39339d71f2c7");
}

/**************************************************************************************
   
    ����� : WeatherInformer_ParseJsonData_OW
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� : ������ ������ ���������� �� ��������� � ������� JSON �� open-weather.ru

**************************************************************************************/
void WeatherInformer_ParseJsonData_OW(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	
	if (wInformer -> error) return;

	char * errorData = "Not Found";
	// �������� ��������� ������
	if (strcmp (wInformer -> buffer, errorData) == 0)
	{
		wInformer -> error = CURLE_RECV_ERROR;
		return;
	}
	// ������ JSON ������
	json_object * jobj = json_tokener_parse(wInformer -> buffer);
	JSONParse(jobj, wInformer -> wInfo, "minimal_temperature", "maximal_temperature", "minimal_temperature_f", "maximal_temperature_f", "pressure", "humidity", "wind_speed", "wind_direction", "cloudcover", "precipitation");
	// ��� ������������� ��������������� �������� �����������
	if (wInformer -> scale == C_SCALE)
	{
		if ((wInformer -> wInfo -> minTempC != 0) && (wInformer -> wInfo -> maxTempC != 0) &&
			(wInformer -> wInfo -> minTempF == 0) && (wInformer -> wInfo -> maxTempF == 0))
			   wInformer -> CelsiusToFahrenheit(wInformer);
	}
	else
	if (wInformer -> scale == F_SCALE)
	{
		if ((wInformer -> wInfo -> minTempF != 0) && (wInformer -> wInfo -> maxTempF != 0) &&
			(wInformer -> wInfo -> minTempC == 0) && (wInformer -> wInfo -> maxTempC == 0))
			wInformer -> FahrenheitToCelsius(wInformer);
	}
}


/**************************************************************************************
   
    ����� : WeatherInformer_CreateHttpRequest_PBO
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� :  ������������ ������� � ��������� ������ �� pogoda.bilets.org

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_PBO(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer -> httpReq, "http://pogoda.bilets.org/api/weather/");
	strcat(wInformer -> httpReq, wInformer -> wInfo -> cityName);
	strcat(wInformer -> httpReq, "/?key=063e5ee07278d771043590a169bc0983645ff814f1ed39339d71f2c7");
}

/**************************************************************************************
   
    ����� : WeatherInformer_ParseJsonData_PBO
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� : ������ ������ ���������� �� ��������� � ������� JSON �� pogoda.bilets.org

**************************************************************************************/
void WeatherInformer_ParseJsonData_PBO(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	
	if (wInformer -> error) return;

	char * errorData = "Not Found";
	// �������� ��������� ������
	if (strcmp (wInformer -> buffer, errorData) == 0)
	{
		wInformer -> error = CURLE_RECV_ERROR;
		return;
	}
	// ������ JSON ������
	json_object * jobj = json_tokener_parse(wInformer -> buffer);
	JSONParse(jobj, wInformer -> wInfo, "minimal_temperature", "maximal_temperature", "minimal_temperature_f", "maximal_temperature_f", "pressure", "humidity", "wind_speed", "wind_direction", "cloudcover", "precipitation");
	// ��� ������������� ��������������� �������� �����������
	if (wInformer -> scale == C_SCALE)
	{
		if ((wInformer -> wInfo -> minTempC != 0) && (wInformer -> wInfo -> maxTempC != 0) &&
			(wInformer -> wInfo -> minTempF == 0) && (wInformer -> wInfo -> maxTempF == 0))
			   wInformer -> CelsiusToFahrenheit(wInformer);
	}
	else
	if (wInformer -> scale == F_SCALE)
	{
		if ((wInformer -> wInfo -> minTempF != 0) && (wInformer -> wInfo -> maxTempF != 0) &&
			(wInformer -> wInfo -> minTempC == 0) && (wInformer -> wInfo -> maxTempC == 0))
			wInformer -> FahrenheitToCelsius(wInformer);
	}
}

/**************************************************************************************
   
    ����� : WeatherInformer_CreateHttpRequest_WG
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� :  ������������ ������� � ��������� ������ �� api.wunderground.com

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_WG(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer -> httpReq, "http://api.wunderground.com/api/df4498d31345157d/conditions/forecast/q/");
	strcat(wInformer -> httpReq, wInformer -> wInfo -> cityName);
	strcat(wInformer -> httpReq, ".json");
}

/**************************************************************************************
   
    ����� : WeatherInformer_ParseJsonData_WG
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� : ������ ������ ���������� �� ��������� � ������� JSON �� api.wunderground.com

**************************************************************************************/
void WeatherInformer_ParseJsonData_WG(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	
	if (wInformer -> error) return;
	// �������� ��������� ������
	char * errorData = "No cities match your search query";
	
	if (strstr(wInformer -> buffer, errorData) != NULL)
	{
		wInformer -> error = CURLE_RECV_ERROR;
		return;
	}
	// ������ JSON ������
	json_object * jobj = json_tokener_parse(wInformer -> buffer);
	JSONParse(jobj, wInformer -> wInfo, "celsius", "celsius", "fahrenheit", "fahrenheit", "pressure", "maxhumidity", "maxwind", "wind_direction", "cloudcover", "precipitation");
	// ��� ������������� ��������������� �������� �����������
	if (wInformer -> scale == C_SCALE)
	{
		if ((wInformer -> wInfo -> minTempC != 0) && (wInformer -> wInfo -> maxTempC != 0) &&
			(wInformer -> wInfo -> minTempF == 0) && (wInformer -> wInfo -> maxTempF == 0))
			   wInformer -> CelsiusToFahrenheit(wInformer);
	}
	else
	if (wInformer -> scale == F_SCALE)
	{
		if ((wInformer -> wInfo -> minTempF != 0) && (wInformer -> wInfo -> maxTempF != 0) &&
			(wInformer -> wInfo -> minTempC == 0) && (wInformer -> wInfo -> maxTempC == 0))
			wInformer -> FahrenheitToCelsius(wInformer);
	}
}


/**************************************************************************************
   
    ����� : WeatherInformer_CreateHttpRequest_GA
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� :  ������������ ������� � ��������� ������ �� www.google.com

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_GA(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer -> httpReq, "http://www.google.com/ig/api?weather=");
	strcat(wInformer -> httpReq, wInformer -> wInfo -> cityName);
	strcat(wInformer -> httpReq, "&format=json");
}

/**************************************************************************************
   
    ����� : WeatherInformer_ParseJsonData_GA
	
	��������� : 
				_this - ��������� �� ������ ������ WeatherInformer

	�������� : ������ ������ ���������� �� ��������� � ������� JSON �� www.google.com

**************************************************************************************/
void WeatherInformer_ParseJsonData_GA(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	
	if (wInformer -> error) return;

	char * errorData = "We're sorry...";
	// �������� ��������� ������
	if (strstr(wInformer -> buffer, errorData) != NULL)
	{
		wInformer -> error = CURLE_RECV_ERROR;
		return;
	}
	// ������ JSON ������
	json_object * jobj = json_tokener_parse(wInformer -> buffer);
	JSONParse(jobj, wInformer -> wInfo, "temp_c", "temp_c", "temp_f", "temp_f", "pressure", "humidity", "wind_speed", "wind_direction", "cloudcover", "precipitation");
	// ��� ������������� ��������������� �������� �����������
	if (wInformer -> scale == C_SCALE)
	{
		if ((wInformer -> wInfo -> minTempC != 0) && (wInformer -> wInfo -> maxTempC != 0) &&
			(wInformer -> wInfo -> minTempF == 0) && (wInformer -> wInfo -> maxTempF == 0))
			   wInformer -> CelsiusToFahrenheit(wInformer);
	}
	else
	if (wInformer -> scale == F_SCALE)
	{
		if ((wInformer -> wInfo -> minTempF != 0) && (wInformer -> wInfo -> maxTempF != 0) &&
			(wInformer -> wInfo -> minTempC == 0) && (wInformer -> wInfo -> maxTempC == 0))
			wInformer -> FahrenheitToCelsius(wInformer);
	}
}

/**************************************************************************************
   
    ������� : JSONParse
	
	��������� : jobj - ������ json �������
				wInfo - ���������� � ������
	            minTempCName - ������� �������� ��������� ����������� ����������� (�������)
				maxTempCName - ������� �������� ��������� ������������ ����������� (�������)
				minTempFName - ������� �������� ��������� ����������� ����������� (���������)
				maxTempFName - ������� �������� ��������� ������������ ����������� (���������)
				preasurename - ������� �������� ��������� ��������
				humidityName -������� �������� ��������� ���������
				windSpeedName - ������� �������� ��������� �������� �����
				windDirectionName - ������� �������� ��������� ����������� �����
				cloudinessName - ������� �������� ��������� ����������
				precipitationName - ������� �������� ��������� �������

	�������� : �������������� ������ JSON ������

**************************************************************************************/
void JSONParse(json_object * jobj,
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
				char *precipitationName)
{
    // �������� ��� JSON ������
	enum json_type type = json_object_get_type(jobj);
		
	if (type == json_type_array)
	{
	   // ��������� JSON �������
	   PasreJSONArray(jobj, NULL, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName,	preasurename, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
	}
	else
	{
	    // ���������� JSON �������
		json_object_object_foreach(jobj, key, val) {
	    
		// �������� ��� JSON ������
		enum json_type type = json_object_get_type(val);

	    switch (type) {
		case json_type_double: 
		case json_type_int: 
							   // ��������� JSON ������
		case json_type_string: CheckJSONValue(val, key, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName,	preasurename, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
			                   break; 
							   // ��������� JSON ������
		case json_type_object: jobj = json_object_object_get(jobj, key);
			                   JSONParse(jobj, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName,	preasurename, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
				               break;
							   // ��������� JSON �������
		case json_type_array:  PasreJSONArray(jobj, key, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName,	preasurename, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
				               break;
		}
	  }
	}
}


/**************************************************************************************
   
    ������� : PasreJSONArray
	
	��������� : jobj - ������ json �������
	            key - �������� ���������
				wInfo - ���������� � ������
	            minTempCName - ������� �������� ��������� ����������� ����������� (�������)
				maxTempCName - ������� �������� ��������� ������������ ����������� (�������)
				minTempFName - ������� �������� ��������� ����������� ����������� (���������)
				maxTempFName - ������� �������� ��������� ������������ ����������� (���������)
				preasurename - ������� �������� ��������� ��������
				humidityName -������� �������� ��������� ���������
				windSpeedName - ������� �������� ��������� �������� �����
				windDirectionName - ������� �������� ��������� ����������� �����
				cloudinessName - ������� �������� ��������� ����������
				precipitationName - ������� �������� ��������� �������

	�������� : �������������� ������ JSON ������

**************************************************************************************/
void PasreJSONArray( json_object *jobj, char *key,
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
					char *precipitationName)
{
  json_object *jarray = jobj; 
  // �������� JSON ������
  if(key) jarray = json_object_object_get(jobj, key);
  if (!jarray) return;

  // �������� ����� ������
  int arraylen = json_object_array_length(jarray);
  
  // ������������ �������� ������
  for (int i = 0; i < arraylen; i++)
  {
    json_object * jvalue = json_object_array_get_idx(jarray, i);
	// �������� ��� JSON ������
    enum json_type type = json_object_get_type(jvalue);

	// ������������ ������� ������ � ������������ � �����
    if (type == json_type_array)
	   PasreJSONArray(jvalue, NULL, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName,	preasurename, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
    else 
	if (type != json_type_object)
	{
		struct lh_entry *entry = json_object_get_object(jvalue) -> head;
		char *key = (char*)entry -> k;
	    CheckJSONValue(jvalue, key, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName,	preasurename, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
	}
    else 
       JSONParse(jvalue, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName,	preasurename, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
  }
}

/**************************************************************************************
   
    ������� : CheckJSONValue
	
	��������� : jobj - ������ json �������
	            key - �������� ���������
				wInfo - ���������� � ������
	            minTempCName - ������� �������� ��������� ����������� ����������� (�������)
				maxTempCName - ������� �������� ��������� ������������ ����������� (�������)
				minTempFName - ������� �������� ��������� ����������� ����������� (���������)
				maxTempFName - ������� �������� ��������� ������������ ����������� (���������)
				preasurename - ������� �������� ��������� ��������
				humidityName -������� �������� ��������� ���������
				windSpeedName - ������� �������� ��������� �������� �����
				windDirectionName - ������� �������� ��������� ����������� �����
				cloudinessName - ������� �������� ��������� ����������
				precipitationName - ������� �������� ��������� �������

	�������� :  �������� �������� ���������� ������ ����� 
				��������������� ������� JSON

**************************************************************************************/
void CheckJSONValue(json_object * jobj,
					char * key,
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
					char * precipitationName)
{
  // �������� ��� JSON ������
  enum json_type type = json_object_get_type(jobj);
  
  // ��������� ������ � ������������ � ����� � ���������� ����������
  switch (type) {
	case json_type_double: if (strcmp(key, minTempCName) == 0) wInfo -> minTempC = (int)json_object_get_double(jobj);
                           if (strcmp(key, maxTempCName) == 0) wInfo -> maxTempC = (int)json_object_get_double(jobj);                    
						   if (strcmp(key, minTempFName) == 0) wInfo -> minTempF = (int)json_object_get_double(jobj);
                           if (strcmp(key, maxTempFName) == 0) wInfo -> maxTempF = (int)json_object_get_double(jobj);                    
						   if (strcmp(key, preasureName) == 0) wInfo -> preasure = (int)json_object_get_double(jobj);
                           if (strcmp(key, humidityName) == 0) wInfo -> humidity = (int)json_object_get_double(jobj);                    
						   if (strcmp(key, windSpeedName) == 0) wInfo -> windSpeed = (int)json_object_get_double(jobj);
                           if (strcmp(key, cloudinessName) == 0) wInfo -> cloudiness = (int)json_object_get_double(jobj);                    
						   if (strcmp(key, precipitationName) == 0) wInfo -> precipitation = (int)json_object_get_double(jobj);                    
                           break;

    case json_type_int:    if (strcmp(key, minTempCName) == 0) wInfo -> minTempC = json_object_get_int(jobj);
                           if (strcmp(key, maxTempCName) == 0) wInfo -> maxTempC = json_object_get_int(jobj);                    
						   if (strcmp(key, minTempFName) == 0) wInfo -> minTempF = json_object_get_int(jobj);
                           if (strcmp(key, maxTempFName) == 0) wInfo -> maxTempF = json_object_get_int(jobj);                    
						   if (strcmp(key, preasureName) == 0) wInfo -> preasure = json_object_get_int(jobj);
                           if (strcmp(key, humidityName) == 0) wInfo -> humidity = json_object_get_int(jobj);                    
						   if (strcmp(key, windSpeedName) == 0) wInfo -> windSpeed = json_object_get_int(jobj);
                           if (strcmp(key, cloudinessName) == 0) wInfo -> cloudiness = json_object_get_int(jobj);                    
						   if (strcmp(key, precipitationName) == 0) wInfo -> precipitation = json_object_get_int(jobj);                    
                           break;

    case json_type_string: if (strcmp(key, minTempCName) == 0)  wInfo -> minTempC = atoi(json_object_get_string(jobj));
						   if (strcmp(key, maxTempCName) == 0) wInfo -> maxTempC = atoi(json_object_get_string(jobj));
						   if (strcmp(key, minTempFName) == 0) wInfo -> minTempF = atoi(json_object_get_string(jobj));
                           if (strcmp(key, maxTempFName) == 0) wInfo -> maxTempF = atoi(json_object_get_string(jobj));
						   if (strcmp(key, preasureName) == 0) wInfo -> preasure = atoi(json_object_get_string(jobj));
                           if (strcmp(key, humidityName) == 0) wInfo -> humidity = atoi(json_object_get_string(jobj));
						   if (strcmp(key, windSpeedName) == 0) wInfo -> windSpeed = atoi(json_object_get_string(jobj));
                           if (strcmp(key, cloudinessName) == 0) wInfo -> cloudiness = atoi(json_object_get_string(jobj));
						   if (strcmp(key, precipitationName) == 0) wInfo -> precipitation = atoi(json_object_get_string(jobj));
						   if (strcmp(key, windDirectionName) == 0) strcpy(wInfo -> windDirection,json_object_get_string(jobj));
                           break;
  }
}

