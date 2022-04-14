## cppJson

使用C++写的**json解析和生成,格式化**的轻量库

#### English [introduction](./README.en.md)

## 作者

**chenxuan**

![](https://i.loli.net/2021/10/25/7pQUDsB12GE4tgx.png)

## 使用方法

所有代码包含于json.h头文件,包含该头文件即可使用所有功能

## 制作说明

1. 源代码大约1000行,包括解析,生成和格式化json数据

2. 支持使用//和/**/作为注释

3. 使用非常简便.上手时间短

4. 可以通过{}直接像go一样生成json,非常方便

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
    auto root=json.getRootObj();//get the result
    if(root["empt"]!=Json::npos)
        for(unsigned i=0;i<root["empt"].arr.size();i++)
            printf("%f\n",root["empt"][i].floVal);
    if(root["first Name"]!=Json::npos)
        printf("%s\n",root["first Name"].strVal.c_str());
    if(root["ept"]["ko"]!=Json::npos)
        printf("ept:ko:%lf\n",root["ept"]["ko"].floVal);
```

> 先获取解析后的对象,再用[]获取结果
> 
> 数组类型全都是object类型,可以通过.获取内部
> 
> 如果没有结果返回值是Json::npos

#### 生成json数据

##### 创造json对象

```cpp
     //create a new json text
     Json::Node node={{"status","ok"}};
     auto node=json.createObject({{"status","ok"},{"arr",{1,2,3,4}},{"strarr",{"we","lpl"}}});
```

> 可以直接通过{}来初始化对象
> 
> json本身就是就是node对象,可以使用node的方法添加键值对

##### 添加键值对

```cpp
    node["stdStr"]=string("koko");//添加字符串
    node["strOld"]="ok";
    node["null"]=nullptr;//添加null类型
    node["bool"]=true;//添加bool类型
    node["Int"]=1000;//添加int
    node["double"]=1.43;//添加double
    printf("node:\n%s\n",node());
```

> C类型和C++的string类型都可以直接添加
> 
> 没有用[]原因是分析json数据用了[]

##### 添加数组和对象

```cpp
    //添加对象,可以再创建一个node添加或者直接{}
    node["nodeself"]=node;
    node["obj"]={
        {"status","ok"},
        {"vector",vector<string>()={"chenxuan","is","winner"}}
    };
    //C 类型数组
    const char* oldStr[]={"asdf","nkjn"};
    auto arrOld=json.createArray(oldStr,2);
    //或者 Json::Node temp(oldStr,2);
    json["arrold"]=arrOld;
    //C++类型
    node["arrarr"]=vector<vector<int>>()={{1,2,3},{4,5,6}};
    node["boolArr"]=vector<bool>()={true,false};
```

> C的数组需要调用createArray,或者再生成一个node
> 
> vector类型可以直接添加,Vector最多支持二维,超过二维需要先创建node数组再加入

##### 初始化添加

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

> 获取结果的方式均为()

#### 格式化json

```cpp
    const char* result=json.formatPrint(json.getRootObj());
    printf("%s\n",result);//json.getRootObj is get the root of create
```

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
