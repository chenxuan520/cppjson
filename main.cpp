#include <iostream>
#include "./json.h"
using namespace std;
int main()
{
	char temp[4000]={0};
	FileGet::getFileMsg("./a.json",temp,2000);
	//create tree for json text
	Json json(temp);
	if(json.lastError()!=NULL)
	{
		printf("%s\n",json.lastError());
		return -1;
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
	printf("%s\n",getStr);
	return 0;
}

