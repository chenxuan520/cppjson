#pragma once
#include <functional>
#include <initializer_list>
#include <iostream>
#include <stack>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
namespace cppjson {
class Json {
public:
  enum TypeJson { // object type
    INT = 0,
    FLOAT = 1,
    ARRAY = 2,
    OBJ = 3,
    STRING = 4,
    BOOL = 5,
    STRUCT = 6,
    EMPTY = 7
  };
  struct Object {
    friend class Json;
    TypeJson type;
    TypeJson arr_type;
    std::string key;
    std::string str_val;
    std::vector<Object *> arr;
    Object *next_obj;
    double flo_val;
    int int_val;
    bool bool_val;
    bool is_data;
    Object() {
      flo_val = 0;
      int_val = 0;
      bool_val = false;
      is_data = false;
      next_obj = NULL;
      obj_val = NULL;
    }
    bool operator==(const Object &old) { return &old == this; }
    bool operator!=(const Object &old) { return &old != this; }
    Object &operator[](const char *key) {
      Object *now = this->next_obj;
      while (now != NULL) {
        if (now->key == key) {
          if (now->type == OBJ)
            return *now->obj_val;
          else
            return *now;
        }
        now = now->next_obj;
      }
      return Json::npos();
    }
    Object &operator[](unsigned pos) {
      if (this->type != ARRAY || this->arr.size() <= pos)
        return Json::npos();
      else
        return *this->arr[pos];
    }
    Object &ExcapeChar() {
      auto temp = str_val;
      str_val.clear();
      for (unsigned i = 0; i < temp.size(); i++) {
        if (temp[i] == '\\')
          i++;
        if (i >= temp.size())
          break;
        str_val += temp[i];
      }
      return *this;
    }

  protected:
    Object *obj_val;
  };
  static Object &npos() {
    static Object npos;
    return npos;
  }
  class Node;

private:
  struct InitType { // struct for ctreate such as {{"as","ds"}} in init;
    std::string val;
    InitType(std::initializer_list<std::pair<std::string, InitType>> listInit) {
      Json::Node node(listInit);
      val = node();
    }
    template <typename T> InitType(std::initializer_list<T> listInit) {
      std::vector<T> arr(listInit.size());
      int i = 0;
      for (auto iter = listInit.begin(); iter != listInit.end(); iter++) {
        arr[i] = *iter;
        i++;
      }
      int arrlen = listInit.size();
      Json::Node node(arr, arrlen);
      val = node();
    }
    InitType(const Json::Node &val) {
      Json::Node temp = val;
      this->val = temp();
    }
    template <typename T> InitType(T val) { this->val = std::to_string(val); }
    InitType(bool val) {
      if (val)
        this->val = "true";
      else
        this->val = "false";
    }
    InitType(std::nullptr_t) { this->val = "null"; }
    template <typename T> InitType(const std::vector<T> data) {
      Json::Node node(data);
      val = node();
    }
    InitType(std::string strVal) { createChar(strVal); }
    InitType(const char *pt) { createChar(pt); }
    InitType(char *pt) { createChar(pt); }
    template <typename T> void createChar(T data) {
      val += '\"';
      val += data;
      val += '\"';
    }
  };

public:
  class Node {
  private:
    std::string result_;
    std::string key_;
    const char *error_;
    unsigned flo_num_;
    bool is_array_;

  public:
    explicit Node(bool isArr = false) {
      if (!isArr)
        result_ = "{}";
      else
        result_ = "[]";
      is_array_ = isArr;
      flo_num_ = 3;
      is_array_ = false;
    }
    template <typename T>
    explicit Node(const std::vector<T> &data, unsigned floNum = 3) : Node() {
      is_array_ = true;
      result_ = "[]";
      this->flo_num_ = floNum;
      AddArray(data);
    }
    template <typename T>
    explicit Node(const T *data, unsigned len, unsigned floNum = 3) : Node() {
      std::vector<T> arr(len);
      for (unsigned i = 0; i < len; i++)
        arr[i] = data[i];
      is_array_ = true;
      result_ = "[]";
      this->flo_num_ = floNum;
      AddArray((const std::vector<T> &)arr);
    }
    Node(std::initializer_list<std::pair<std::string, InitType>> initList)
        : Node() {
      InitWithList(initList);
    }
    void InitWithList(
        std::initializer_list<std::pair<std::string, InitType>> initList) {
      for (auto iter = initList.begin(); iter != initList.end(); iter++)
        AddKeyValue(ARRAY, iter->first.c_str(), iter->second.val.c_str());
    }
    inline const char *operator()() const { return result_.c_str(); }
    inline const char *getStr() const { return result_.c_str(); }
    Node &operator[](const char *key) {
      this->key_ = key;
      return *this;
    }
    template <typename T> Node &operator=(T &data) {
      AddKeyValue(key_.c_str(), data);
      return *this;
    }
    template <typename T> Node &operator=(T &&data) {
      AddKeyValue(key_.c_str(), data);
      return *this;
    }
    Node &operator=(std::string data) {
      AddKeyValue(key_.c_str(), data.c_str());
      return *this;
    }
    Node &operator=(
        std::initializer_list<std::pair<std::string, InitType>> initList) {
      Node node(initList);
      AddKeyValue(key_.c_str(), node);
      return *this;
    }
    unsigned inline GetLen() { return result_.size(); }
    bool ChangeSetting(unsigned floNum) {
      if (floNum > 255) {
        error_ = "flonum must less 256";
        return false;
      }
      this->flo_num_ = floNum;
      return true;
    }
    inline const char *LastError() { return error_; }

  private:
    bool AddArray(const std::vector<std::string> &arr) {
      return AddArray(STRUCT, &arr[0], arr.size());
    }
    bool AddArray(const std::vector<int> &arr) {
      return AddArray(INT, &arr[0], arr.size());
    }
    bool AddArray(const std::vector<double> &arr) {
      return AddArray(FLOAT, &arr[0], arr.size());
    }
    bool AddArray(const std::vector<const char *> &arr) {
      return AddArray(STRING, &arr[0], arr.size());
    }
    bool AddArray(const std::vector<Node> &arr) {
      return AddArray(OBJ, &arr[0], arr.size());
    }
    template <typename T> bool AddArray(const std::vector<T> &arr) {
      std::vector<Node> temp(arr.size());
      for (int i = 0; i < arr.size(); i++) {
        arr[i].DumpToJson(temp[i]);
      }
      return AddArray(temp);
    }
    template <typename T>
    bool AddArray(const std::vector<std::vector<T>> &arr) {
      std::vector<Node> temp;
      for (auto &now : arr)
        temp.push_back(Node(now));
      return AddArray(temp);
    }
    bool AddArray(const std::vector<bool> &arr) {
      if (arr.size() == 0)
        return true;
      bool *temp = (bool *)malloc(sizeof(bool) * arr.size());
      if (temp == NULL) {
        error_ = "malloc wrong";
        return false;
      }
      for (unsigned i = 0; i < arr.size(); i++)
        temp[i] = arr[i];
      AddArray(BOOL, temp, arr.size());
      free(temp);
      return true;
    }
    bool AddArray(TypeJson type, const void *arr, unsigned len) {
      if (arr == NULL || !is_array_ || len == 0)
        return true;
      char word[128] = {0};
      int *arrInt = (int *)arr;
      double *arrFlo = (double *)arr;
      bool *arrBool = (bool *)arr;
      Node *arrNode = (Node *)arr;
      char **arrStr = (char **)arr;
      std::string *arrStd = (std::string *)arr;
      if (*(result_.end() - 1) == ']') {
        if (result_.size() > 2)
          *(result_.end() - 1) = ',';
        else
          result_.erase(result_.end() - 1);
      }
      switch (type) {
      case INT:
        for (unsigned i = 0; i < len; i++)
          result_ += std::to_string(arrInt[i]) + ",";
        break;
      case FLOAT:
        for (unsigned i = 0; i < len; i++) {
          memset(word, 0, sizeof(char) * 128);
          snprintf(word, 128, "%.*lf,", flo_num_, arrFlo[i]);
          result_ += word;
        }
        break;
      case STRING:
        for (unsigned i = 0; i < len; i++) {
          result_ += '\"';
          result_ += arrStr[i];
          result_ += '\"';
          result_ += ',';
        }
        break;
      case STRUCT:
        for (unsigned i = 0; i < len; i++)
          result_ += '\"' + arrStd[i] + '\"' + ',';
        break;
      case BOOL:
        for (unsigned i = 0; i < len; i++) {
          if (arrBool[i])
            result_ += "true";
          else
            result_ += "false";
          result_ += ',';
        }
        break;
      case EMPTY:
        for (unsigned i = 0; i < len; i++) {
          result_ += "null";
          result_ += ',';
        }
        break;
      case ARRAY:
      case OBJ:
        for (unsigned i = 0; i < len; i++) {
          result_ += arrNode[i]();
          result_ += ',';
        }
        break;
      }
      *(result_.end() - 1) = ']';
      return true;
    }
    bool AddKeyValue(TypeJson type, const char *key, ...) {
      if (key == NULL) {
        error_ = "key null";
        return false;
      }
      va_list args;
      va_start(args, key);
      if (*(result_.end() - 1) == '}') {
        result_.erase(result_.end() - 1);
        if (result_.size() > 1)
          result_ += ',';
      }
      result_ += '\"';
      result_ += key;
      result_ += "\":";
      char word[128] = {0};
      int valInt = 0;
      char *valStr = NULL;
      double valFlo = 9;
      bool valBool = false;
      Node *valNode = NULL;
      switch (type) {
      case INT:
        valInt = va_arg(args, int);
        result_ += std::to_string(valInt);
        break;
      case FLOAT:
        valFlo = va_arg(args, double);
        memset(word, 0, sizeof(char) * 128);
        snprintf(word, 128, "%.*lf", flo_num_, valFlo);
        result_ += word;
        break;
      case STRING:
        valStr = va_arg(args, char *);
        if (valStr == NULL) {
          error_ = "null input";
          return false;
        }
        result_ += '\"';
        result_ += valStr;
        result_ += '\"';
        break;
      case EMPTY:
        result_ += "null";
        break;
      case BOOL:
        valBool = va_arg(args, int);
        if (valBool == true)
          result_ += "true";
        else
          result_ += "false";
        break;
      case ARRAY:
        valStr = va_arg(args, char *);
        if (valStr == NULL) {
          error_ = "null input";
          return false;
        }
        result_ += valStr;
        break;
      case OBJ:
        valStr = va_arg(args, char *);
        valNode = (Node *)valStr;
        if (valStr == NULL) {
          error_ = "null input";
          return false;
        }
        result_ += (*valNode)();
        break;
      default:
        error_ = "can not insert this type";
        result_ += '}';
        return false;
      }
      result_ += '}';
      return true;
    }
    inline bool AddKeyValue(const char *key, int data) {
      return AddKeyValue(INT, key, data);
    }
    inline bool AddKeyValue(const char *key, double data) {
      return AddKeyValue(FLOAT, key, data);
    }
    inline bool AddKeyValue(const char *key, bool data) {
      return AddKeyValue(BOOL, key, data);
    }
    inline bool AddKeyValue(const char *key, Node data) {
      return AddKeyValue(OBJ, key, &data);
    }
    inline bool AddKeyValue(const char *key, const char *data) {
      return AddKeyValue(STRING, key, data);
    }
    inline bool AddKeyValue(const char *key, const std::string &data) {
      return AddKeyValue(STRING, key, data.c_str());
    }
    inline bool AddKeyValue(const char *key, const std::nullptr_t &) {
      return AddKeyValue(EMPTY, key, NULL);
    }
    template <typename T> bool AddKeyValue(const char *key, const T &data) {
      Node node;
      data.DumpToJson(node);
      return AddKeyValue(key, node);
    }
    template <typename T>
    bool AddKeyValue(const char *key, const std::vector<T> &data) {
      Node node(data, flo_num_);
      return AddKeyValue(key, node);
    }
  };

private:
  char *text_;
  const char *err_msg_;
  const char *nowKey_;
  Object *obj_;
  Node node_;
  unsigned floNum_;
  std::string wordStr_;
  std::string formatStr_;
  std::unordered_map<char *, char *> bracket_;

public:
  Json() : text_(NULL), err_msg_(NULL), nowKey_(NULL), obj_(NULL), floNum_(3) {}
  Json(std::initializer_list<std::pair<std::string, InitType>> initList)
      : Json() {
    node_.InitWithList(initList);
  }
  Json(const char *jsonText) : Json() { Parse(jsonText); }
  Json(const Json &old) = delete;
  ~Json() {
    DeleteNode(obj_);
    if (text_ != NULL)
      free(text_);
  }
  bool Parse(const std::string &jsonText) { return Parse(jsonText.c_str()); }
  bool Parse(const char *jsonText) {
    if (jsonText == NULL || strlen(jsonText) == 0) {
      err_msg_ = "message error";
      return false;
    }
    if (text_ != NULL)
      free(text_);
    text_ = (char *)malloc(strlen(jsonText) + 10);
    if (text_ == NULL) {
      err_msg_ = "malloc wrong";
      return false;
    }
    memset(text_, 0, strlen(jsonText) + 10);
    strcpy(text_, jsonText);
    DeleteComment();
    DeleteSpace();
    if (false == PairBracket()) {
      err_msg_ = "pair bracket wrong";
      return false;
    }
    if (text_[0] != '{') {
      err_msg_ = "text wrong";
      return false;
    }
    if (obj_ != NULL)
      DeleteNode(obj_);
    obj_ = AnalyseObj(text_, bracket_[text_]);
    if (obj_ == NULL)
      return false;
    return true;
  }
  const char *FormatPrint(Object &example) {
    formatStr_.clear();
    if (obj_ == NULL)
      return NULL;
    PrintObj(formatStr_, &example);
    return formatStr_.c_str();
  }
  inline Node CreateObject() { return Node(); }
  inline Node CreateObject(
      std::initializer_list<std::pair<std::string, InitType>> initList) {
    return Node(initList);
  }
  template <typename T> inline Node CreateArray(T *data, unsigned len) {
    return Node(data, len);
  }
  template <typename T> Node CreateArray(std::vector<T> &data) {
    return Node(data);
  }
  inline const char *operator()() const { return node_(); }
  inline const char *Dump() const { return node_(); }
  inline const char *GetStr() const { return node_(); }
  Json &operator[](const char *key) {
    nowKey_ = key;
    return *this;
  }
  template <typename T> Json &operator=(T value) {
    node_[nowKey_] = value;
    return *this;
  }
  inline Object &GetRootObj() {
    if (obj_ != NULL)
      return *obj_;
    else
      return Json::npos();
  }
  inline const char *err_msg() { return err_msg_; }
  inline bool ChangeSetting(unsigned floNum) {
    if (floNum > 255) {
      err_msg_ = "float num max is 255";
      return false;
    }
    this->floNum_ = floNum;
    node_.ChangeSetting(floNum);
    return true;
  }

public:
  static const char *CreateJson(const Node &temp) { return temp(); }

  template <typename T>
  static int Parse(const std::string &json_text, T &result) {
    Json json;
    json.Parse(json_text);
    if (json.Parse(json_text) == false) {
      return -1;
    }
    return result.ParseFromJson(json.GetRootObj());
  }

  template <typename T>
  static int Parse(const std::string &json_text, T &result,
                   std::function<int(Object &, T &)> parse_func) {
    Json json;
    json.Parse(json_text);
    if (json.Parse(json_text) == false) {
      return -1;
    }
    return parse_func(json.GetRootObj(), result);
  }

  template <typename T> static std::string Dump(T &result) {
    Node node;
    result.DumpToJson(node);
    return Json::CreateJson(node);
  }

  template <typename T>
  static std::string Dump(const T &result,
                          std::function<int(const T &, Node &)> format_func) {
    Node node;
    format_func(result, node);
    return Json::CreateJson(node);
  }

private:
  Object *AnalyseObj(char *begin, char *end) {
    Object *root = new Object, *last = root;
    root->type = STRUCT;
    char *now = begin + 1, *next = now;
    char temp = *end;
    *end = 0;
    while (now < end) {
      Object *nextObj = new Object;
      now = FindString(now, wordStr_);
      if (now == NULL) {
        err_msg_ = "find key wrong";
        return NULL;
      }
      nextObj->key = wordStr_;
      now += 2;
      if (*now == '\"') {
        nextObj->type = STRING;
        now = FindString(now, wordStr_);
        if (now == NULL) {
          err_msg_ = "string judge wrong";
          return NULL;
        }
        nextObj->str_val = wordStr_;
        now++;
        if (*now == ',')
          now++;
      } else if (('0' <= *now && '9' >= *now) || *now == '-') {
        next = now;
        nextObj->type = INT;
        while (*next != ',' && *next != 0) {
          next++;
          if (*next == '.')
            nextObj->type = FLOAT;
        }
        if (nextObj->type == INT)
          sscanf(now, "%d", &nextObj->int_val);
        else
          sscanf(now, "%lf", &nextObj->flo_val);
        now = next + 1;
      } else if (*now == '[') {
        next = bracket_[now];
        if (next == NULL) {
          err_msg_ = "format wrong";
          return root;
        }
        nextObj->type = ARRAY;
        nextObj->arr_type = AnalyseArray(now, next, nextObj->arr);
        now = next + 1;
        if (*now == ',')
          now++;
      } else if (*now == '{') {
        next = bracket_[now];
        if (next == NULL) {
          err_msg_ = "format wrong";
          return root;
        }
        nextObj->type = OBJ;
        nextObj->obj_val = AnalyseObj(now, next);
        now = next + 1;
        if (*now == ',')
          now++;
      } else if (strncmp(now, "true", 4) == 0) {
        nextObj->type = BOOL;
        now += 4;
        if (*now == ',')
          now++;
        nextObj->bool_val = true;
      } else if (strncmp(now, "false", 5) == 0) {
        nextObj->type = BOOL;
        now += 5;
        if (*now == ',')
          now++;
        nextObj->bool_val = false;
      } else if (strncmp(now, "null", 4) == 0) {
        nextObj->type = EMPTY;
        now += 4;
        if (*now == ',')
          now++;
      } else {
        err_msg_ = "text wrong";
        return root;
      }
      last->next_obj = nextObj;
      last = nextObj;
    }
    *end = temp;
    return root;
  }
  TypeJson AnalyseArray(char *begin, char *end, std::vector<Object *> &arr) {
    char *now = begin + 1, *next = end;
    Object *nextObj = NULL;
    if ((*now >= '0' && *now <= '9') || *now == '-') {
      next = now;
      while (next < end && *next != ',')
        next++;
      TypeJson type = JudgeNum(now, next);
      while (now < end && now != NULL) {
        nextObj = new Object;
        nextObj->is_data = true;
        nextObj->type = type;
        if (nextObj->type == INT) {
          FindNum(now, type, &nextObj->int_val);
          arr.push_back(nextObj);
        } else {
          FindNum(now, type, &nextObj->flo_val);
          arr.push_back(nextObj);
        }
        now = strchr(now + 1, ',');
        if (now != NULL)
          now++;
      }
      nextObj->arr_type = type;
    } else if (*now == '\"') {
      while (now < end && now != NULL) {
        now = FindString(now, wordStr_);
        if (now == NULL) {
          err_msg_ = "fing string wrong";
          return STRING;
        }
        nextObj = new Object;
        nextObj->type = STRING;
        nextObj->is_data = true;
        nextObj->str_val = wordStr_;
        arr.push_back(nextObj);
        now = strchr(now + 1, ',');
        if (now == NULL)
          break;
        now += 1;
      }
    } else if (strncmp(now, "true", 4) == 0 || strncmp(now, "false", 5) == 0) {
      while (now < end && now != NULL) {
        nextObj = new Object;
        nextObj->type = BOOL;
        nextObj->is_data = true;
        nextObj->bool_val = strncmp(now, "true", 4) == 0;
        arr.push_back(nextObj);
        now = strchr(now + 1, ',');
        if (now == NULL)
          break;
        now += 1;
      }
    } else if (*now == '{') {
      while (now < end && now != NULL) {
        next = bracket_[now];
        nextObj = AnalyseObj(now, next);
        nextObj->type = OBJ;
        nextObj->is_data = true;
        arr.push_back(nextObj);
        now = next;
        now = strchr(now + 1, ',');
        if (now == NULL)
          break;
        now += 1;
      }
    } else if (*now == '[') {
      while (now < end && now != NULL) {
        next = bracket_[now];
        nextObj = new Object;
        TypeJson type = AnalyseArray(now, next, nextObj->arr);
        nextObj->type = ARRAY;
        nextObj->arr_type = type;
        nextObj->is_data = true;
        arr.push_back(nextObj);
        now = next;
        now = strchr(now + 1, ',');
        if (now == NULL)
          break;
        now += 1;
      }
    } else if (*now == ']')
      return INT;
    else {
      err_msg_ = "array find wrong";
      return INT;
    }
    return nextObj->type;
  }
  char *FindString(char *begin, std::string &buffer) {
    char *now = begin + 1, *nextOne = now;
    buffer.clear();
    nextOne = strchr(now, '\"');
    while (nextOne != NULL && *(nextOne - 1) == '\\')
      nextOne = strchr(nextOne + 1, '\"');
    if (nextOne == NULL) {
      printf("wrong:\n\%s\n\n", begin);
      err_msg_ = "text wrong";
      return NULL;
    }
    for (unsigned i = 0; now + i < nextOne; i++) {
      if (*(now + i) == '\\') {
        i += 1;
        if (now + i >= nextOne) {
          break;
        }
      }
      buffer += *(now + i);
    }
    return nextOne;
  }
  void FindNum(const char *begin, TypeJson type, void *pnum) {
    if (type == INT) {
      if (sscanf(begin, "%d", (int *)pnum) < 1)
        err_msg_ = "num wrong";
    } else {
      if (sscanf(begin, "%lf", (double *)pnum) < 1)
        err_msg_ = "num wrong";
    }
  }
  inline TypeJson JudgeNum(const char *begin, const char *end) {
    for (unsigned i = 0; i + begin < end; i++)
      if (begin[i] == '.')
        return FLOAT;
    return INT;
  }
  void DeleteComment() {
    unsigned flag = 0;
    for (unsigned i = 0; i < strlen(text_); i++) {
      if (text_[i] == '\"' && text_[i - 1] != '\\')
        flag++;
      else if (flag % 2 == 0 && text_[i] == '/' && i + 1 < strlen(text_) &&
               text_[i + 1] == '/')
        while (text_[i] != '\n' && i < strlen(text_)) {
          text_[i] = ' ';
          i++;
        }
      else if (flag % 2 == 0 && text_[i] == '/' && i + 1 < strlen(text_) &&
               text_[i + 1] == '*')
        while (i + 1 < strlen(text_)) {
          if (text_[i + 1] == '/' && text_[i] == '*') {
            text_[i] = ' ';
            text_[i + 1] = ' ';
            break;
          }
          text_[i] = ' ';
          i++;
        }
      else
        continue;
    }
  }
  void DeleteSpace() {
    unsigned j = 0, k = 0;
    unsigned flag = 0;
    for (j = 0, k = 0; text_[j] != '\0'; j++) {
      if (text_[j] != '\r' && text_[j] != '\n' && text_[j] != '\t' &&
          (text_[j] != ' ' || flag % 2 != 0))
        text_[k++] = text_[j];
      if (text_[j] == '\"' && j > 0 && text_[j - 1] != '\\')
        flag++;
    }
    text_[k] = 0;
  }
  void DeleteNode(Object *root) {
    if (root == NULL)
      return;
    if (root->next_obj != NULL)
      DeleteNode(root->next_obj);
    if (root->arr.size() > 0)
      for (unsigned i = 0; i < root->arr.size(); i++)
        DeleteNode(root->arr[i]);
    if (root->obj_val != NULL)
      DeleteNode(root->obj_val);
    delete root;
    root = NULL;
  }
  bool PairBracket() {
    unsigned flag = 0;
    std::stack<char *> sta;
    bracket_.clear();
    for (unsigned i = 0; i < strlen(text_); i++) {
      if ((text_[i] == '[' || text_[i] == '{') && flag % 2 == 0)
        sta.push(text_ + i);
      else if ((text_[i] == ']' || text_[i] == '}') && flag % 2 == 0) {
        if (sta.empty())
          return false;
        if (text_[i] == ']' && *sta.top() != '[')
          return false;
        if (text_[i] == '}' && *sta.top() != '{')
          return false;
        bracket_.insert(std::pair<char *, char *>{sta.top(), &text_[i]});
        sta.pop();
      } else if (text_[i] == '\"' && i > 0 && text_[i - 1] != '\\')
        flag++;
      else
        continue;
    }
    if (!sta.empty())
      return false;
    return true;
  }
  void PrintObj(std::string &buffer, const Object *obj) {
    if (obj == NULL) {
      buffer += "{}";
      err_msg_ = "message wrong";
      return;
    }
    unsigned deep = 0;
    char word[256] = {0};
    auto line = buffer.find_last_of('\n');
    if (line == buffer.npos)
      deep = 1;
    else
      deep = buffer.c_str() + buffer.size() - &buffer[line];
    buffer += "{\n";
    Object *now = obj->next_obj;
    while (now != NULL) {
      for (unsigned i = 0; i < deep + 4; i++)
        buffer += ' ';
      buffer += '\"' + now->key + "\":";
      switch (now->type) {
      case INT:
        buffer += std::to_string(now->int_val) + ',';
        break;
      case FLOAT:
        snprintf(word, 128, "%.*lf,", floNum_, now->flo_val);
        buffer += word;
        break;
      case STRING:
        buffer += '\"' + now->str_val + "\",";
        break;
      case BOOL:
        if (now->bool_val)
          buffer += "true,";
        else
          buffer += "false,";
        break;
      case OBJ:
        PrintObj(buffer, now->obj_val);
        buffer += ',';
        break;
      case ARRAY:
        PrintArr(buffer, now->arr_type, now->arr);
        buffer += ',';
        break;
      case EMPTY:
        buffer += "null,";
        break;
      default:
        err_msg_ = "struct cannot print";
      }
      buffer += '\n';
      now = now->next_obj;
      if (now == NULL) {
        auto pos = buffer.find_last_of(',');
        if (pos != buffer.npos)
          buffer[pos] = ' ';
      }
    }
    for (unsigned i = 0; i < deep - 1; i++)
      buffer += " ";
    buffer += "}";
    return;
  }
  void PrintArr(std::string &buffer, TypeJson type,
                const std::vector<Object *> &arr) {
    unsigned deep = 0;
    char word[256] = {0};
    auto line = buffer.find_last_of('\n');
    if (line == buffer.npos)
      deep = 1;
    else
      deep = buffer.c_str() + buffer.size() - &buffer[line];
    buffer += "[\n";
    for (unsigned i = 0; i < arr.size(); i++) {
      for (unsigned i = 0; i < deep + 4; i++)
        buffer += " ";
      switch (type) {
      case INT:
        buffer += std::to_string(arr[i]->int_val) + ',';
        break;
      case FLOAT:
        snprintf(word, 128, "%.*lf,", floNum_, arr[i]->flo_val);
        buffer += word;
        break;
      case STRING:
        buffer += '\"' + arr[i]->str_val + "\",";
        break;
      case BOOL:
        if (arr[i]->bool_val)
          buffer += "true,";
        else
          buffer += "false,";
        break;
      case OBJ:
        PrintObj(buffer, arr[i]);
        buffer += ',';
        break;
      case ARRAY:
        PrintArr(buffer, arr[i]->arr_type, arr[i]->arr);
        buffer += ',';
        break;
      case EMPTY:
        buffer += "null,";
        break;
      default:
        err_msg_ = "struct cannot print";
      }
      buffer += '\n';
      if (i == arr.size() - 1) {
        auto pos = buffer.find_last_of(',');
        if (pos != buffer.npos)
          buffer[pos] = ' ';
      }
    }
    for (unsigned i = 0; i < deep - 1; i++)
      buffer += " ";
    buffer += "]";
  }
};
} // namespace cppjson
