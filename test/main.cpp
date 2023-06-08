/***********************************************
* Author: chenxuan-1607772321@qq.com
* change time:2023-04-03 09:37:40
* description:run test file
* example: run make in shell
***********************************************/
#include "../json.h"
#include "test.h"
#include <fstream>
#include <vector>

TEST(JsonAnalysis,Comprehensive){
	//read from file
	std::ifstream in("./comprehensive_test.json", std::ios::in);
	std::istreambuf_iterator<char> beg(in), end;
	std::string strdata(beg, end);
	in.close();
	//create tree for json text
	cppweb::Json json(strdata.c_str());
	if(json.lastError()!=NULL){
		FATAL(json.lastError());
		return;
	}
	//space and bukect check
	auto root=json.getRootObj();
	if (root["first Name"]==json.npos||root["first Name"].strVal!="Brett{)({}\"") {
		FATAL(root["first Name"].strVal);
		return;
	}
	//emjoy check
	if (root["陈 玄"]==json.npos||root["陈 玄"].strVal!="哈哈哈👇") {
		FATAL(root["陈 玄"].strVal);
		return;
	}
	//json object check
	if (root["obj"]==json.npos||root["obj"]["ad"].strVal!="op") {
		FATAL(json.formatPrint(root["obj"]));
		return;
	}
	//int check
	if (root["money_"]==json.npos||root["money_"].intVal!=100) {
		FATAL(root["money_"].intVal);
		return;
	}
}

TEST(JsonCreate,BaseType){
	cppweb::Json json;
	//create BaseType
	json["int"]=1;
	json["double"]=1.3;
	json["bool"]=false;
	json["null"]=nullptr;
	json["arr_int"]=(std::vector<int>()={1,2,3});
	//analysis self
	auto flag=json.analyseText(json());
	MUST_TRUE(flag, "analyseText self wrong");
	auto root=json.getRootObj();

	if (root["sfds"]!=cppweb::Json::npos) {
		ERROR("find not exist");
	}
	MUST_EQUAL(root["int"].intVal, 1);
	MUST_EQUAL(root["double"].floVal, 1.3);
	MUST_EQUAL(root["bool"].boolVal, false);
	MUST_EQUAL(root["null"].type,cppweb::Json::EMPTY);
	MUST_EQUAL(root["arr_int"].arr.size(), 3);
	for (int i = 0; i < 3; i++) {
		MUST_EQUAL(root["arr_int"].arr[i]->intVal, i+1);
	}
}

ARGC_FUNC{
	TESTSTDOUT("CPPJSON TESTING...");
}

RUN
