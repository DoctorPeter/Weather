/**************************************************************************************
   
	Проект : Weather

	Модуль : Weather.cpp

	Описание : Программа получает прогноз погоды на зватра из пяти источников, 
	           которые предоставляют free weather api и передают данные в формате
			   json. Пользователю информация отображается по кажому источнику отдельно,
			   а также усреднённый прогноз. Если пользователь пожелает, он может
			   получить результирующие данные в форматах json и/или xml.

**************************************************************************************/

#include "stdafx.h"
#include <string.h>
#include "WeatherInformerContainer.h"

// определение результатов проверки 
// аргументов командной строки
#define ARG_CHECK_OK  0
#define ARG_CHECK_ERR 1


/**************************************************************************************
   
    Функция :  Usage 
	
	Описание : Отображает пользователю правила использования программы.

**************************************************************************************/
void Usage()
{
	printf("Usage:\n");
	printf("       weather <city name> <-C/-F> [-J] [-X] \n\n");
	printf("Examples: \n");
	printf("          weather Moscow -C \n");
	printf("          weather Kiev -F \n");
	printf("          weather London -C -J\n");
	printf("          weather Paris -F -X\n\n");
	printf("If city name have more than one word then use symbol + \n\n");
	printf("Example: \n");
	printf("          weather New+York -C -X\n");

}

/**************************************************************************************
   
    Функция :  ArgCheck 
	
	Параметры : 
				argc - количество параметров
				argv - масив параметров
				scale - тип используемой шкалы температур (Цельсий/Фаренгейт)
				json_out - нужно ли делать вывод в формате json
				xml_out - нужно ли делать вывод в формате xml

	Возвращает : код ошибки

	Описание : Проверяет аргументы командной строки.

**************************************************************************************/
int ArgCheck(int argc, char ** argv, int * scale, int * json_out, int * xml_out)
{
	// Проверка количества параметров
	if ((argc < 3) || (argc > 5)) return ARG_CHECK_ERR;
	
	// Проверка шкалы температур
	if (argc >= 3)
	{
		if (strcmp(argv[2], "-C") == 0) 
			*scale = C_SCALE;
		else
		if (strcmp(argv[2], "-F") == 0) 
			*scale = F_SCALE;
		else
		   return ARG_CHECK_ERR;
	
	}
	 
	// Проверка дополнительного вывода json/xml
	if (argc >= 4)
	{
		if (strcmp(argv[3], "-J") == 0) 
			*json_out = 1;
		else
		if (strcmp(argv[3], "-X") == 0) 
			*xml_out = 1;
		else
		   return ARG_CHECK_ERR;
	}

    // Проверка дополнительного вывода json/xml
	if (argc == 5)
	{
		if (strcmp(argv[4], "-J") == 0) 
			*json_out = 1;
		else
		if (strcmp(argv[4], "-X") == 0) 
			*xml_out = 1;
		else
			return ARG_CHECK_ERR;
	}

	return ARG_CHECK_OK;
}


/**************************************************************************************
   
    Функция :  main 

	Параметры : 
				argc - количество параметров командной строки
				argv - масив параметров командной строки

	Возвращает : код ошибки

	Описание : Основная функция программы. Точка входа в программу.

**************************************************************************************/
int main(int argc, char ** argv)
{
	int scale = 0;
	int json_out = 0;
	int xml_out = 0;

	// Проверка параметров
	if (ArgCheck(argc, argv, &scale, &json_out, &xml_out) != ARG_CHECK_OK)
	{
		Usage();
		return 0;
	}

	printf("Please wait, working ... \n\n");

	// Создаём контернер погодных информеров
	PWEATHER_INFORMER_CONTAINER wInfoContainer = WeatherInformerContainer_Constructor(argv[1], scale);
	
	// Получаем данных от источников
	wInfoContainer -> GetData(wInfoContainer);

	// Анализируем данные
	wInfoContainer -> ParseData(wInfoContainer);

	// Создаем усреднённую информацию
	wInfoContainer -> CreateTotalData(wInfoContainer);

	// Отображаем результаты на экран
	wInfoContainer -> Show(wInfoContainer);

	// Если нужно, то выводим результаты в формате json
	if (json_out)
		wInfoContainer -> JsonOutput(wInfoContainer);
	
	// Если нужно, то выводим результаты в формате xml
	if (xml_out)
		wInfoContainer -> XmlOutput(wInfoContainer);

	// Удаляем контейнер
	WeatherInformerContainer_Destructor(wInfoContainer);
	
	return 0;
}

