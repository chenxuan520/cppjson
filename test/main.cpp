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

TEST(JsonAnalysis, Comprehensive) {
  // read from file
  std::ifstream in("./comprehensive_test.json", std::ios::in);
  std::istreambuf_iterator<char> beg(in), end;
  std::string strdata(beg, end);
  in.close();
  // create tree for json text
  cppjson::Json json(strdata.c_str());
  if (json.err_msg() != NULL) {
    FATAL(json.err_msg());
    return;
  }
  // space and bukect check
  auto root = json.GetRootObj();
  if (root["first Name"] == json.npos() ||
      root["first Name"].str_val != "Brett{)({}\"") {
    FATAL(root["first Name"].str_val);
    return;
  }
  // emjoy check
  if (root["陈 玄"] == json.npos() || root["陈 玄"].str_val != "哈哈哈👇") {
    FATAL(root["陈 玄"].str_val);
    return;
  }
  // json object check
  if (root["obj"] == json.npos() || root["obj"]["ad"].str_val != "op") {
    FATAL(json.FormatPrint(root["obj"]));
    return;
  }
  // int check
  if (root["money_"] == json.npos() || root["money_"].int_val != 100) {
    FATAL(root["money_"].int_val);
    return;
  }
}

TEST(JsonCreate, BaseType) {
  cppjson::Json json;
  // create BaseType
  json["int"] = 1;
  json["double"] = 1.3;
  json["bool"] = false;
  json["null"] = nullptr;
  json["arr_int"] = (std::vector<int>() = {1, 2, 3});
  // analysis self
  auto flag = json.Parse(json());
  MUST_TRUE(flag, "analyseText self wrong");
  auto root = json.GetRootObj();

  if (root["sfds"] != cppjson::Json::npos()) {
    ERROR("find not exist");
  }
  MUST_EQUAL(root["int"].int_val, 1);
  MUST_EQUAL(root["double"].flo_val, 1.3);
  MUST_EQUAL(root["bool"].bool_val, false);
  MUST_EQUAL(root["null"].type, cppjson::Json::EMPTY);
  MUST_EQUAL(root["arr_int"].arr.size(), 3);
  for (int i = 0; i < 3; i++) {
    MUST_EQUAL(root["arr_int"].arr[i]->int_val, i + 1);
  }
}

ARGC_FUNC { DEBUG("CPPJSON TESTING..."); }
