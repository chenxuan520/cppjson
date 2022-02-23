## cppJson

使用C++写的**json解析和生成,格式化**的轻量库

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

##### 通用添加方法

```cpp
    //create a new json text
    char* getStr=json.createObject();
    json.addKeyVal(getStr,Json::STRING,"try","exmaple");
    printf("%s\n",getStr);
    // an others way to add
	json.addKeyVal(getStr,"new","string");
	json.addKeyVal(getStr,"file name",(const char*)temp);
	json.addKeyVal(getStr,"newarr",arr);
```

> 第二种方法添加时候字符串需要转为const char*
> 
> 推荐使用第二种方法添加

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
    printf("new create:\n%s\n",json2());
    //after begin you can continue add ketValue
    json2.addKeyVal(json2(),Json::STRING,"temp","chenxuan");
    json2("new arr")=arr;
    printf("new create:\n%s\n",json2());
```

> 第二种方法添加之后通过()获取结果
> 
> 第二种方法之后依旧可以通过第一种方法继续添加
> 
> 第二种方法不支持二维数组以及对象数组,因为高维后数组和对象界限会模糊
> 
> 如果需要请用第一种添加

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
