/***********************************************
* Author: chenxuan-1607772321@qq.com
* change time:2023-04-03 09:37:40
* description:run test file
* example: run make in shell
***********************************************/
#include "test.h"
#include "../json.h"
#include <fstream>

TEST(Json,Comprehensive){
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

RUN
