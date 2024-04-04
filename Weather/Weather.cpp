/**************************************************************************************

	Project : Weather

	Module : Weather.cpp

	Description : The program retrieves weather forecasts for tomorrow from five sources
				  that provide free weather APIs and pass data in JSON format. The information
				  is displayed to the user separately for each source, as well as an averaged forecast.
				  If the user wishes, they can get the resulting data in JSON and/or XML formats.

**************************************************************************************/

#include "stdafx.h"
#include <string.h>
#include "WeatherInformerContainer.h"

// Definition of command-line argument check results
#define ARG_CHECK_OK  0
#define ARG_CHECK_ERR 1


/**************************************************************************************

	Function :  Usage

	Description : Displays the usage rules of the program to the user.

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
	printf("If the city name consists of more than one word, use the plus symbol + \n\n");
	printf("Example: \n");
	printf("          weather New+York -C -X\n");

}

/**************************************************************************************

	Function :  ArgCheck

	Parameters :
				argc - number of command-line arguments
				argv - array of command-line arguments
				scale - type of temperature scale used (Celsius/Fahrenheit)
				json_out - whether to output in JSON format
				xml_out - whether to output in XML format

	Returns : error code

	Description : Checks the command-line arguments.

**************************************************************************************/
int ArgCheck(int argc, char ** argv, int * scale, int * json_out, int * xml_out)
{
	// Check the number of parameters
	if ((argc < 3) || (argc > 5)) return ARG_CHECK_ERR;

	// Check the temperature scale
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

	// Check additional JSON/XML output
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

	// Check additional JSON/XML output
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

	Function :  main

	Parameters :
				argc - number of command-line arguments
				argv - array of command-line arguments

	Returns : error code

	Description : Main function of the program. Entry point of the program.

**************************************************************************************/
int main(int argc, char ** argv)
{
	int scale = 0;
	int json_out = 0;
	int xml_out = 0;

	// Check parameters
	if (ArgCheck(argc, argv, &scale, &json_out, &xml_out) != ARG_CHECK_OK)
	{
		Usage();
		return 0;
	}

	printf("Please wait, working ... \n\n");

	// Create a container for weather informers
	PWEATHER_INFORMER_CONTAINER wInfoContainer = WeatherInformerContainer_Constructor(argv[1], scale);

	// Get data from sources
	wInfoContainer->GetData(wInfoContainer);

	// Parse data
	wInfoContainer->ParseData(wInfoContainer);

	// Create averaged information
	wInfoContainer->CreateTotalData(wInfoContainer);

	// Display results on the screen
	wInfoContainer->Show(wInfoContainer);

	// If necessary, output results in JSON format
	if (json_out)
		wInfoContainer->JsonOutput(wInfoContainer);

	// If necessary, output results in XML format
	if (xml_out)
		wInfoContainer->XmlOutput(wInfoContainer);

	// Delete the container
	WeatherInformerContainer_Destructor(wInfoContainer);

	return 0;
}
