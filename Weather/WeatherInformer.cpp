/**************************************************************************************

	Project: Weather

	Module: WeatherInformer.cpp

	Description: The module contains the implementation of the methods of the WeatherInformer class.

**************************************************************************************/

#include "stdafx.h"

#include "curl/curl.h"
#include <stdlib.h>

#include "WeatherInformer.h"

// Temporary global variable
// to determine the buffer size
int bufSize = 0;

/**************************************************************************************

	Method: WeatherInformer_Constructor

	Parameters:
				scale - temperature scale
				cityName - city name
				sourceName - data source name
				CreateHttpRequest - method to form a request
				ParseJsonData - method to parse data from the source

	Returns: pointer to an object

	Description: Constructor of objects of the WeatherInformer class.

**************************************************************************************/
PWEATHER_INFORMER WeatherInformer_Constructor(int scale, char * cityName, char *sourceName, WeatherInformer_CreateHttpRequest_Method CreateHttpRequest, WeatherInformer_ParseJsonData_Method ParseJsonData)
{

	// Memory allocation for the object
	PWEATHER_INFORMER _this = (PWEATHER_INFORMER)malloc(sizeof(WEATHER_INFORMER));

	// Initialization of fields
	_this->wInfo = WeatherInfo_Constructor();
	_this->buffer = NULL;
	_this->bufSize = 0;
	_this->scale = scale;
	_this->error = 0;

	// Initialization of methods
	_this->DataWriter = WeatherInformer_DataWriter;
	_this->GetData = WeatherInformer_GetData;
	_this->SetCityName = WeatherInformer_SetCityName;
	_this->SetSourceName = WeatherInformer_SetSourceName;
	_this->Show = WeatherInformer_Show;
	_this->CelsiusToFahrenheit = WeatherInformer_CelsiusToFahrenheit;
	_this->FahrenheitToCelsius = WeatherInformer_FahrenheitToCelsius;
	_this->CreateHttpRequest = CreateHttpRequest;
	_this->ParseJsonData = ParseJsonData;

	// Setting the values of the city name and source name
	_this->SetCityName(_this, cityName);
	_this->SetSourceName(_this, sourceName);

	return _this;
}

/**************************************************************************************

	Method: WeatherInformer_Denstructor

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Destructor of objects of the WeatherInformer class.

**************************************************************************************/
void WeatherInformer_Denstructor(PWEATHER_INFORMER _this)
{
	if (_this)
	{
		// Delete the WeatherInfo object
		WeatherInfo_Destructor(_this->wInfo);

		// Delete the buffer
		if (_this->buffer) free(_this->buffer);

		// Delete itself
		free(_this);
	}
}


/**************************************************************************************

	Method: WeatherInformer_DataWriter

	Parameters:
				data - data received from the source
				size - data size
				nMemb - number of data elements
				buffer - buffer to store data

	Returns: number of bytes written

	Description: Writing data from the source to the buffer

**************************************************************************************/
int WeatherInformer_DataWriter(char *data, size_t size, size_t nMemb, char ** buffer)
{
	if (*buffer == NULL)
	{
		// If the buffer is empty, allocate memory for it
		*buffer = (char *)malloc(size * nMemb);
		bufSize = 0;
	}
	else
	{
		// Otherwise, increase the buffer
		*buffer = (char*)realloc(*buffer, bufSize + size * nMemb);
	}

	// Copy data to the buffer
	memcpy(*buffer + bufSize, data, size * nMemb);

	// Increase the value of the number of bytes read
	bufSize += size * nMemb;

	// Return the number of bytes read
	return bufSize;
}

/**************************************************************************************

	Method: WeatherInformer_GetData

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Returns: error code

	Description: Connecting to the source and extracting weather forecast data.
				 Uses the functionality of the CURL library.

**************************************************************************************/
int WeatherInformer_GetData(void * _this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;

	char errorBuffer[CURL_ERROR_SIZE];

	CURL *curl;
	CURLcode result = CURLE_RECV_ERROR;

	// Initialize the CURL object
	curl = curl_easy_init();

	if (curl)
	{
		// Configure the CURL connection
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
		curl_easy_setopt(curl, CURLOPT_URL, wInformer->httpReq);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wInformer->DataWriter);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &(wInformer->buffer));

		// Extract data
		result = curl_easy_perform(curl);

		if (result == CURLE_OK)
		{
			// If data is received, append a null character to the end of the buffer
			wInformer->buffer = (char*)realloc(wInformer->buffer, bufSize + 1);
			bufSize += 1;
			wInformer->buffer[bufSize - 1] = '\0';
			wInformer->bufSize = bufSize;
		}
	}

	// Clean up CURL object
	curl_easy_cleanup(curl);

	// Return the error code
	wInformer->error = (int)result;
	return wInformer->error;
}

/**************************************************************************************

	Method: WeatherInformer_SetCityName

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Setting the city name

**************************************************************************************/
void WeatherInformer_SetCityName(void* _this, char *cityName)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;

	strcpy(wInformer->wInfo->cityName, cityName);
}

/**************************************************************************************

	Method: WeatherInformer_SetSourceName

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Setting the source name

**************************************************************************************/
void WeatherInformer_SetSourceName(void* _this, char *sourceName)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;

	strcpy(wInformer->sourceName, sourceName);
}

/**************************************************************************************

	Method: WeatherInformer_Show

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Displaying weather forecast data on the screen

**************************************************************************************/
void WeatherInformer_Show(void* _this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;

	if (wInformer->error)
	{
		printf("------------------------------------------------------------------------\n");
		printf("\n No weather forecast data from source: %s\n", wInformer->sourceName);
		printf("------------------------------------------------------------------------\n\n");
	}
	else
	{
		printf("\n-------Weather forecast for tomorrow: -------\n");
		printf("  Source:  %s\n", wInformer->sourceName);
		wInformer->wInfo->Show(wInformer->wInfo, wInformer->scale);
	}
}

/**************************************************************************************

	Method: WeatherInformer_CelsiusToFahrenheit

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Converting temperature data from Celsius scale to Fahrenheit scale

**************************************************************************************/
void WeatherInformer_CelsiusToFahrenheit(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	wInformer->wInfo->minTempF = (wInformer->wInfo->minTempC) * (5.0 / 9.0) + 32.0;
	wInformer->wInfo->maxTempF = (wInformer->wInfo->maxTempC) * (5.0 / 9.0) + 32.0;
}

/**************************************************************************************

	Method: WeatherInformer_FahrenheitToCelsius

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Converting temperature data from Fahrenheit scale to Celsius scale

**************************************************************************************/
void WeatherInformer_FahrenheitToCelsius(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	wInformer->wInfo->minTempC = ((wInformer->wInfo->minTempF) - 32.0) * (5.0 / 9.0);
	wInformer->wInfo->maxTempC = ((wInformer->wInfo->maxTempF) - 32.0) * (5.0 / 9.0);
}

/**************************************************************************************

	Method: WeatherInformer_CreateHttpRequest_WWO

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Forming a request to the data source from worldweatheronline

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_WWO(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer->httpReq, "http://free.worldweatheronline.com/feed/weather.ashx?format=json&date=tomorrow&key=52a9e8e499131425132901&q=");
	strcat(wInformer->httpReq, wInformer->wInfo->cityName);
}

/**************************************************************************************

	Method: WeatherInformer_ParseJsonData_WWO

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Parsing data received from the source in JSON format from worldweatheronline

**************************************************************************************/
void WeatherInformer_ParseJsonData_WWO(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;

	if (wInformer->error) return;

	char * errorData = "{ \"data\": { \"error\": [ {\"msg\": \"Unable to find any matching weather location to the query submitted!\" } ] }}";

	// Checking for possible error
	if (strcmp(wInformer->buffer, errorData) == 0)
	{
		wInformer->error = CURLE_RECV_ERROR;
		return;
	}

	// Parsing JSON data
	json_object * jobj = json_tokener_parse(wInformer->buffer);
	JSONParse(jobj, wInformer->wInfo, "tempMinC", "tempMaxC", "tempMinF", "tempMaxF", "pressure", "humidity", "windspeedKmph", "winddirection", "cloudcover", "precipMM");

	// Convert temperature values if necessary
	if (wInformer->scale == C_SCALE)
	{
		if ((wInformer->wInfo->minTempC != 0) && (wInformer->wInfo->maxTempC != 0) &&
			(wInformer->wInfo->minTempF == 0) && (wInformer->wInfo->maxTempF == 0))
			wInformer->CelsiusToFahrenheit(wInformer);
	}
	else if (wInformer->scale == F_SCALE)
	{
		if ((wInformer->wInfo->minTempF != 0) && (wInformer->wInfo->maxTempF != 0) &&
			(wInformer->wInfo->minTempC == 0) && (wInformer->wInfo->maxTempC == 0))
			wInformer->FahrenheitToCelsius(wInformer);
	}
}


/**************************************************************************************

	Method: WeatherInformer_CreateHttpRequest_OW

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Forming a request to the data source from open-weather.ru

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_OW(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer->httpReq, "http://open-weather.ru/api/weather/");
	strcat(wInformer->httpReq, wInformer->wInfo->cityName);
	strcat(wInformer->httpReq, "/?key=063e5ee07278d771043590a169bc0983645ff814f1ed39339d71f2c7");
}

/**************************************************************************************

	Method: WeatherInformer_ParseJsonData_OW

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Parsing data received from the source in JSON format from open-weather.ru

**************************************************************************************/
void WeatherInformer_ParseJsonData_OW(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;

	if (wInformer->error) return;

	char * errorData = "Not Found";
	
	// Checking for possible error
	if (strcmp(wInformer->buffer, errorData) == 0)
	{
		wInformer->error = CURLE_RECV_ERROR;
		return;
	}
	
	// Parsing JSON data
	json_object * jobj = json_tokener_parse(wInformer->buffer);
	JSONParse(jobj, wInformer->wInfo, "minimal_temperature", "maximal_temperature", "minimal_temperature_f", "maximal_temperature_f", "pressure", "humidity", "wind_speed", "wind_direction", "cloudcover", "precipitation");
	
	// Convert temperature values if necessary
	if (wInformer->scale == C_SCALE)
	{
		if ((wInformer->wInfo->minTempC != 0) && (wInformer->wInfo->maxTempC != 0) &&
			(wInformer->wInfo->minTempF == 0) && (wInformer->wInfo->maxTempF == 0))
			wInformer->CelsiusToFahrenheit(wInformer);
	}
	else if (wInformer->scale == F_SCALE)
	{
		if ((wInformer->wInfo->minTempF != 0) && (wInformer->wInfo->maxTempF != 0) &&
			(wInformer->wInfo->minTempC == 0) && (wInformer->wInfo->maxTempC == 0))
			wInformer->FahrenheitToCelsius(wInformer);
	}
}

/**************************************************************************************

	Method: WeatherInformer_CreateHttpRequest_PBO

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Forming a request to the data source from pogoda.bilets.org

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_PBO(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer->httpReq, "http://pogoda.bilets.org/api/weather/");
	strcat(wInformer->httpReq, wInformer->wInfo->cityName);
	strcat(wInformer->httpReq, "/?key=063e5ee07278d771043590a169bc0983645ff814f1ed39339d71f2c7");
}

/**************************************************************************************

	Method: WeatherInformer_ParseJsonData_PBO

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Parsing data received from the source in JSON format from pogoda.bilets.org

**************************************************************************************/
void WeatherInformer_ParseJsonData_PBO(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;

	if (wInformer->error) return;

	char * errorData = "Not Found";
	// Checking for possible error
	if (strcmp(wInformer->buffer, errorData) == 0)
	{
		wInformer->error = CURLE_RECV_ERROR;
		return;
	}
	// Parsing JSON data
	json_object * jobj = json_tokener_parse(wInformer->buffer);
	JSONParse(jobj, wInformer->wInfo, "minimal_temperature", "maximal_temperature", "minimal_temperature_f", "maximal_temperature_f", "pressure", "humidity", "wind_speed", "wind_direction", "cloudcover", "precipitation");
	// Convert temperature values if necessary
	if (wInformer->scale == C_SCALE)
	{
		if ((wInformer->wInfo->minTempC != 0) && (wInformer->wInfo->maxTempC != 0) &&
			(wInformer->wInfo->minTempF == 0) && (wInformer->wInfo->maxTempF == 0))
			wInformer->CelsiusToFahrenheit(wInformer);
	}
	else if (wInformer->scale == F_SCALE)
	{
		if ((wInformer->wInfo->minTempF != 0) && (wInformer->wInfo->maxTempF != 0) &&
			(wInformer->wInfo->minTempC == 0) && (wInformer->wInfo->maxTempC == 0))
			wInformer->FahrenheitToCelsius(wInformer);
	}
}

/**************************************************************************************

	Method: WeatherInformer_CreateHttpRequest_WG

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Forming a request to the data source from api.wunderground.com

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_WG(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer->httpReq, "http://api.wunderground.com/api/df4498d31345157d/conditions/forecast/q/");
	strcat(wInformer->httpReq, wInformer->wInfo->cityName);
	strcat(wInformer->httpReq, ".json");
}

/**************************************************************************************

	Method: WeatherInformer_ParseJsonData_WG

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Parsing data received from the source in JSON format from api.wunderground.com

**************************************************************************************/
void WeatherInformer_ParseJsonData_WG(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;

	if (wInformer->error) return;
	// Checking for possible error
	char * errorData = "No cities match your search query";

	if (strstr(wInformer->buffer, errorData) != NULL)
	{
		wInformer->error = CURLE_RECV_ERROR;
		return;
	}
	// Parsing JSON data
	json_object * jobj = json_tokener_parse(wInformer->buffer);
	JSONParse(jobj, wInformer->wInfo, "celsius", "celsius", "fahrenheit", "fahrenheit", "pressure", "maxhumidity", "maxwind", "wind_direction", "cloudcover", "precipitation");
	// Convert temperature values if necessary
	if (wInformer->scale == C_SCALE)
	{
		if ((wInformer->wInfo->minTempC != 0) && (wInformer->wInfo->maxTempC != 0) &&
			(wInformer->wInfo->minTempF == 0) && (wInformer->wInfo->maxTempF == 0))
			wInformer->CelsiusToFahrenheit(wInformer);
	}
	else if (wInformer->scale == F_SCALE)
	{
		if ((wInformer->wInfo->minTempF != 0) && (wInformer->wInfo->maxTempF != 0) &&
			(wInformer->wInfo->minTempC == 0) && (wInformer->wInfo->maxTempC == 0))
			wInformer->FahrenheitToCelsius(wInformer);
	}
}


/**************************************************************************************

	Method: WeatherInformer_CreateHttpRequest_GA

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Forming a request to the data source from www.google.com

**************************************************************************************/
void WeatherInformer_CreateHttpRequest_GA(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;
	strcpy(wInformer->httpReq, "http://www.google.com/ig/api?weather=");
	strcat(wInformer->httpReq, wInformer->wInfo->cityName);
	strcat(wInformer->httpReq, "&format=json");
}

/**************************************************************************************

	Method: WeatherInformer_ParseJsonData_GA

	Parameters:
				_this - pointer to an object of the WeatherInformer class

	Description: Parsing data received from the source in JSON format from www.google.com

**************************************************************************************/
void WeatherInformer_ParseJsonData_GA(void *_this)
{
	PWEATHER_INFORMER wInformer = (PWEATHER_INFORMER)_this;

	if (wInformer->error) return;

	char * errorData = "We're sorry...";
	
	// Checking for possible error
	if (strstr(wInformer->buffer, errorData) != NULL)
	{
		wInformer->error = CURLE_RECV_ERROR;
		return;
	}
	
	// Parsing JSON data
	json_object * jobj = json_tokener_parse(wInformer->buffer);
	JSONParse(jobj, wInformer->wInfo, "temp_c", "temp_c", "temp_f", "temp_f", "pressure", "humidity", "wind_speed", "wind_direction", "cloudcover", "precipitation");
	
	// Convert temperature values if necessary
	if (wInformer->scale == C_SCALE)
	{
		if ((wInformer->wInfo->minTempC != 0) && (wInformer->wInfo->maxTempC != 0) &&
			(wInformer->wInfo->minTempF == 0) && (wInformer->wInfo->maxTempF == 0))
			wInformer->CelsiusToFahrenheit(wInformer);
	}
	else if (wInformer->scale == F_SCALE)
	{
		if ((wInformer->wInfo->minTempF != 0) && (wInformer->wInfo->maxTempF != 0) &&
			(wInformer->wInfo->minTempC == 0) && (wInformer->wInfo->maxTempC == 0))
			wInformer->FahrenheitToCelsius(wInformer);
	}
}

/**************************************************************************************

	Function: JSONParse

	Parameters: jobj - json parsing object
				wInfo - weather information
				minTempCName - name of the searched parameter for minimum temperature (Celsius)
				maxTempCName - name of the searched parameter for maximum temperature (Celsius)
				minTempFName - name of the searched parameter for minimum temperature (Fahrenheit)
				maxTempFName - name of the searched parameter for maximum temperature (Fahrenheit)
				preasureName - name of the searched parameter for pressure
				humidityName - name of the searched parameter for humidity
				windSpeedName - name of the searched parameter for wind speed
				windDirectionName - name of the searched parameter for wind direction
				cloudinessName - name of the searched parameter for cloudiness
				precipitationName - name of the searched parameter for precipitation

	Description: Syntax analysis of JSON data

**************************************************************************************/
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
			   char * precipitationName)
{
	// Get JSON data type
	enum json_type type = json_object_get_type(jobj);

	if (type == json_type_array)
	{
		// Processing JSON array
		PasreJSONArray(jobj, NULL, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName, preasureName, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
	}
	else
	{
		// Iterate over JSON objects
		json_object_object_foreach(jobj, key, val)
		{

			// Get JSON data type
			enum json_type type = json_object_get_type(val);

			switch (type)
			{
				case json_type_double:
				case json_type_int:
					// Processing JSON record
				case json_type_string: CheckJSONValue(val, key, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName, preasureName, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
					break;
					// Processing JSON object
				case json_type_object: jobj = json_object_object_get(jobj, key);
					JSONParse(jobj, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName, preasureName, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
					break;
					// Processing JSON array
				case json_type_array:  PasreJSONArray(jobj, key, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName, preasureName, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
					break;
			}
		}
	}
}


/**************************************************************************************

	Function: PasreJSONArray

	Parameters: jobj - json parsing object
				key - parameter name
				wInfo - weather information
				minTempCName - name of the searched parameter for minimum temperature (Celsius)
				maxTempCName - name of the searched parameter for maximum temperature (Celsius)
				minTempFName - name of the searched parameter for minimum temperature (Fahrenheit)
				maxTempFName - name of the searched parameter for maximum temperature (Fahrenheit)
				preasureName - name of the searched parameter for pressure
				humidityName - name of the searched parameter for humidity
				windSpeedName - name of the searched parameter for wind speed
				windDirectionName - name of the searched parameter for wind direction
				cloudinessName - name of the searched parameter for cloudiness
				precipitationName - name of the searched parameter for precipitation

	Description: Syntax analysis of JSON array

**************************************************************************************/
void PasreJSONArray(json_object *jobj, char *key,
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
	json_object *jarray = jobj;
	// Get JSON array
	if (key) jarray = json_object_object_get(jobj, key);
	if (!jarray) return;

	// Get array length
	int arraylen = json_object_array_length(jarray);

	// Process array elements
	for (int i = 0; i < arraylen; i++)
	{
		json_object * jvalue = json_object_array_get_idx(jarray, i);
		// Get JSON data type
		enum json_type type = json_object_get_type(jvalue);

		// Process array element according to type
		if (type == json_type_array)
			PasreJSONArray(jvalue, NULL, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName, preasureName, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
		else
			if (type != json_type_object)
			{
				struct lh_entry *entry = json_object_get_object(jvalue)->head;
				char *key = (char*)entry->k;
				CheckJSONValue(jvalue, key, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName, preasureName, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
			}
			else
				JSONParse(jvalue, wInfo, minTempCName, maxTempCName, minTempFName, maxTempFName, preasureName, humidityName, windSpeedName, windDirectionName, cloudinessName, precipitationName);
	}
}

/**************************************************************************************

	Function: CheckJSONValue

	Parameters: jobj - json parsing object
				key - parameter name
				wInfo - weather information
				minTempCName - name of the searched parameter for minimum temperature (Celsius)
				maxTempCName - name of the searched parameter for maximum temperature (Celsius)
				minTempFName - name of the searched parameter for minimum temperature (Fahrenheit)
				maxTempFName - name of the searched parameter for maximum temperature (Fahrenheit)
				preasureName - name of the searched parameter for pressure
				humidityName - name of the searched parameter for humidity
				windSpeedName - name of the searched parameter for wind speed
				windDirectionName - name of the searched parameter for wind direction
				cloudinessName - name of the searched parameter for cloudiness
				precipitationName - name of the searched parameter for precipitation

	Description: Check the value of a specific record after
				 syntax analysis of JSON

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
	// Get JSON data type
	enum json_type type = json_object_get_type(jobj);

	// Read data according to type and parameter names
	switch (type)
	{
		case json_type_double: if (strcmp(key, minTempCName) == 0) wInfo->minTempC = (int)json_object_get_double(jobj);
			if (strcmp(key, maxTempCName) == 0) wInfo->maxTempC = (int)json_object_get_double(jobj);
			if (strcmp(key, minTempFName) == 0) wInfo->minTempF = (int)json_object_get_double(jobj);
			if (strcmp(key, maxTempFName) == 0) wInfo->maxTempF = (int)json_object_get_double(jobj);
			if (strcmp(key, preasureName) == 0) wInfo->preasure = (int)json_object_get_double(jobj);
			if (strcmp(key, humidityName) == 0) wInfo->humidity = (int)json_object_get_double(jobj);
			if (strcmp(key, windSpeedName) == 0) wInfo->windSpeed = (int)json_object_get_double(jobj);
			if (strcmp(key, cloudinessName) == 0) wInfo->cloudiness = (int)json_object_get_double(jobj);
			if (strcmp(key, precipitationName) == 0) wInfo->precipitation = (int)json_object_get_double(jobj);
			break;

		case json_type_int:    if (strcmp(key, minTempCName) == 0) wInfo->minTempC = json_object_get_int(jobj);
			if (strcmp(key, maxTempCName) == 0) wInfo->maxTempC = json_object_get_int(jobj);
			if (strcmp(key, minTempFName) == 0) wInfo->minTempF = json_object_get_int(jobj);
			if (strcmp(key, maxTempFName) == 0) wInfo->maxTempF = json_object_get_int(jobj);
			if (strcmp(key, preasureName) == 0) wInfo->preasure = json_object_get_int(jobj);
			if (strcmp(key, humidityName) == 0) wInfo->humidity = json_object_get_int(jobj);
			if (strcmp(key, windSpeedName) == 0) wInfo->windSpeed = json_object_get_int(jobj);
			if (strcmp(key, cloudinessName) == 0) wInfo->cloudiness = json_object_get_int(jobj);
			if (strcmp(key, precipitationName) == 0) wInfo->precipitation = json_object_get_int(jobj);
			break;

		case json_type_string: if (strcmp(key, minTempCName) == 0)  wInfo->minTempC = atoi(json_object_get_string(jobj));
			if (strcmp(key, maxTempCName) == 0) wInfo->maxTempC = atoi(json_object_get_string(jobj));
			if (strcmp(key, minTempFName) == 0) wInfo->minTempF = atoi(json_object_get_string(jobj));
			if (strcmp(key, maxTempFName) == 0) wInfo->maxTempF = atoi(json_object_get_string(jobj));
			if (strcmp(key, preasureName) == 0) wInfo->preasure = atoi(json_object_get_string(jobj));
			if (strcmp(key, humidityName) == 0) wInfo->humidity = atoi(json_object_get_string(jobj));
			if (strcmp(key, windSpeedName) == 0) wInfo->windSpeed = atoi(json_object_get_string(jobj));
			if (strcmp(key, cloudinessName) == 0) wInfo->cloudiness = atoi(json_object_get_string(jobj));
			if (strcmp(key, precipitationName) == 0) wInfo->precipitation = atoi(json_object_get_string(jobj));
			if (strcmp(key, windDirectionName) == 0) strcpy(wInfo->windDirection, json_object_get_string(jobj));
			break;
	}
}

