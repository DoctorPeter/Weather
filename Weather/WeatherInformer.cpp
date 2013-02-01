/**************************************************************************************
   
	Проект : Weather

	Модуль : WeatherInformer.cpp

	Описание : Модуль содержит реализацию методов класса WeatherInformer.

**************************************************************************************/

#include "stdafx.h"

#include "curl/curl.h"
#include <stdlib.h>

#include "WeatherInformer.h"

// временная глобальная переменная 
// для определения размера буфера
int bufSize = 0;

/**************************************************************************************
   
    Метод :  WeatherInformer_Constructor
	
	Параметры : 
	            scale - шкала температур
				cityName - название города
				sourceName - имя источника данных
				CreateHttpRequest - метод для формирование запроса
				ParseJsonData - метод анализа данных от источника

	Возвращает : указатель на объект

	Описание : Конструктор объектов класса WeatherInformer.

**************************************************************************************/
PWEATHER_INFORMER WeatherInformer_Constructor(int scale, char * cityName,  char *sourceName, WeatherInformer_CreateHttpRequest_Method CreateHttpRequest, WeatherInformer_ParseJsonData_Method ParseJsonData)
{

	// Выделение памяти под объект
	PWEATHER_INFORMER _this = (PWEATHER_INFORMER)malloc(sizeof(WEATHER_INFORMER));
	
	// Инициализация полей
	_this -> wInfo = WeatherInfo_Constructor();
	_this -> buffer = NULL;
	_this -> bufSize = 0;
	_this -> scale = scale;
	_this -> error = 0;
	
	// Инициализация методов
	_this -> DataWriter = WeatherInformer_DataWriter;
	_this -> GetData = WeatherInformer_GetData;
	_this -> SetCityName = WeatherInformer_SetCityName;
	_this -> SetSourceName = WeatherInformer_SetSourceName;
	_this -> Show = WeatherInformer_Show;
	_this -> CelsiusToFahrenheit = WeatherInformer_CelsiusToFahrenheit;
    _this -> FahrenheitToCelsius = WeatherInformer_FahrenheitToCelsius;
	_this -> CreateHttpRequest = CreateHttpRequest;
	_this -> ParseJsonData = ParseJsonData;

	// Установка значений названия города и имени источника
	_this -> SetCityName(_this, cityName);
	_this -> SetSourceName(_this, sourceName);

	return _this;
}

/**************************************************************************************
   
    Метод :  WeatherInformer_Denstructor
	
	Параметры : 
	            _this - указатель на объект класса WeatherInformer
				
	Описание : Деструктор объектов класса WeatherInformer.

**************************************************************************************/
void WeatherInformer_Denstructor(PWEATHER_INFORMER _this)
{
	if (_this)
	{
		// Удаляем объект WeatherInfo
		WeatherInfo_Destructor(_this -> wInfo);

		// Удаляем буфер
		if (_this -> buffer) free(_this -> buffer);

		// Удаляем себя
		free(_this);
	}
}

/**************************************************************************************
   
    Метод :  WeatherInformer_DataWriter
	
	Параметры : 
				data - данных полученных от источника
				size - размер данных
				nMemb - количество элементов данных
				buffer - буфер для храненния данных

	Возвращает : количество записаных байт

	Описание : Запись данных из источника в буфер

**************************************************************************************/
int WeatherInformer_DataWriter(char *data, size_t size, size_t nMemb, char ** buffer)
{
  if (*buffer == NULL)
  {
	  // Если буфер пустой, то выделить под него память
	  *buffer = (char *)malloc(size*nMemb);
	  bufSize = 0;
  }
  else
  {
	  // Иначе увеличить буфер
	  *buffer = (char*)realloc(*buffer, bufSize + size*nMemb); 
  }

  // Скопировать данные в буфер
  memcpy(*buffer + bufSize, data, size*nMemb);

  // Увеличить значение количества прочитанных байтов
  bufSize += size*nMemb;
  
  // Вернуть значение количества прочитанных байтов
  return bufSize;
}

/**************************************************************************************
   
    Метод : WeatherInformer_GetData
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Возвращает : код ошибки

	Описание : Подключение к источнику и извлечение данных прогноза погоды.
	           Использует функционал библиотеки CURL.

**************************************************************************************/
int WeatherInformer_GetData(void * _this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	
	char errorBuffer[CURL_ERROR_SIZE];

	CURL *curl;
    CURLcode result = CURLE_RECV_ERROR;
    
	// Инициализируем CURL объект
	curl = curl_easy_init();
    
    if (curl)
    {
       // Настраиваем CURL подключение
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
		curl_easy_setopt(curl, CURLOPT_URL, wInformer -> httpReq);
	    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wInformer -> DataWriter);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &(wInformer -> buffer));

		// Извлекаем данные
	    result = curl_easy_perform(curl);
      
      if (result == CURLE_OK)
	  {
		  // Если данные получены дописываем в конец буфера ноль
		  wInformer -> buffer = (char*)realloc(wInformer -> buffer, bufSize + 1); 
		  bufSize += 1;
		  wInformer -> buffer[bufSize - 1] = '\0';
		  wInformer -> bufSize = bufSize;
	  }
	}
  
  // Удаляем CURL объект
  curl_easy_cleanup(curl);

  // Возвращаем код ошибки
  wInformer -> error = (int)result;
  return wInformer -> error;
}

/**************************************************************************************
   
    Метод : WeatherInformer_SetCityName
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание : Установка навания города

**************************************************************************************/
void WeatherInformer_SetCityName(void* _this, char *cityName)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;

	strcpy(wInformer -> wInfo -> cityName, cityName);
}

/**************************************************************************************
   
    Метод : WeatherInformer_SetSourceName
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание : Установка имени источника

**************************************************************************************/
void WeatherInformer_SetSourceName(void* _this, char *sourceName)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;

	strcpy(wInformer -> sourceName, sourceName);
}

/**************************************************************************************
   
    Метод : WeatherInformer_Show
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание : Отображение данных прогноза погоды на экран

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
   
    Метод : WeatherInformer_CelsiusToFahrenheit
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание : Преобразование данных температуры из шкалы Цельсия в шкалу Фаренгейта

**************************************************************************************/
void WeatherInformer_CelsiusToFahrenheit(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	wInformer -> wInfo -> minTempF = (wInformer -> wInfo -> minTempC) * (5.0/9.0) + 32.0;
	wInformer -> wInfo -> maxTempF = (wInformer -> wInfo -> maxTempC) * (5.0/9.0) + 32.0;
}

/**************************************************************************************
   
    Метод : WeatherInformer_FahrenheitToCelsius
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание : Преобразование данных температуры из шкалы Фаренгейта в шкалу Цельсия

**************************************************************************************/
void WeatherInformer_FahrenheitToCelsius(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	wInformer -> wInfo -> minTempC = ((wInformer -> wInfo -> minTempF) - 32.0) * (5.0/9.0);
	wInformer -> wInfo -> maxTempC = ((wInformer -> wInfo -> maxTempF) - 32.0) * (5.0/9.0);
}

/**************************************************************************************
   
    Метод : WeatherInformer_CreateHttpRequest_WWO
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание :  Формирование запроса к источнику данных от worldweatheronline

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_WWO(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer -> httpReq, "http://free.worldweatheronline.com/feed/weather.ashx?format=json&date=tomorrow&key=52a9e8e499131425132901&q=");
	strcat(wInformer -> httpReq, wInformer -> wInfo -> cityName);
}

/**************************************************************************************
   
    Метод : WeatherInformer_ParseJsonData_WWO
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание : Анализ данных полученных от источника в формате JSON от worldweatheronline

**************************************************************************************/
void WeatherInformer_ParseJsonData_WWO(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	
	if (wInformer -> error) return;

	char * errorData = "{ \"data\": { \"error\": [ {\"msg\": \"Unable to find any matching weather location to the query submitted!\" } ] }}";
	
	// Проверка возможной ошибки
	if (strcmp (wInformer -> buffer, errorData) == 0)
	{
		wInformer -> error = CURLE_RECV_ERROR;
		return;
	}

	// Анализ JSON данных
	json_object * jobj = json_tokener_parse(wInformer -> buffer);
	JSONParse(jobj, wInformer -> wInfo, "tempMinC", "tempMaxC", "tempMinF", "tempMaxF", "pressure", "humidity", "windspeedKmph", "winddirection", "cloudcover", "precipMM");

	// При необходимости конвертирование значений температуры
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
   
    Метод : WeatherInformer_CreateHttpRequest_OW
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание :  Формирование запроса к источнику данных от open-weather.ru

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_OW(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer -> httpReq, "http://open-weather.ru/api/weather/");
	strcat(wInformer -> httpReq, wInformer -> wInfo -> cityName);
	strcat(wInformer -> httpReq, "/?key=063e5ee07278d771043590a169bc0983645ff814f1ed39339d71f2c7");
}

/**************************************************************************************
   
    Метод : WeatherInformer_ParseJsonData_OW
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание : Анализ данных полученных от источника в формате JSON от open-weather.ru

**************************************************************************************/
void WeatherInformer_ParseJsonData_OW(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	
	if (wInformer -> error) return;

	char * errorData = "Not Found";
	// Проверка возможной ошибки
	if (strcmp (wInformer -> buffer, errorData) == 0)
	{
		wInformer -> error = CURLE_RECV_ERROR;
		return;
	}
	// Анализ JSON данных
	json_object * jobj = json_tokener_parse(wInformer -> buffer);
	JSONParse(jobj, wInformer -> wInfo, "minimal_temperature", "maximal_temperature", "minimal_temperature_f", "maximal_temperature_f", "pressure", "humidity", "wind_speed", "wind_direction", "cloudcover", "precipitation");
	// При необходимости конвертирование значений температуры
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
   
    Метод : WeatherInformer_CreateHttpRequest_PBO
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание :  Формирование запроса к источнику данных от pogoda.bilets.org

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_PBO(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer -> httpReq, "http://pogoda.bilets.org/api/weather/");
	strcat(wInformer -> httpReq, wInformer -> wInfo -> cityName);
	strcat(wInformer -> httpReq, "/?key=063e5ee07278d771043590a169bc0983645ff814f1ed39339d71f2c7");
}

/**************************************************************************************
   
    Метод : WeatherInformer_ParseJsonData_PBO
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание : Анализ данных полученных от источника в формате JSON от pogoda.bilets.org

**************************************************************************************/
void WeatherInformer_ParseJsonData_PBO(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	
	if (wInformer -> error) return;

	char * errorData = "Not Found";
	// Проверка возможной ошибки
	if (strcmp (wInformer -> buffer, errorData) == 0)
	{
		wInformer -> error = CURLE_RECV_ERROR;
		return;
	}
	// Анализ JSON данных
	json_object * jobj = json_tokener_parse(wInformer -> buffer);
	JSONParse(jobj, wInformer -> wInfo, "minimal_temperature", "maximal_temperature", "minimal_temperature_f", "maximal_temperature_f", "pressure", "humidity", "wind_speed", "wind_direction", "cloudcover", "precipitation");
	// При необходимости конвертирование значений температуры
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
   
    Метод : WeatherInformer_CreateHttpRequest_WG
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание :  Формирование запроса к источнику данных от api.wunderground.com

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_WG(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer -> httpReq, "http://api.wunderground.com/api/df4498d31345157d/conditions/forecast/q/");
	strcat(wInformer -> httpReq, wInformer -> wInfo -> cityName);
	strcat(wInformer -> httpReq, ".json");
}

/**************************************************************************************
   
    Метод : WeatherInformer_ParseJsonData_WG
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание : Анализ данных полученных от источника в формате JSON от api.wunderground.com

**************************************************************************************/
void WeatherInformer_ParseJsonData_WG(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	
	if (wInformer -> error) return;
	// Проверка возможной ошибки
	char * errorData = "No cities match your search query";
	
	if (strstr(wInformer -> buffer, errorData) != NULL)
	{
		wInformer -> error = CURLE_RECV_ERROR;
		return;
	}
	// Анализ JSON данных
	json_object * jobj = json_tokener_parse(wInformer -> buffer);
	JSONParse(jobj, wInformer -> wInfo, "celsius", "celsius", "fahrenheit", "fahrenheit", "pressure", "maxhumidity", "maxwind", "wind_direction", "cloudcover", "precipitation");
	// При необходимости конвертирование значений температуры
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
   
    Метод : WeatherInformer_CreateHttpRequest_GA
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание :  Формирование запроса к источнику данных от www.google.com

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_GA(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer -> httpReq, "http://www.google.com/ig/api?weather=");
	strcat(wInformer -> httpReq, wInformer -> wInfo -> cityName);
	strcat(wInformer -> httpReq, "&format=json");
}

/**************************************************************************************
   
    Метод : WeatherInformer_ParseJsonData_GA
	
	Параметры : 
				_this - указатель на объект класса WeatherInformer

	Описание : Анализ данных полученных от источника в формате JSON от www.google.com

**************************************************************************************/
void WeatherInformer_ParseJsonData_GA(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	
	if (wInformer -> error) return;

	char * errorData = "We're sorry...";
	// Проверка возможной ошибки
	if (strstr(wInformer -> buffer, errorData) != NULL)
	{
		wInformer -> error = CURLE_RECV_ERROR;
		return;
	}
	// Анализ JSON данных
	json_object * jobj = json_tokener_parse(wInformer -> buffer);
	JSONParse(jobj, wInformer -> wInfo, "temp_c", "temp_c", "temp_f", "temp_f", "pressure", "humidity", "wind_speed", "wind_direction", "cloudcover", "precipitation");
	// При необходимости конвертирование значений температуры
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
   
    Функция : JSONParse
	
	Параметры : jobj - объект json разбора
				wInfo - информация о погоде
	            minTempCName - навание искомого параметра минимальной температуры (Цельсий)
				maxTempCName - навание искомого параметра максимальной температуры (Цельсий)
				minTempFName - навание искомого параметра минимальной температуры (Фаренгейт)
				maxTempFName - навание искомого параметра максимальной температуры (Фаренгейт)
				preasurename - навание искомого параметра давления
				humidityName -навание искомого параметра влажности
				windSpeedName - навание искомого параметра скорости ветра
				windDirectionName - навание искомого параметра направления ветра
				cloudinessName - навание искомого параметра облачности
				precipitationName - навание искомого параметра осадков

	Описание : Синтаксический анализ JSON данных

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
    // Получаем тип JSON данных
	enum json_type type = json_object_get_type(jobj);
		
	if (type == json_type_array)
	{
	   // Обработка JSON массива
	   PasreJSONArray(jobj, NULL, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName,	preasurename, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
	}
	else
	{
	    // Перебираем JSON объекты
		json_object_object_foreach(jobj, key, val) {
	    
		// Получаем тип JSON данных
		enum json_type type = json_object_get_type(val);

	    switch (type) {
		case json_type_double: 
		case json_type_int: 
							   // Обработка JSON записи
		case json_type_string: CheckJSONValue(val, key, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName,	preasurename, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
			                   break; 
							   // Обработка JSON обекта
		case json_type_object: jobj = json_object_object_get(jobj, key);
			                   JSONParse(jobj, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName,	preasurename, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
				               break;
							   // Обработка JSON массива
		case json_type_array:  PasreJSONArray(jobj, key, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName,	preasurename, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
				               break;
		}
	  }
	}
}


/**************************************************************************************
   
    Функция : PasreJSONArray
	
	Параметры : jobj - объект json разбора
	            key - название параметра
				wInfo - информация о погоде
	            minTempCName - навание искомого параметра минимальной температуры (Цельсий)
				maxTempCName - навание искомого параметра максимальной температуры (Цельсий)
				minTempFName - навание искомого параметра минимальной температуры (Фаренгейт)
				maxTempFName - навание искомого параметра максимальной температуры (Фаренгейт)
				preasurename - навание искомого параметра давления
				humidityName -навание искомого параметра влажности
				windSpeedName - навание искомого параметра скорости ветра
				windDirectionName - навание искомого параметра направления ветра
				cloudinessName - навание искомого параметра облачности
				precipitationName - навание искомого параметра осадков

	Описание : Синтаксический анализ JSON масива

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
  // Получаем JSON массив
  if(key) jarray = json_object_object_get(jobj, key);
  if (!jarray) return;

  // Получаем длину масива
  int arraylen = json_object_array_length(jarray);
  
  // Обрабатываем элементы масива
  for (int i = 0; i < arraylen; i++)
  {
    json_object * jvalue = json_object_array_get_idx(jarray, i);
	// Получаем тип JSON данных
    enum json_type type = json_object_get_type(jvalue);

	// Обрабатываем элемент масива в соответствии с типом
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
   
    Функция : CheckJSONValue
	
	Параметры : jobj - объект json разбора
	            key - название параметра
				wInfo - информация о погоде
	            minTempCName - навание искомого параметра минимальной температуры (Цельсий)
				maxTempCName - навание искомого параметра максимальной температуры (Цельсий)
				minTempFName - навание искомого параметра минимальной температуры (Фаренгейт)
				maxTempFName - навание искомого параметра максимальной температуры (Фаренгейт)
				preasurename - навание искомого параметра давления
				humidityName -навание искомого параметра влажности
				windSpeedName - навание искомого параметра скорости ветра
				windDirectionName - навание искомого параметра направления ветра
				cloudinessName - навание искомого параметра облачности
				precipitationName - навание искомого параметра осадков

	Описание :  Проверка значения опредеённой записи после 
				синтаксического анализа JSON

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
  // Получаем тип JSON данных
  enum json_type type = json_object_get_type(jobj);
  
  // Считываем данные в соответствии с типом и названиями параметров
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

