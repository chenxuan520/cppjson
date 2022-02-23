#include <iostream>
#include "./json.h"
using namespace std;
void funtion()
{
	char* temp=(char*)malloc(sizeof(char)*600);
	printf("please input the json file to read:");
	scanf("%600s",temp);
	FileGet file;
	//create tree for json text
	Json json(file.getFileBuff(temp));
	if(json.lastError()!=NULL)
	{
		printf("%s\n",json.lastError());
		return ;
	}
	// fmt print the tree result
	const char* result=json.formatPrint(json.getRootObj());
	printf("%s\n",result);
	// find the key value
	if(json["empt"]!=NULL)
		for(unsigned i=0;i<json["empt"]->arr.size();i++)
			printf("%f\n",json["empt"]->arr[i]->floVal);
	if(json["first Name"]!=NULL)
		printf("%s\n",json["first Name"]->strVal.c_str());
	//create a new json text
	char* getStr=json.createObject();
	json.addKeyVal(getStr,Json::STRING,"try","exmaple");
	json.addKeyVal(getStr,"null",nullptr);
	//create an array to json
	int arrInt[5]={1,2,3,4,5};
	char* arr=json.createArray(Json::INT,5,arrInt);
	json.addKeyVal(getStr,Json::ARRAY,"arr",arr);
	json.addKeyVal(getStr,Json::EMPTY,"empty",NULL);
	// an others way to add
	json.addKeyVal(getStr,"new","string");
	json.addKeyVal(getStr,"file name",(const char*)temp);
	json.addKeyVal(getStr,"newarr",arr);
	json.addKeyVal(getStr,"newnum",12);
	printf("%s\n",getStr);
	free(temp);
	//create json in begin
	Json json2={
		{"float",12.3},
		{"int",23},
		{"bool",true},
		{"str","string"},
		{"null",nullptr},
		{"arrFloat",{2.3,8.9,2.3}},
		{"arrBool",{true,false}},
		{"arrStr",{"chenxuan","create"}}
	};
	printf("new create:\n%s\n",json2());
	//after begin you can continue add ketValue
	json2.addKeyVal(json2(),Json::STRING,"temp","chenxuan");
	json2("new arr")=arr;
	printf("new create:\n%s\n",json2());
}
int main()
{
	funtion();
	return 0;
}

