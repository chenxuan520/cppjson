## cppJson

使用C++写的**json解析和生成,格式化**的轻量库

## 作者

**chenxuan**

![](https://i.loli.net/2021/10/25/7pQUDsB12GE4tgx.png)

## 使用方法

所有代码包含于json.h头文件,包含该头文件即可使用所有功能

## 制作说明

1. 除了少部分使用STL,大部分都是C制作

2. 源代码大约700行,包括解析,生成和格式化json数据

3. 支持使用//和/**/作为注释

## 使用方法

#### 生成json解析对象

```cpp
    char temp[4000]={0};
    FileGet::getFileMsg("./a.json",temp,2000);
    //create tree for json text
    Json json(temp);//input const char* to init
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

```cpp
    //create a new json text
    char* getStr=json.createObject(200);
    json.addKeyVal(getStr,Json::STRING,"try","exmaple");
    printf("%s\n",getStr);
```

#### 格式化json

```cpp
    const char* result=json.formatPrint(json.getRootObj(),2000);
    printf("%s\n",result);
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
