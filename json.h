#ifndef _JSON_H_
#define _JSON_H_
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
namespace cppweb {
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
    TypeJson type;
    TypeJson arr_type;
    std::string key;
    std::string str_val;
    std::vector<Object *> arr;
    Object *obj_val;
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
      return Json::npos;
    }
    Object &operator[](unsigned pos) {
      if (this->type != ARRAY || this->arr.size() <= pos)
        return Json::npos;
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
  };
  static Object npos;
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
    std::string result;
    std::string key;
    const char *error;
    unsigned flo_num;
    bool is_array;

  public:
    explicit Node(bool isArr = false) {
      if (!isArr)
        result = "{}";
      else
        result = "[]";
      is_array = isArr;
      flo_num = 3;
      is_array = false;
    }
    template <typename T>
    explicit Node(const std::vector<T> &data, unsigned floNum = 3) : Node() {
      is_array = true;
      result = "[]";
      this->flo_num = floNum;
      addArray(data);
    }
    template <typename T>
    explicit Node(const T *data, unsigned len, unsigned floNum = 3) : Node() {
      std::vector<T> arr(len);
      for (unsigned i = 0; i < len; i++)
        arr[i] = data[i];
      is_array = true;
      result = "[]";
      this->flo_num = floNum;
      addArray((const std::vector<T> &)arr);
    }
    Node(std::initializer_list<std::pair<std::string, InitType>> initList)
        : Node() {
      InitWithList(initList);
    }
    void InitWithList(
        std::initializer_list<std::pair<std::string, InitType>> initList) {
      for (auto iter = initList.begin(); iter != initList.end(); iter++)
        addKeyValue(ARRAY, iter->first.c_str(), iter->second.val.c_str());
    }
    inline const char *operator()() const { return result.c_str(); }
    inline const char *getStr() const { return result.c_str(); }
    Node &operator[](const char *key) {
      this->key = key;
      return *this;
    }
    template <typename T> Node &operator=(T &data) {
      addKeyValue(key.c_str(), data);
      return *this;
    }
    template <typename T> Node &operator=(T &&data) {
      addKeyValue(key.c_str(), data);
      return *this;
    }
    Node &operator=(std::string data) {
      addKeyValue(key.c_str(), data.c_str());
      return *this;
    }
    Node &operator=(
        std::initializer_list<std::pair<std::string, InitType>> initList) {
      Node node(initList);
      addKeyValue(key.c_str(), node);
      return *this;
    }
    unsigned inline GetLen() { return result.size(); }
    bool ChangeSetting(unsigned floNum) {
      if (floNum > 255) {
        error = "flonum must less 256";
        return false;
      }
      this->flo_num = floNum;
      return true;
    }
    inline const char *lastError() { return error; }

  private:
    template <typename T> bool addArray(const std::vector<T> &arr) {
      if (arr.size() == 0)
        return false;
      if (std::is_same<T, int>::value)
        return addArray(INT, &arr[0], arr.size());
      else if (std::is_same<T, double>::value)
        return addArray(FLOAT, &arr[0], arr.size());
      else if (std::is_same<T, char *>::value ||
               std::is_same<T, const char *>::value)
        return addArray(STRING, &arr[0], arr.size());
      else if (std::is_same<T, std::string>::value)
        return addArray(STRUCT, &arr[0], arr.size());
      else if (std::is_same<T, Node>::value)
        return addArray(OBJ, &arr[0], arr.size());
      else
        return addArray(EMPTY, NULL, arr.size());
      return true;
    }
    template <typename T>
    bool addArray(const std::vector<std::vector<T>> &arr) {
      std::vector<Node> temp;
      for (auto &now : arr)
        temp.push_back(Node(now));
      return addArray(temp);
    }
    bool addArray(const std::vector<bool> &arr) {
      if (arr.size() == 0)
        return true;
      bool *temp = (bool *)malloc(sizeof(bool) * arr.size());
      if (temp == NULL) {
        error = "malloc wrong";
        return false;
      }
      for (unsigned i = 0; i < arr.size(); i++)
        temp[i] = arr[i];
      addArray(BOOL, temp, arr.size());
      free(temp);
      return true;
    }
    bool addArray(TypeJson type, const void *arr, unsigned len) {
      if (arr == NULL || !is_array || len == 0)
        return true;
      char word[128] = {0};
      int *arrInt = (int *)arr;
      double *arrFlo = (double *)arr;
      bool *arrBool = (bool *)arr;
      Node *arrNode = (Node *)arr;
      char **arrStr = (char **)arr;
      std::string *arrStd = (std::string *)arr;
      if (*(result.end() - 1) == ']') {
        if (result.size() > 2)
          *(result.end() - 1) = ',';
        else
          result.erase(result.end() - 1);
      }
      switch (type) {
      case INT:
        for (unsigned i = 0; i < len; i++)
          result += std::to_string(arrInt[i]) + ",";
        break;
      case FLOAT:
        for (unsigned i = 0; i < len; i++) {
          memset(word, 0, sizeof(char) * 128);
          sprintf(word, "%.*lf,", flo_num, arrFlo[i]);
          result += word;
        }
        break;
      case STRING:
        for (unsigned i = 0; i < len; i++) {
          result += '\"';
          result += arrStr[i];
          result += '\"';
          result += ',';
        }
        break;
      case STRUCT:
        for (unsigned i = 0; i < len; i++)
          result += '\"' + arrStd[i] + '\"' + ',';
        break;
      case BOOL:
        for (unsigned i = 0; i < len; i++) {
          if (arrBool[i])
            result += "true";
          else
            result += "false";
          result += ',';
        }
        break;
      case EMPTY:
        for (unsigned i = 0; i < len; i++) {
          result += "null";
          result += ',';
        }
        break;
      case ARRAY:
      case OBJ:
        for (unsigned i = 0; i < len; i++) {
          result += arrNode[i]();
          result += ',';
        }
        break;
      }
      *(result.end() - 1) = ']';
      return true;
    }
    bool addKeyValue(TypeJson type, const char *key, ...) {
      if (key == NULL) {
        error = "key null";
        return false;
      }
      va_list args;
      va_start(args, key);
      if (*(result.end() - 1) == '}') {
        result.erase(result.end() - 1);
        if (result.size() > 1)
          result += ',';
      }
      result += '\"';
      result += key;
      result += "\":";
      char word[128] = {0};
      int valInt = 0;
      char *valStr = NULL;
      double valFlo = 9;
      bool valBool = false;
      Node *valNode = NULL;
      switch (type) {
      case INT:
        valInt = va_arg(args, int);
        result += std::to_string(valInt);
        break;
      case FLOAT:
        valFlo = va_arg(args, double);
        memset(word, 0, sizeof(char) * 128);
        sprintf(word, "%.*lf", flo_num, valFlo);
        result += word;
        break;
      case STRING:
        valStr = va_arg(args, char *);
        if (valStr == NULL) {
          error = "null input";
          return false;
        }
        result += '\"';
        result += valStr;
        result += '\"';
        break;
      case EMPTY:
        result += "null";
        break;
      case BOOL:
        valBool = va_arg(args, int);
        if (valBool == true)
          result += "true";
        else
          result += "false";
        break;
      case ARRAY:
        valStr = va_arg(args, char *);
        if (valStr == NULL) {
          error = "null input";
          return false;
        }
        result += valStr;
        break;
      case OBJ:
        valStr = va_arg(args, char *);
        valNode = (Node *)valStr;
        if (valStr == NULL) {
          error = "null input";
          return false;
        }
        result += (*valNode)();
        break;
      default:
        error = "can not insert this type";
        result += '}';
        return false;
      }
      result += '}';
      return true;
    }
    inline bool addKeyValue(const char *key, int data) {
      return addKeyValue(INT, key, data);
    }
    inline bool addKeyValue(const char *key, double data) {
      return addKeyValue(FLOAT, key, data);
    }
    inline bool addKeyValue(const char *key, bool data) {
      return addKeyValue(BOOL, key, data);
    }
    inline bool addKeyValue(const char *key, Node data) {
      return addKeyValue(OBJ, key, &data);
    }
    inline bool addKeyValue(const char *key, char *data) {
      return addKeyValue(STRING, key, data);
    }
    inline bool addKeyValue(const char *key, const char *data) {
      return addKeyValue(STRING, key, data);
    }
    template <typename T> bool addKeyValue(const char *key, T) {
      return addKeyValue(EMPTY, key, NULL);
    }
    template <typename T>
    bool addKeyValue(const char *key, const std::vector<T> &data) {
      Node node(data, flo_num);
      return addKeyValue(key, node);
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
  Json(const char *jsonText) : Json() {
    if (jsonText == NULL || strlen(jsonText) == 0) {
      err_msg_ = "message error";
      return;
    }
    text_ = (char *)malloc(strlen(jsonText) + 10);
    if (text_ == NULL) {
      err_msg_ = "malloc wrong";
      return;
    }
    memset(text_, 0, strlen(jsonText) + 10);
    strcpy(text_, jsonText);
    deleteComment();
    deleteSpace();
    if (false == pairBracket()) {
      err_msg_ = "pair bracket wrong";
      return;
    }
    if (text_[0] != '{') {
      err_msg_ = "text wrong";
      return;
    }
    obj_ = analyseObj(text_, bracket_[text_]);
    if (obj_ == NULL)
      return;
  }
  Json(const Json &old) = delete;
  ~Json() {
    deleteNode(obj_);
    if (text_ != NULL)
      free(text_);
  }
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
    deleteComment();
    deleteSpace();
    if (false == pairBracket()) {
      err_msg_ = "pair bracket wrong";
      return false;
    }
    if (text_[0] != '{') {
      err_msg_ = "text wrong";
      return false;
    }
    if (obj_ != NULL)
      deleteNode(obj_);
    obj_ = analyseObj(text_, bracket_[text_]);
    if (obj_ == NULL)
      return false;
    return true;
  }
  const char *FormatPrint(Object &example) {
    formatStr_.clear();
    if (obj_ == NULL)
      return NULL;
    printObj(formatStr_, &example);
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
      return Json::npos;
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
  static const char *CreateJson(const Node &temp) { return temp(); }

private:
  Object *analyseObj(char *begin, char *end) {
    Object *root = new Object, *last = root;
    root->type = STRUCT;
    char *now = begin + 1, *next = now;
    char temp = *end;
    *end = 0;
    while (now < end) {
      Object *nextObj = new Object;
      now = findString(now, wordStr_);
      if (now == NULL) {
        err_msg_ = "find key wrong";
        return NULL;
      }
      nextObj->key = wordStr_;
      now += 2;
      if (*now == '\"') {
        nextObj->type = STRING;
        now = findString(now, wordStr_);
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
        nextObj->arr_type = analyseArray(now, next, nextObj->arr);
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
        nextObj->obj_val = analyseObj(now, next);
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
  TypeJson analyseArray(char *begin, char *end, std::vector<Object *> &arr) {
    char *now = begin + 1, *next = end;
    Object *nextObj = NULL;
    if ((*now >= '0' && *now <= '9') || *now == '-') {
      next = now;
      while (next < end && *next != ',')
        next++;
      TypeJson type = judgeNum(now, next);
      while (now < end && now != NULL) {
        nextObj = new Object;
        nextObj->is_data = true;
        nextObj->type = type;
        if (nextObj->type == INT) {
          findNum(now, type, &nextObj->int_val);
          arr.push_back(nextObj);
        } else {
          findNum(now, type, &nextObj->flo_val);
          arr.push_back(nextObj);
        }
        now = strchr(now + 1, ',');
        if (now != NULL)
          now++;
      }
      nextObj->arr_type = type;
    } else if (*now == '\"') {
      while (now < end && now != NULL) {
        now = findString(now, wordStr_);
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
        nextObj = analyseObj(now, next);
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
        TypeJson type = analyseArray(now, next, nextObj->arr);
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
  char *findString(char *begin, std::string &buffer) {
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
  void findNum(const char *begin, TypeJson type, void *pnum) {
    if (type == INT) {
      if (sscanf(begin, "%d", (int *)pnum) < 1)
        err_msg_ = "num wrong";
    } else {
      if (sscanf(begin, "%lf", (double *)pnum) < 1)
        err_msg_ = "num wrong";
    }
  }
  inline TypeJson judgeNum(const char *begin, const char *end) {
    for (unsigned i = 0; i + begin < end; i++)
      if (begin[i] == '.')
        return FLOAT;
    return INT;
  }
  void deleteComment() {
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
  void deleteSpace() {
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
  void deleteNode(Object *root) {
    if (root == NULL)
      return;
    if (root->next_obj != NULL)
      deleteNode(root->next_obj);
    if (root->arr.size() > 0)
      for (unsigned i = 0; i < root->arr.size(); i++)
        deleteNode(root->arr[i]);
    if (root->obj_val != NULL)
      deleteNode(root->obj_val);
    delete root;
    root = NULL;
  }
  bool pairBracket() {
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
  void printObj(std::string &buffer, const Object *obj) {
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
        sprintf(word, "%.*lf,", floNum_, now->flo_val);
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
        printObj(buffer, now->obj_val);
        buffer += ',';
        break;
      case ARRAY:
        printArr(buffer, now->arr_type, now->arr);
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
  void printArr(std::string &buffer, TypeJson type,
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
        sprintf(word, "%.*lf,", floNum_, arr[i]->flo_val);
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
        printObj(buffer, arr[i]);
        buffer += ',';
        break;
      case ARRAY:
        printArr(buffer, arr[i]->arr_type, arr[i]->arr);
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
Json::Object Json::npos;
} // namespace cppweb
#endif
