#include "./json.h"
#include <fstream>
#include <iostream>
using namespace std;
using namespace cppjson;
void funtion() {
  printf("please input the json file to read:");
  string name;
  cin >> name;
  std::ifstream in(name, std::ios::in);
  std::istreambuf_iterator<char> beg(in), end;
  std::string strdata(beg, end);
  in.close();
  // create tree for json text
  Json json(strdata.c_str());
  if (json.err_msg() != NULL) {
    printf("%s\n", json.err_msg());
    return;
  }
  // fmt print the tree result
  const char *result = json.FormatPrint(json.GetRootObj());
  printf("%s\n", result);
  // find the key value
  auto root = json.GetRootObj();
  if (root["empt"] != Json::npos())
    for (unsigned i = 0; i < root["empt"].arr.size(); i++)
      printf("%f\n", root["empt"][i].flo_val);
  if (root["first Name"] != Json::npos())
    printf("%s\n", root["first Name"].str_val.c_str());
  if (root["ept"]["io"] != Json::npos())
    printf("ept:ko:%lf\n", root["ept"]["ko"].flo_val);
  // create a new json text
  auto node = json.CreateObject(
      {{"status", "ok"}, {"arr", {1, 2, 3, 4}}, {"strarr", {"we", "lpl"}}});
  printf("node:%s\n", node());
  // create an array to json
  vector<double> arrFlo = {1.2, 3.4, 4.5};
  const char *oldStr[] = {"asdf", "nkjn"};
  int oldInt[] = {1, 2, 3, 4};
  auto arrOld = json.CreateArray(oldStr, 2);
  auto intOld = json.CreateArray(oldInt, 4);
  vector<Json::Node> arrNode{
      {{"status", "ok"}, {"name", "chenxuan"}, {"arrFlo", arrFlo}},
      {{"status", "happy"},
       {"name", "xiaozhu"},
       {"arrFlo", vector<double>() = {1.2, 3.4, 6.7}}}};
  node["arrarr"] = vector<vector<int>>() = {{1, 2, 3}, {4, 5, 6}};
  node["stdStr"] = string("koko");
  node["strOld"] = "ok";
  node["null"] = nullptr;
  node["bool"] = true;
  node["arrNode"] = arrNode;
  node["Int"] = 1000;
  node["double"] = 1.43;
  node["boolArr"] = vector<bool>() = {true, false};
  node["nodeself"] = node;
  node["obj"] = {{"status", "ok"},
                 {"vector", vector<string>() = {"chenxuan", "is", "winner"}}};
  printf("node:\n%s\n", node());
  /* //add node to json */
  json["node"] = node;
  json["numInt"] = 1000;
  json["arrold"] = arrOld;
  json["int old"] = intOld;
  json["arrint"] = vector<int>() = {1, 2, 3};
  json["arrFlo"] = arrFlo;
  json["emp arr"] = vector<int>();
  printf("result:\n%s\n", json());
  json.Parse(json());
  root = json.GetRootObj();
  printf("\n\n%s\n", json.FormatPrint(json.GetRootObj()));
  if (root["node"]["arrNode"] != Json::npos()) {
    auto temp = root["node"]["arrNode"];
    for (unsigned i = 0; i < temp.arr.size(); i++) {
      cout << "name:" << temp[i]["name"].str_val
           << " status:" << temp[i]["status"].str_val << endl;
      for (auto now : temp[i]["arrFlo"].arr)
        cout << now->flo_val << endl;
    }
  }
  Json json2 = {{"float", 12.3},
                {"int", 23},
                {"bool", true},
                {"str", "string"},
                {"null", nullptr},
                {"stdstr", string("chenxuan")},
                {"arrFloat", {2.3, 8.9, 2.3}},
                {"arrBool", {true, false}},
                {"arrObj", {{"chenxuan", "create"}, {"lover", "xiaozhu"}}},
                {"node", node},
                {"arrVec", arrFlo},
                {"empty arr", vector<int>()},
                {"empty obj", {}}};
  printf("json2:\n%s \n", json2());
}
int main() {
  funtion();
  return 0;
}
