## cppJson

A lightweight library for parsing, generating, and formatting json written in C++

#### English [introduction](./README.en.md)

## author

**chenxuan**

![](https://i.loli.net/2021/10/25/7pQUDsB12GE4tgx.png)

## Instructions

All code is included in the json.h header file, and all functions can be used by including this header file

## Production Instructions

1. The source code is about 1000 lines, including parsing, generating and formatting json data

2. Support the use of // and /**/ as comments

3. Very easy to use. Short hands-on time

4. You can generate json directly like go through {}, which is very convenient

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
auto root=json.getRootObj();//get the result
if(root["empt"]!=Json::npos)
for(unsigned i=0;i<root["empt"].arr.size();i++)
printf("%f\n",root["empt"][i].floVal);
if(root["first Name"]!=Json::npos)
printf("%s\n",root["first Name"].strVal.c_str());
if(root["ept"]["ko"]!=Json::npos)
printf("ept:ko:%lf\n",root["ept"]["ko"].floVal);
```

> Get the parsed object first, then use [] to get the result
> 
> The array types are all object types, which can be obtained through .
> 
> if there is no result the return value is Json::npos

#### Generate json data

##### Create json object

```cpp
     //create a new json text
     Json::Node node={{"status","ok"}};
     auto node=json.createObject({{"status","ok"},{"arr",{1,2,3,4}},{"strarr",{"we","lpl"}}} );
```

> Objects can be initialized directly through {}
> 
> json itself is a node object, you can use the node method to add key-value pairs

##### Add key-value pair

```cpp
node["stdStr"]=string("koko");//Add string
node["strOld"]="ok";
node["null"]=nullptr;//Add null type
node["bool"]=true;//Add bool type
node["Int"]=1000;//Add int
node["double"]=1.43;//Add double
printf("node:\n%s\n",node());
```

> Both C type and C++ string type can be added directly
> 
> Useless [] because the analysis of json data uses []

##### Adding arrays and objects

```cpp
    //Add an object, you can create another node to add or directly {}
node["nodeself"]=node;
node["obj"]={
{"status","ok"},
{"vector",vector<string>()={"chenxuan","is","winner"}}
};
    //C type array
    const char* oldStr[]={"asdf","nkjn"};
    auto arrOld=json.createArray(oldStr,2);
    //or Json::Node temp(oldStr,2);
    json["arrold"]=arrOld;
    //C++ type
    node["arrarr"]=vector<vector<int>>()={{1,2,3},{4,5,6}};
    node["boolArr"]=vector<bool>()={true,false};
```

> The array of C needs to call createArray, or generate a node again
> 
> The vector type can be added directly. Vector supports up to two dimensions. If it exceeds two dimensions, you need to create a node array before adding it.

##### Initialize add

```cpp
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
    {"arrVec",arrFlo}
};
printf("json2:\n%s \n",json2());
```

> The way to get the result is ()

#### format json

```cpp
    const char* result=json.formatPrint(json.getRootObj());
    printf("%s\n",result);//json.getRootObj is get the root of create
```

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
