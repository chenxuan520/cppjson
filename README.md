## cppJson

使用C++写的**json解析和生成,格式化**的轻量库

#### English [introduction](./README.en.md)

## 作者

**chenxuan**

![](https://i.loli.net/2021/10/25/7pQUDsB12GE4tgx.png)

## 使用方法

所有代码包含于json.h头文件,包含该头文件即可使用所有功能

## 制作说明

1. 除了少部分使用STL,大部分都是C制作

2. 源代码大约1000行,包括解析,生成和格式化json数据

3. 支持使用//和/**/作为注释

4. 使用非常简便.上手时间短

5. 可以通过{}直接像go一样生成json,非常方便

## 使用方法

#### 生成json解析对象

```cpp
    //create tree for json text
    Json json(file.getFileBuff(temp));//file.getFileBuff(temp) is const char*
    if(json.lastError()!=NULL)
    {
        printf("%s\n",json.lastError());
        return ;
    }
```

#### 获取json数据

```cpp
    // find the key value
    if(json["empt"]!=NULL)
        for(unsigned i=0;i<json["empt"]->arr.size();i++)
            printf("%f\n",json["empt"]->arr[i]->floVal);
    if(json["first Name"]!=NULL)
        printf("%s\n",json["first Name"]->strVal.c_str());
```

#### 生成json数据

##### 创造json对象

```cpp
     //create a new json text
     Json::Node node={{"status","ok"}};
     auto node=json.createObject({{"status","ok"},{"arr",{1,2,3,4}},{"strarr",{"we","lpl"}}});
```

> 可以直接通过{}来初始化对象

##### 创建json数组

```cpp
    //C model
    const char* oldStr[]={"asdf","nkjn"};
    auto arrOld=json.createArray(oldStr,2);
    //C++ vector
    //add Add directly
```

> C的数组需要调用createArray
> 
> C++的Vector可以直接添加(下面有实例)

##### 初始化添加

```cpp
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
    json2("new arr")=arr;
    printf("new create:\n%s\n",json2());
    //node add kry value
    auto arr=json.createArray(vector<string>()={"str1","str2"});
    const char* oldStr[]={"asdf","nkjn"};
    vector<Json::Node> arrNode;
    auto arrOld=json.createArray(oldStr,2);
    json.changeSetting(2);
    node("std")=string("koko");
    node("str")="ok";
    node("null")=nullptr;
    node("bool")=true;
    for(unsigned i=0;i<5;i++)
        arrNode.push_back(node);
    node("arrnode")=arrNode;
    node("Int")=1000;
    node("double")=1.43;
    node("boolArr")=vector<bool>()={true,false};
    node("nodeself")=node;
```

>  添加key Value方式均为("key")=Value
> 
> 获取结果的方式均为()

#### 格式化json

```cpp
    const char* result=json.formatPrint(json.getRootObj());
    printf("%s\n",result);//json.getRootObj is get the root of create
```

## 介绍视频

**[blibili](https://www.bilibili.com/video/BV1Nq4y1w7zY?from=search&seid=12628536326241937240&spm_id_from=333.337.0.0)**

## 补充说明

- 不支持json数组类型不相同如[1,"dad",true],因为这种数组的添加复杂而且使用不多,~~懒得支持~~

- 因为刚刚开发不久,后面改进后不能保证100%兼容性(尽量兼容)

- 技术有限难免出现bug,可以提issues,一定认真修改

## 感谢支持

---

[个人官网](http://chenxuanweb.top) 

[个人简历](http://chenxuanweb.top/resume.html) 

[gitee](https://gitee.com/chenxuan520) 

[github](https://github.com/chenxuan520)

---

如果你喜欢这个项目,可以给一个⭐
