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
using namespace cppjson;

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
  auto flag = json.Parse(json.Dump());
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

TEST(Json, Struct) {
  struct Person {
    int name;
    double age;
    bool sex;
    std::string phone;
    Person *lover;
    std::vector<Person> family;
    std::vector<std::string> hobby;
    int ParseFromJson(cppjson::Json::Object &obj) {
      name = obj["name"].int_val;
      age = obj["age"].flo_val;
      sex = obj["sex"].bool_val;
      phone = obj["phone"].str_val;
      for (auto &now : obj["family"].arr) {
        Person temp;
        int rc = temp.ParseFromJson(*now);
        if (rc != 0) {
          return -1;
        }
        family.emplace_back(temp);
      }
      for (auto &now : obj["hobby"].arr) {
        hobby.emplace_back(now->str_val);
      }
      if (obj["lover"] != Json::npos() &&
          obj["lover"].type != cppjson::Json::EMPTY) {
        lover = new Person;
        int rc = lover->ParseFromJson(obj["lover"]);
        if (rc != 0) {
          return -1;
        }
      } else {
        lover = nullptr;
      }
      return 0;
    }
    void DumpToJson(Json::Node &node) const {
      node["name"] = name;
      node["age"] = age;
      node["sex"] = sex;
      node["phone"] = phone;
      node["hobby"] = hobby;
      node["family"] = family;
      if (lover) {
        node["lover"] = *lover;
      } else {
        node["lover"] = nullptr;
      }
    }
  };
  Person person;
  person.name = 1;
  person.age = 1.2;
  person.sex = true;
  person.phone = "123456789";
  person.lover = new Person{
      .name = 2,
      .age = 1.2,
      .sex = true,
      .phone = "123456789",
      .lover = nullptr,
      .hobby = {"play", "eat"},
  };
  person.hobby = {"play", "eat"};
  person.family = {{
      .name = 1,
      .age = 1.2,
      .sex = true,
      .phone = "123456789",
      .lover = nullptr,
      .hobby = {"play", "eat"},
  }};
  auto json_text = cppjson::Json::Dump(person);
  DEBUG(json_text);
  MUST_TRUE(json_text != "", "json_text is empty");

  Person result;
  int rc = Json::Parse(json_text, result);
  MUST_EQUAL(rc, 0);
  MUST_EQUAL(result.name, 1);
  MUST_EQUAL(result.age, 1.2);
  MUST_EQUAL(result.sex, true);
  MUST_EQUAL(result.phone, "123456789");
  MUST_TRUE(result.lover != nullptr, "lover is nullptr");
  MUST_EQUAL(result.lover->name, 2);
  MUST_EQUAL(result.lover->age, 1.2);
  MUST_EQUAL(result.lover->sex, true);
  MUST_EQUAL(result.lover->phone, "123456789");
  MUST_TRUE(result.lover->lover == nullptr, "lover->lover is not nullptr");
  MUST_EQUAL(result.lover->hobby.size(), 2);
  MUST_EQUAL(result.lover->hobby[0], "play");
  MUST_EQUAL(result.lover->hobby[1], "eat");
  MUST_EQUAL(result.hobby.size(), 2);
  MUST_EQUAL(result.hobby[0], "play");
  MUST_EQUAL(result.hobby[1], "eat");
  MUST_EQUAL(result.family.size(), 1);
  MUST_EQUAL(result.family[0].name, 1);
  MUST_EQUAL(result.family[0].age, 1.2);
  MUST_EQUAL(result.family[0].sex, true);
  MUST_EQUAL(result.family[0].phone, "123456789");
  MUST_TRUE(result.family[0].lover == nullptr, "lover is not nullptr");
  MUST_EQUAL(result.family[0].hobby.size(), 2);
  MUST_EQUAL(result.family[0].hobby[0], "play");
  MUST_EQUAL(result.family[0].hobby[1], "eat");
}

ARGC_FUNC { DEBUG("CPPJSON TESTING..."); }
