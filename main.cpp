#include <iostream>
#include "./json.h"
using namespace std;
void funtion()
{
	char temp[4000]={0};
	FileGet::getFileMsg("./a.json",temp,2000);
	//create tree for json text
	Json json(temp);
	if(json.lastError()!=NULL)
	{
		printf("%s\n",json.lastError());
		return ;
	}
	// fmt print the tree result
	const char* result=json.formatPrint(json.getRootObj(),2000);
	printf("%s\n",result);
	// find the key value
	if(json["empt"]!=NULL)
		for(unsigned i=0;i<json["empt"]->arr.size();i++)
			printf("%f\n",json["empt"]->arr[i]->floVal);
	if(json["first Name"]!=NULL)
		printf("%s\n",json["first Name"]->strVal.c_str());
	//create a new json text
	char* getStr=json.createObject(200);
	json.addKeyVal(getStr,Json::STRING,"try","exmaple");
	//create an array to json
	int arrInt[5]={1,2,3,4,5};
	char* arr=json.createArray(200,Json::INT,5,arrInt);
	json.addKeyVal(getStr,Json::ARRAY,"arr",arr);
	json.addKeyVal(getStr,Json::EMPTY,"empty");
	printf("%s\n",getStr);
}
int main()
{
	funtion();
	return 0;
}

