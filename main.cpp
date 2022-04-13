#include <iostream>
#include "./json.h"
using namespace std;
using namespace cppweb;
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
	auto root=json.getRootObj();
	if(root["empt"]!=Json::npos)
		for(unsigned i=0;i<root["empt"].arr.size();i++)
			printf("%f\n",root["empt"][i].floVal);
	if(root["first Name"]!=Json::npos)
		printf("%s\n",root["first Name"].strVal.c_str());
	if(root["ept"]["io"]!=Json::npos)
		printf("ept:ko:%lf\n",root["ept"]["ko"].floVal);
	//create a new json text
	auto node=json.createObject({{"status","ok"},{"arr",{1,2,3,4}},{"strarr",{"we","lpl"}}});
	printf("node:%s\n",node());
	//create an array to json
	vector<double> arrFlo={1.2,3.4,4.5};
	const char* oldStr[]={"asdf","nkjn"};
	auto arrOld=json.createArray(oldStr,2);
	vector<Json::Node> arrNode{
		{
			{"status","ok"},
		},
		{
			{"status","wrong"},
		}
	};
	node["arrarr"]=vector<vector<int>>()={{1,2,3},{4,5,6}};
	node["stdStr"]=string("koko");
	node["strOld"]="ok";
	node["null"]=nullptr;
	node["bool"]=true;
	node["arrNode"]=arrNode;
	node["Int"]=1000;
	node["double"]=1.43;
	node["boolArr"]=vector<bool>()={true,false};
	node["nodeself"]=node;
	node["obj"]={
		{"status","ok"},
		{"vector",vector<string>()={"chenxuan","is","winner"}}
	};
	printf("node:\n%s\n",node());
	/* //add node to json */
	json["node"]=node;
	json["numInt"]=1000;
	json["arrold"]=arrOld;
	json["arrint"]=vector<int>()={1,2,3};
	json["arrFlo"]=arrFlo;
	json["emp arr"]=vector<int>();
	printf("result:\n%s\n",json());
	json.analyseText(json());
	printf("\n\n%s\n",json.formatPrint(json.getRootObj()));
	free(temp);
	Json json2={
		{"float",12.3},
		{"int",23},
		{"bool",true},
		{"str","string"},
		{"null",nullptr},
		{"stdstr",string("chenxuan")},
		{"arrFloat",{2.3,8.9,2.3}},
		{"arrBool",{true,false}},
		{"arrStr",{{"chenxuan","create"},{"lover","xiaozhu"}}},
		{"node",node},
		{"arrVec",arrFlo},
		{"empty arr",vector<int>()},
		{"empty obj",{}}
	};
	printf("json2:\n%s \n",json2());
}
int main()
{
	funtion();
	return 0;
}

