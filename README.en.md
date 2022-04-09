## cppJson

A lightweight library for parsing, generating, and formatting json written in C++

## author

**chenxuan**

![](https://i.loli.net/2021/10/25/7pQUDsB12GE4tgx.png)

## Instructions

All code is included in the json.h header file, and all functions can be used by including this header file

## Production Instructions

1. Except for a small part using STL, most of them are made in C

2. The source code is about 1000 lines, including parsing, generating and formatting json data

3. Support the use of // and /**/ as comments

4. Very easy to use. Short hands-on time

5. You can generate json directly like go through {}, which is very convenient

## Instructions

#### Generate json parsing object

```cpp
    //create tree for json text
    Json json(file.getFileBuff(temp));//file.getFileBuff(temp) is const char*
    if(json.lastError()!=NULL)
    {
        printf("%s\n",json.lastError());
        return ;
    }
```

#### Get json data

```cpp
    // find the key value
    if(json["empt"]!=NULL)
        for(unsigned i=0;i<json["empt"]->arr.size();i++)
            printf("%f\n",json["empt"]->arr[i]->floVal);
    if(json["first Name"]!=NULL)
        printf("%s\n",json["first Name"]->strVal.c_str());
```

#### Generate json data

##### Create json object

```cpp
     //create a new json text
     Json::Node node={{"status","ok"}};
     auto node=json.createObject({{"status","ok"},{"arr",{1,2,3,4}},{"strarr",{"we","lpl"}}} );
```

> Objects can be initialized directly through {}

##### Create json array

```cpp
    //C model
    const char* oldStr[]={"asdf","nkjn"};
    auto arrOld=json.createArray(oldStr,2);
    //C++ vector
    //add Add directly
```

C's arrays need to call createArray

> Vector of C++ can be added directly (there are examples below)

##### Initialize add

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

> The way to add key Value is ("key")=Value
> 
> The way to get the result is ()

#### format json

```cpp
    const char* result=json.formatPrint(json.getRootObj());
    printf("%s\n",result);//json.getRootObj is get the root of create
```

## Introduction video

**[blibili](https://www.bilibili.com/video/BV1Nq4y1w7zY?from=search&seid=12628536326241937240&spm_id_from=333.337.0.0)**

## Supplementary notes

- Does not support json array types such as [1, "dad", true], because the addition of such arrays is complicated and not used much, ~~ too lazy to support~~

- Because it has just been developed, 100% compatibility cannot be guaranteed after subsequent improvements (as compatible as possible)

- Limited technology will inevitably lead to bugs, you can raise issues, and must be carefully revised

## Thanks for the support

---

[Personal official website](http://chenxuanweb.top)

[Resume](http://chenxuanweb.top/resume.html)

[gitee](https://gitee.com/chenxuan520)

[github](https://github.com/chenxuan520)

---

If you like this project, you can give a ⭐
