/**************************************************************************************
   
	Проект : Weather

	Модуль : WeatherInformerContainer.cpp

	Описание :  Модуль содержит реализацию методов класса WeatherInformerContainer.

**************************************************************************************/

#include "stdafx.h"

#include <stdlib.h>
#include "curl/curl.h"

#include "WeatherInformerContainer.h"

/**************************************************************************************
   
    Метод :  WeatherInformerContainer_Constructor
	
	Параметры : 
				cityName - название города
				scale - шкала температур

	Возвращает : указатель на объект

	Описание : Конструктор объектов класса WeatherInformerContainer.

**************************************************************************************/
PWEATHER_INFORMER_CONTAINER WeatherInformerContainer_Constructor(char * cityName, int scale)
{
	// Выделение памяти под объект
	PWEATHER_INFORMER_CONTAINER _this = (PWEATHER_INFORMER_CONTAINER)malloc(sizeof(WEATHER_INFORMER_CONTAINER));

	// Инициализация полей
	
	// Насторойка каждого информера
	_this -> container[0] = WeatherInformer_Constructor(scale, cityName, "http://free.worldweatheronline.com", WeatherInformer_CreateHttpRequest_WWO, WeatherInformer_ParseJsonData_WWO);
	_this -> container[1] = WeatherInformer_Constructor(scale, cityName, "http://open-weather.ru", WeatherInformer_CreateHttpRequest_OW, WeatherInformer_ParseJsonData_OW);
	_this -> container[2] = WeatherInformer_Constructor(scale, cityName, "http://pogoda.bilets.org", WeatherInformer_CreateHttpRequest_PBO, WeatherInformer_ParseJsonData_PBO);
	_this -> container[3] = WeatherInformer_Constructor(scale, cityName, "http://www.wunderground.com", WeatherInformer_CreateHttpRequest_WG, WeatherInformer_ParseJsonData_WG);
	_this -> container[4] = WeatherInformer_Constructor(scale, cityName, "http://www.google.com", WeatherInformer_CreateHttpRequest_GA, WeatherInformer_ParseJsonData_GA);
	

	// Информер усреднённого погноза погоды
	_this -> container[5] = WeatherInformer_Constructor(scale, cityName, "Total forecast", NULL, NULL);

	// Инициализация методов
	_this -> GetData = WeatherInformerContainer_GetData;
	_this -> ParseData = WeatherInformerContainer_ParseData;
	_this -> CreateTotalData = WeatherInformerContainer_CreateTotalData;
	_this -> Show = WeatherInformerContainer_Show;
	_this -> JsonOutput = WeatherInformerContainer_JsonOutput;
	_this -> XmlOutput = WeatherInformerContainer_XmlOutput;

	return _this;
}

/**************************************************************************************
   
    Метод :  WeatherInformerContainer_Destructor
	
	Параметры : 
	            _this - указатель на объект класса WeatherInformerContainer
				
	Описание : Деструктор объектов класса WeatherInformerContainer.

**************************************************************************************/
void WeatherInformerContainer_Destructor(PWEATHER_INFORMER_CONTAINER _this)
{
	if (_this)
	{
		// Удаляем все информеры
		for(int i = 0; i < WEATHER_INFORMER_CONTAINER_SIZE + 1; i++)
			WeatherInformer_Denstructor(_this -> container[i]);
		
		// Удаляем себя
		free(_this);
	}
}

/**************************************************************************************
   
    Метод : WeatherInformerContainer_GetData
	
	Параметры : 
				_this - указатель на объект класса WeatherInformerContainer

	Описание :  Подключение к источнику и извлечение данных прогноза погоды 
				для всех погодных информеров

**************************************************************************************/
void WeatherInformerContainer_GetData(void * _this)
{
	PWEATHER_INFORMER_CONTAINER wInfoContainer = (PWEATHER_INFORMER_CONTAINER)_this;

	for(int i = 0; i < WEATHER_INFORMER_CONTAINER_SIZE; i++)
	{
		try
		{
			// Формируем запрос
			wInfoContainer -> container[i] -> CreateHttpRequest(wInfoContainer -> container[i]);
		
			// Извлекаем данные
			wInfoContainer -> container[i] -> GetData(wInfoContainer -> container[i]);
		}
		catch(...)
		{
			printf("Error of data reading from source: %s \n", wInfoContainer -> container[i] -> sourceName);
			wInfoContainer -> container[i] -> error = CURLE_RECV_ERROR;
		}

	}
}

/**************************************************************************************
   
    Метод : WeatherInformerContainer_ParseData
	
	Параметры : 
				_this - указатель на объект класса WeatherInformerContainer

	Описание :  Анализ данных полученных от источника в формате JSON
				для всех погодных информеров

**************************************************************************************/
void WeatherInformerContainer_ParseData(void * _this)
{
	PWEATHER_INFORMER_CONTAINER wInfoContainer = (PWEATHER_INFORMER_CONTAINER)_this;

	for(int i = 0; i < WEATHER_INFORMER_CONTAINER_SIZE; i++)
	{
		try
		{
			wInfoContainer -> container[i] -> ParseJsonData(wInfoContainer -> container[i]);
		}
		catch(...)
		{
			printf("Error parsing data from source: %s \n", wInfoContainer -> container[i] -> sourceName);
			wInfoContainer -> container[i] -> error = CURLE_RECV_ERROR;
		}
	}
}

/**************************************************************************************
   
    Метод : WeatherInformerContainer_CreateTotalData
	
	Параметры : 
				_this - указатель на объект класса WeatherInformerContainer

	Описание :   Создание усреднённого прогноза погоды

**************************************************************************************/
void WeatherInformerContainer_CreateTotalData(void * _this)
{
	PWEATHER_INFORMER_CONTAINER wInfoContainer = (PWEATHER_INFORMER_CONTAINER)_this;

	int count = 0;

	// Подсичитываем количество информетор с ошибочными данными
	for(int i = 0; i < WEATHER_INFORMER_CONTAINER_SIZE; i++)
	  if (wInfoContainer -> container[i] -> error) count++;

	if (count == WEATHER_INFORMER_CONTAINER_SIZE)
	{
		// Если все информеры ошибочны, то устанавливаем ошибку и в усреднённом информере
		wInfoContainer -> container[WEATHER_INFORMER_CONTAINER_SIZE] -> error = CURLE_RECV_ERROR;
		return;
	}

	int avgMinTempC = 0;
	int avgMaxTempC = 0;
	int avgMinTempF = 0;
	int avgMaxTempF = 0;
		
	int avgPreasure = 0;
	int avgHumidity = 0;

	int avgWindSpeed = 0;
	int avgCloudiness = 0;
	int avgPrecipitation = 0;

	count = 0;

	// Подсичтываем суммы всех данных во всех информерах
	for(int i = 0; i < WEATHER_INFORMER_CONTAINER_SIZE; i++)
	{
		if (!wInfoContainer -> container[i] -> error)
		{
		    avgMinTempC += wInfoContainer -> container[i] -> wInfo -> minTempC;
			avgMaxTempC += wInfoContainer -> container[i] -> wInfo -> maxTempC;
		    avgMinTempF += wInfoContainer -> container[i] -> wInfo -> minTempF;
			avgMaxTempF += wInfoContainer -> container[i] -> wInfo -> maxTempF;
			avgPreasure += wInfoContainer -> container[i] -> wInfo -> preasure;
			avgHumidity += wInfoContainer -> container[i] -> wInfo -> humidity;
			avgWindSpeed += wInfoContainer -> container[i] -> wInfo -> windSpeed;
			avgCloudiness += wInfoContainer -> container[i] -> wInfo -> cloudiness;
			avgPrecipitation += wInfoContainer -> container[i] -> wInfo -> precipitation;
           
			count++;
		}
	}

	// Формируем усреднённые значения
	if (count)
	{
	    wInfoContainer -> container[WEATHER_INFORMER_CONTAINER_SIZE] -> wInfo -> minTempC = avgMinTempC / count;
		wInfoContainer -> container[WEATHER_INFORMER_CONTAINER_SIZE] -> wInfo -> maxTempC = avgMaxTempC / count;
	    wInfoContainer -> container[WEATHER_INFORMER_CONTAINER_SIZE] -> wInfo -> minTempF = avgMinTempF / count;
	    wInfoContainer -> container[WEATHER_INFORMER_CONTAINER_SIZE] -> wInfo -> maxTempF = avgMaxTempF / count;
		wInfoContainer -> container[WEATHER_INFORMER_CONTAINER_SIZE] -> wInfo -> preasure = avgPreasure / count;
		wInfoContainer -> container[WEATHER_INFORMER_CONTAINER_SIZE] -> wInfo -> humidity = avgHumidity / count;
		wInfoContainer -> container[WEATHER_INFORMER_CONTAINER_SIZE] -> wInfo -> windSpeed = avgWindSpeed / count;
		wInfoContainer -> container[WEATHER_INFORMER_CONTAINER_SIZE] -> wInfo -> cloudiness = avgCloudiness / count;
		wInfoContainer -> container[WEATHER_INFORMER_CONTAINER_SIZE] -> wInfo -> precipitation = avgPrecipitation / count;
	}

	// Находим максимально часто встречающееся значение направления ветра
	int maxCount = 0;
	int maxCountInd = 0;
	for(int i = 0; i < WEATHER_INFORMER_CONTAINER_SIZE; i++)
	{
		count = 0;

		for(int j = 0; j < WEATHER_INFORMER_CONTAINER_SIZE; j++)
			if ((!wInfoContainer -> container[i] -> error) && (!wInfoContainer -> container[j] -> error))
				if (strcmp(wInfoContainer -> container[i] -> wInfo -> windDirection, wInfoContainer -> container[j] -> wInfo -> windDirection) == 0)
					count;

		if (count > maxCount)
		{
			maxCount = count;
			maxCountInd = i;
		}
	}

	// Устатавливаем максимально часто встречающееся значение направления ветра в усреднённый информер
	strcpy(wInfoContainer -> container[WEATHER_INFORMER_CONTAINER_SIZE] -> wInfo -> windDirection, wInfoContainer -> container[maxCountInd] -> wInfo -> windDirection);
}

/**************************************************************************************
   
    Метод : WeatherInformerContainer_Show
	
	Параметры : 
				_this - указатель на объект класса WeatherInformerContainer

	Описание :  Отображение данных прогноза погоды на экран
				для всех погодных информеров

**************************************************************************************/
void WeatherInformerContainer_Show(void * _this)
{
	PWEATHER_INFORMER_CONTAINER wInfoContainer = (PWEATHER_INFORMER_CONTAINER)_this;

	printf("\n-------Weather infomation-------\n");

	for(int i = 0; i < WEATHER_INFORMER_CONTAINER_SIZE + 1; i++)
		wInfoContainer -> container[i] -> Show(wInfoContainer -> container[i]);
}

/**************************************************************************************
   
    Метод : WeatherInformerContainer_JsonOutput
	
	Параметры : 
				_this - указатель на объект класса WeatherInformerContainer

	Описание :  Отображение данных прогноза погоды в файл
				в формате JSON

**************************************************************************************/
void WeatherInformerContainer_JsonOutput(void * _this)
{
	PWEATHER_INFORMER_CONTAINER wInfoContainer = (PWEATHER_INFORMER_CONTAINER)_this;

	char fileName[FILE_NAME_SIZE];
	
	// Формируем имя файла
	strcpy(fileName, "WeatherForecast_");
	strcat(fileName, wInfoContainer -> container[WEATHER_INFORMER_CONTAINER_SIZE] -> wInfo -> cityName);
	strcat(fileName, ".json");

	printf("Data in JSON format was saved to file %s\n", fileName);
	
	// Создаём основной jSON объект
	json_object * mainJObj = json_object_new_object();

	// Создаём JSON масив информеров
	json_object * jarray = json_object_new_array();
	
	// Формируем записи
	json_object *title = json_object_new_string("Weather_forecast_for_tomorrow");
	json_object_object_add(mainJObj, "Title", title);

	for(int i = 0; i < WEATHER_INFORMER_CONTAINER_SIZE + 1; i++)
	{
		if (wInfoContainer -> container[i] -> error) continue;

		// Формируем записи о погоде
		json_object * weatherJObj = json_object_new_object();
		
		json_object *jsourceName = json_object_new_string(wInfoContainer -> container[i] -> sourceName);
		json_object *jcityName = json_object_new_string(wInfoContainer -> container[i] -> wInfo -> cityName);
		json_object *jminTempC = json_object_new_int(wInfoContainer -> container[i] -> wInfo -> minTempC);
		json_object *jmaxTempC = json_object_new_int(wInfoContainer -> container[i] -> wInfo -> maxTempC);
		json_object *jminTempF = json_object_new_int(wInfoContainer -> container[i] -> wInfo -> minTempF);
		json_object *jmaxTempF = json_object_new_int(wInfoContainer -> container[i] -> wInfo -> maxTempF);
		json_object *jhumidity = json_object_new_int(wInfoContainer -> container[i] -> wInfo -> humidity);
		json_object *jpreasure = json_object_new_int(wInfoContainer -> container[i] -> wInfo -> preasure);
		json_object *jcloudiness = json_object_new_int(wInfoContainer -> container[i] -> wInfo -> cloudiness);
		json_object *jprecipitation = json_object_new_int(wInfoContainer -> container[i] -> wInfo -> precipitation);
		json_object *jwindSpeed = json_object_new_int(wInfoContainer -> container[i] -> wInfo -> windSpeed);
		json_object *jwindDirection = json_object_new_string(wInfoContainer -> container[i] -> wInfo -> windDirection);

		json_object_object_add(weatherJObj, "Source", jsourceName);
		json_object_object_add(weatherJObj, "City_name", jcityName);
		json_object_object_add(weatherJObj, "Minimal_temperature_(C)", jminTempC);
		json_object_object_add(weatherJObj, "Maximal_temperature_(C)", jmaxTempC);
		json_object_object_add(weatherJObj, "Minimal_temperature_(F)", jminTempF);
		json_object_object_add(weatherJObj, "Maximal_temperature_(F)", jmaxTempF);
		json_object_object_add(weatherJObj, "Humidity", jhumidity);
		json_object_object_add(weatherJObj, "Preasure", jpreasure);
		json_object_object_add(weatherJObj, "Cloudiness", jcloudiness);
		json_object_object_add(weatherJObj, "Preciptation", jprecipitation);
		json_object_object_add(weatherJObj, "Wind_speed", jwindSpeed);
		json_object_object_add(weatherJObj, "Wind_direction", jwindDirection);

		json_object_array_add(jarray, weatherJObj);
	}


	json_object_object_add(mainJObj, "Weather_Informers", jarray);

	int bufSize = strlen(json_object_to_json_string(mainJObj));
	
	// Записываем результирующие данные в JSON-формате в файл
	FILE * fOut = fopen(fileName, "wb");

	if ((int)fwrite(json_object_to_json_string(mainJObj), 1, bufSize, fOut) != bufSize) 
	{
	    printf ("Error file writting!!! \n");
		fclose(fOut);
		return;
	}

    fclose(fOut);
}

/**************************************************************************************
   
    Метод : WeatherInformerContainer_XmlOutput
	
	Параметры : 
				_this - указатель на объект класса WeatherInformerContainer

	Описание :  Отображение данных прогноза погоды в файл
				в формате XML

**************************************************************************************/
void WeatherInformerContainer_XmlOutput(void * _this)
{
	PWEATHER_INFORMER_CONTAINER wInfoContainer = (PWEATHER_INFORMER_CONTAINER)_this;

	char fileName[FILE_NAME_SIZE];

	// Формируем имя файла
	strcpy(fileName, "WeatherForecast_");
	strcat(fileName, wInfoContainer -> container[WEATHER_INFORMER_CONTAINER_SIZE] -> wInfo -> cityName);
	strcat(fileName, ".xml");

	printf("Data in XML format was saved to file %s\n", fileName);

	// Открываем файл
	FILE * fOut = fopen(fileName, "w");

	char tmpStr[REQUEST_SIZE];
	char intVal[8];
	
	strcpy(tmpStr, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
	fwrite(tmpStr, 1, strlen(tmpStr), fOut);


	strcpy(tmpStr, "<Weather_forecast_for_tomorrow>");
	fwrite(tmpStr, 1, strlen(tmpStr), fOut);

	for(int i = 0; i < WEATHER_INFORMER_CONTAINER_SIZE + 1; i++)
	{
		if (wInfoContainer -> container[i] -> error) continue;

		// Формируем запсиси о погоде в соответствии с xml форматом
		// и записываем в файл
		itoa(i, intVal, 10);
		strcpy(tmpStr, "<Weather_Informer_");
		strcat(tmpStr, intVal);
		strcat(tmpStr, ">");
        fwrite(tmpStr, 1, strlen(tmpStr), fOut);				

		strcpy(tmpStr, "<Source_Name>");
		strcat(tmpStr, wInfoContainer -> container[i] -> sourceName);
		strcat(tmpStr, "</Source_Name>");
		fwrite(tmpStr, 1, strlen(tmpStr), fOut);				
				
		strcpy(tmpStr, "<City_Name>");
		strcat(tmpStr, wInfoContainer -> container[i] -> wInfo -> cityName);
		strcat(tmpStr, "</City_Name>");
		fwrite(tmpStr, 1, strlen(tmpStr), fOut);				

		itoa(wInfoContainer -> container[i] -> wInfo -> minTempC, intVal, 10);
		strcpy(tmpStr, "<Minimal_Temperature_C>");
		strcat(tmpStr, intVal);
		strcat(tmpStr, "</Minimal_Temperature_C>");
		fwrite(tmpStr, 1, strlen(tmpStr), fOut);				

		itoa(wInfoContainer -> container[i] -> wInfo -> maxTempC, intVal, 10);
		strcpy(tmpStr, "<Maximal_Temperature_C>");
		strcat(tmpStr, intVal);
		strcat(tmpStr, "</Maximal_Temperature_C>");
		fwrite(tmpStr, 1, strlen(tmpStr), fOut);				

		itoa(wInfoContainer -> container[i] -> wInfo -> minTempF, intVal, 10);
		strcpy(tmpStr, "<Minimal_Temperature_F>");
		strcat(tmpStr, intVal);
		strcat(tmpStr, "</Minimal_Temperature_F>");
		fwrite(tmpStr, 1, strlen(tmpStr), fOut);				

		itoa(wInfoContainer -> container[i] -> wInfo -> maxTempF, intVal, 10);
		strcpy(tmpStr, "<Maximal_Temperature_F>");
		strcat(tmpStr, intVal);
		strcat(tmpStr, "</Maximal_Temperature_F>");
		fwrite(tmpStr, 1, strlen(tmpStr), fOut);				

  	    itoa(wInfoContainer -> container[i] -> wInfo -> humidity, intVal, 10);
		strcpy(tmpStr, "<Humidity>");
		strcat(tmpStr, intVal);
		strcat(tmpStr, "</Humidity>");
		fwrite(tmpStr, 1, strlen(tmpStr), fOut);				

		itoa(wInfoContainer -> container[i] -> wInfo -> preasure, intVal, 10);
		strcpy(tmpStr, "<Preasure>");
		strcat(tmpStr, intVal);
		strcat(tmpStr, "</Preasure>");
		fwrite(tmpStr, 1, strlen(tmpStr), fOut);				

		itoa(wInfoContainer -> container[i] -> wInfo -> cloudiness, intVal, 10);
		strcpy(tmpStr, "<Cloudiness>");
		strcat(tmpStr, intVal);
		strcat(tmpStr, "</Cloudiness>");
		fwrite(tmpStr, 1, strlen(tmpStr), fOut);				

		itoa(wInfoContainer -> container[i] -> wInfo -> precipitation, intVal, 10);
		strcpy(tmpStr, "<Precipitation>");
		strcat(tmpStr, intVal);
		strcat(tmpStr, "</Precipitation>");
		fwrite(tmpStr, 1, strlen(tmpStr), fOut);				

		itoa(wInfoContainer -> container[i] -> wInfo -> windSpeed, intVal, 10);
		strcpy(tmpStr, "<Wind_speed>");
		strcat(tmpStr, intVal);
		strcat(tmpStr, "</Wind_speed>");
		fwrite(tmpStr, 1, strlen(tmpStr), fOut);				

		strcpy(tmpStr, "<Wind_direction>");
		strcat(tmpStr, wInfoContainer -> container[i] -> wInfo -> windDirection);
		strcat(tmpStr, "</Wind_direction>");
		fwrite(tmpStr, 1, strlen(tmpStr), fOut);				

		itoa(i, intVal, 10);
		strcpy(tmpStr, "</Weather_Informer_");
		strcat(tmpStr, intVal);
		strcat(tmpStr, ">");
        fwrite(tmpStr, 1, strlen(tmpStr), fOut);				
	}

	strcpy(tmpStr, "</Weather_forecast_for_tomorrow>");
	fwrite(tmpStr, 1, strlen(tmpStr), fOut);

	fclose(fOut);
}