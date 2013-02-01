/**************************************************************************************
   
	������ : Weather

	������ : Weather.cpp

	�������� : ��������� �������� ������� ������ �� ������ �� ���� ����������, 
	           ������� ������������� free weather api � �������� ������ � �������
			   json. ������������ ���������� ������������ �� ������ ��������� ��������,
			   � ����� ���������� �������. ���� ������������ ��������, �� �����
			   �������� �������������� ������ � �������� json �/��� xml.

**************************************************************************************/

#include "stdafx.h"
#include <string.h>
#include "WeatherInformerContainer.h"

// ����������� ����������� �������� 
// ���������� ��������� ������
#define ARG_CHECK_OK  0
#define ARG_CHECK_ERR 1


/**************************************************************************************
   
    ������� :  Usage 
	
	�������� : ���������� ������������ ������� ������������� ���������.

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
   
    ������� :  ArgCheck 
	
	��������� : 
				argc - ���������� ����������
				argv - ����� ����������
				scale - ��� ������������ ����� ���������� (�������/���������)
				json_out - ����� �� ������ ����� � ������� json
				xml_out - ����� �� ������ ����� � ������� xml

	���������� : ��� ������

	�������� : ��������� ��������� ��������� ������.

**************************************************************************************/
int ArgCheck(int argc, char ** argv, int * scale, int * json_out, int * xml_out)
{
	// �������� ���������� ����������
	if ((argc < 3) || (argc > 5)) return ARG_CHECK_ERR;
	
	// �������� ����� ����������
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
	 
	// �������� ��������������� ������ json/xml
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

    // �������� ��������������� ������ json/xml
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
   
    ������� :  main 

	��������� : 
				argc - ���������� ���������� ��������� ������
				argv - ����� ���������� ��������� ������

	���������� : ��� ������

	�������� : �������� ������� ���������. ����� ����� � ���������.

**************************************************************************************/
int main(int argc, char ** argv)
{
	int scale = 0;
	int json_out = 0;
	int xml_out = 0;

	// �������� ����������
	if (ArgCheck(argc, argv, &scale, &json_out, &xml_out) != ARG_CHECK_OK)
	{
		Usage();
		return 0;
	}

	printf("Please wait, working ... \n\n");

	// ������ ��������� �������� ����������
	PWEATHER_INFORMER_CONTAINER wInfoContainer = WeatherInformerContainer_Constructor(argv[1], scale);
	
	// �������� ������ �� ����������
	wInfoContainer -> GetData(wInfoContainer);

	// ����������� ������
	wInfoContainer -> ParseData(wInfoContainer);

	// ������� ���������� ����������
	wInfoContainer -> CreateTotalData(wInfoContainer);

	// ���������� ���������� �� �����
	wInfoContainer -> Show(wInfoContainer);

	// ���� �����, �� ������� ���������� � ������� json
	if (json_out)
		wInfoContainer -> JsonOutput(wInfoContainer);
	
	// ���� �����, �� ������� ���������� � ������� xml
	if (xml_out)
		wInfoContainer -> XmlOutput(wInfoContainer);

	// ������� ���������
	WeatherInformerContainer_Destructor(wInfoContainer);
	
	return 0;
}

