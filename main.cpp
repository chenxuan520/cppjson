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
	auto node=json.createObject({{"status","ok"},{"arr",{1,2,3,4}}});
	//create an array to json
	vector<double> arrFlo={1.2,3.4,4.5};
	auto arr=json.createArray(vector<string>()={"str1","str2"});
	const char* oldStr[]={"asdf","nkjn"};
	auto arrOld=json.createArray(oldStr,2);
	json.changeSetting(2);
	node("str")="ok";
	node("null")=nullptr;
	node("bool")=true;
	node("Int")=1000;
	node("double")=1.43;
	node("boolArr")=vector<bool>()={true,false};
	node("nodeself")=node;
	json("node")=node;
	json("numInt")=1000;
	json("arr")=arr;
	json("arrold")=arrOld;
	json("arrint")=vector<int>()={1,2,3};
	json("arrFlo")=arrFlo;
	printf("result:%s\n",json());
}
int main()
{
	funtion();
	return 0;
}

