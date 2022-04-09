#ifndef _JSON_H_
#define _JSON_H_
#include <string.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <stack>
#include <unordered_map>
#include <initializer_list>
#include <type_traits>
class Json{
public:
	enum TypeJson{//object type
		INT=0,FLOAT=1,ARRAY=2,OBJ=3,STRING=4,BOOL=5,STRUCT=6,EMPTY=7
	};
	struct Object{
		TypeJson type;
		TypeJson arrType;
		std::string key;
		std::string strVal;
		std::vector<Object*> arr;
		Object* objVal;
		Object* nextObj;
		double floVal;
		int intVal;
		bool boolVal;
		bool isData;
		Object()
		{
			floVal=0;
			intVal=0;
			boolVal=false;
			isData=false;
			nextObj=NULL;
			objVal=NULL;
		}
		Object* operator[](const char* key)
		{
			Object* now=this->nextObj;
			while(now!=NULL)
			{
				if(now->key==key)
					return now;
				now=now->nextObj;
			}
			return NULL;
		}
	};
private:
	struct InitType{//struct for ctreate such as {{"as","ds"}} in init;
		TypeJson type=STRING;
		void* pval=NULL;
		const char* error=NULL;
		unsigned len=0;
		~InitType()
		{
			if(pval)
			{
				free(pval);
				pval=NULL;
			}
		}
		InitType(const InitType& old)
		{
			this->type=old.type;
			this->error=old.error;
			if(old.pval!=NULL)
			{
				pval=malloc(sizeof(char)*old.len);
				if(pval==NULL)
				{
					error="malloc worng";
					type=EMPTY;
					return;
				}
				memcpy(pval,old.pval,old.len);
			}
		}
		InitType(std::initializer_list<std::pair<std::string,InitType>> listInit)
		{
			type=OBJ;
			std::string result="";
			Json json(listInit);
			result+=json();
			pval=malloc(sizeof(char)*(result.size()+10));
			len=sizeof(char)*(result.size()+10);
			memset(pval,0,sizeof(char)*(result.size()+10));
			if(pval==NULL)
			{
				error="malloc wrong";
				type=EMPTY;
				return;
			}
			strcpy((char*)pval,result.c_str());
		}
		template<typename T>
		InitType(std::initializer_list<T> listInit)
		{
			type=ARRAY;
			T* arr=new T[listInit.size()];
			int i=0;
			for(auto iter=listInit.begin();iter!=listInit.end();iter++)
			{
				arr[i]=*iter;
				i++;
			}
			int arrlen=listInit.size();
			Json::Node node(arr,arrlen);
			pval=malloc(strlen(node())+10);
			len=sizeof(char)*strlen(node())+10;
			strcpy((char*)pval,node());
			delete [] arr;
		}
		template<typename T>
		InitType(T val)
		{
			if(std::is_same<T,int>::value)
				type=INT;
			else if(std::is_same<T,double>::value)
				type=FLOAT;
			else if(std::is_same<T,bool>::value)
				type=BOOL;
			else
				type=EMPTY;
			pval=malloc(sizeof(T));
			if(pval==NULL)
			{
				type=EMPTY;
				error="malloc wrong";
			}
			else
			{
				len=sizeof(T);
				memset(pval,0,sizeof(T));
				*(T*)pval=val;
			}
		}
		InitType(const char* pt)
		{
			if(pt==nullptr)
				type=EMPTY;
			else
			{
				type=STRING;
				pval=malloc(sizeof(char)*strlen(pt)+10);
				len=sizeof(sizeof(char)*strlen(pt)+10);
				if(pval==NULL)
				{
					type=EMPTY;
					error="malloc worng";
				}
				else
				{
					memset(pval,0,sizeof(char)*strlen(pt)+10);
					strcpy((char*)pval,pt);
				}
			}
		}
	};
public:
	class Node{
	private:
		std::string result;
		std::string key;
		const char* error;
		unsigned floNum;
		bool isArray;
	public:
		explicit Node(bool isArr=false)
		{
			if(!isArr)
				result="{}";
			else
				result="[]";
			isArray=isArr;
			floNum=3;
			isArray=false;
		}
		template<typename T>
		explicit Node(const std::vector<T>& data,unsigned floNum=3):Node()
		{
			isArray=true;
			result="[]";
			this->floNum=floNum;
			addArray(data);
		}
		template<typename T>
		explicit Node(const T* data,unsigned len,unsigned floNum=3):Node()
		{
			std::vector<T> arr(len);
			for(unsigned i=0;i<len;i++)
				arr[i]=data[i];
			isArray=true;
			result="[]";
			this->floNum=floNum;
			addArray((const std::vector<T>&)arr);
		}
		Node(std::initializer_list<std::pair<std::string,InitType>> initList):Node()
		{
			initWithList(initList);
		}
		void initWithList(std::initializer_list<std::pair<std::string,InitType>> initList)
		{
			for(auto iter=initList.begin();iter!=initList.end();iter++)
			{
				if(iter->second.pval==NULL&&iter->second.type!=EMPTY)
				{
					error="init wrong";
					return;
				}
				if(iter->second.error!=NULL)
				{
					error="init wrong";
					return;
				}
				switch(iter->second.type)
				{
				case STRING:
					if(false==addKeyValue(STRING,iter->first.c_str(),iter->second.pval))
						return;
					break;
				case INT:
					if(false==addKeyValue(INT,iter->first.c_str(),*(int*)iter->second.pval))
						return;
					break;
				case FLOAT:
					if(false==addKeyValue(FLOAT,iter->first.c_str(),*(double*)iter->second.pval))
						return;
					break;
				case BOOL:
					if(false==addKeyValue(BOOL,iter->first.c_str(),*(bool*)iter->second.pval))
						return;
					break;
				case EMPTY:
					if(false==addKeyValue(EMPTY,iter->first.c_str(),NULL))
						return;
					break;
				case OBJ:
					if(false==addKeyValue(OBJ,iter->first.c_str(),(char*)iter->second.pval))
						return;
					break;
				case ARRAY:
					if(false==addKeyValue(ARRAY,iter->first.c_str(),(char*)iter->second.pval))
						return;
					break;
				default:
					return;
				}
			}
		}
		inline const char* operator()()
		{
			return result.c_str();
		}
		Node& operator()(const char* key)
		{
			this->key=key;
			return *this;
		}
		template<typename T>
		Node& operator=(T& data)
		{
			addKeyValue(key.c_str(),data);
			return *this;
		}
		template<typename T>
		Node& operator=(T&& data)
		{
			addKeyValue(key.c_str(),data);
			return *this;
		}
		bool changeSetting(unsigned floNum)
		{
			if(floNum>255)
			{
				error="flonum must less 256";
				return false;
			}
			this->floNum=floNum;
			return true;
		}
		inline const char* lastError()
		{
			return error;
		}
	private:
		template<typename T>
		bool addArray(const std::vector<T>& arr)
		{
			if(arr.size()==0)
				return false;
			if(std::is_same<T,int>::value)
				return addArray(INT,&arr[0],arr.size());
			else if(std::is_same<T,double>::value)
				return addArray(FLOAT,&arr[0],arr.size());
			else if(std::is_same<T,char*>::value||std::is_same<T,const char*>::value)
				return addArray(STRING,&arr[0],arr.size());
			else if(std::is_same<T,std::string>::value)
				return addArray(STRUCT,&arr[0],arr.size());
			else if(std::is_same<T,Node>::value)
				return addArray(OBJ,&arr[0],arr.size());
			else
				return addArray(EMPTY,NULL,arr.size());
			return true;
		}
		bool addArray(const std::vector<bool>& arr)
		{
			if(arr.size()==0)
				return true;
			bool* temp=(bool*)malloc(sizeof(bool)*arr.size());
			if(temp==NULL)
			{
				error="malloc wrong";
				return false;
			}
			for(unsigned i=0;i<arr.size();i++)
				temp[i]=arr[i];
			addArray(BOOL,temp,arr.size());
			free(temp);
			return true;
		}
		bool addArray(TypeJson type,const void* arr,unsigned len)
		{
			if(arr==NULL||!isArray||len==0)
				return true;
			char word[128]={0};
			int* arrInt=(int*)arr;
			double* arrFlo=(double*)arr;
			bool* arrBool=(bool*)arr;
			Node* arrNode=(Node*)arr;
			char** arrStr=(char**)arr;
			std::string* arrStd=(std::string*)arr;
			if(*(result.end()-1)==']')
			{
				if(result.size()>2)
					*(result.end()-1)=',';
				else
					result.erase(result.end()-1);
			}
			switch(type)
			{
			case INT:
				for(unsigned i=0;i<len;i++)
					result+=std::to_string(arrInt[i])+",";
				break;
			case FLOAT:
				for(unsigned i=0;i<len;i++)
				{
					memset(word,0,sizeof(char)*128);
					sprintf(word,"%.*lf,",floNum,arrFlo[i]);
					result+=word;
				}
				break;
			case STRING:
				for(unsigned i=0;i<len;i++)
				{
					result+='\"';
					result+=+arrStr[i];
					result+='\"';
					result+=',';
				}
				break;
			case STRUCT:
				for(unsigned i=0;i<len;i++)
					result+='\"'+arrStd[i]+'\"'+',';
				break;
			case BOOL:
				for(unsigned i=0;i<len;i++)
				{
					if(arrBool[i])
						result+="true";
					else
						result+="false";
					result+=',';
				}
				break;
			case EMPTY:
				for(unsigned i=0;i<len;i++)
				{
					result+="null";
					result+=',';
				}
				break;
			case ARRAY:
			case OBJ:
				for(unsigned i=0;i<len;i++)
				{
					result+=arrNode[i]();
					result+=',';
				}
				break;
			}
			*(result.end()-1)=']';
			return true;
		}
		bool addKeyValue(TypeJson type,const char* key,...)
		{
			if(key==NULL)
			{
				error="key null";
				return false;
			}
			va_list args;
			va_start(args,key);
			if(*(result.end()-1)=='}')
			{
				result.erase(result.end()-1);
				if(result.size()>1)
					result+=',';
			}
			result+='\"';
			result+=key;
			result+="\":";
			char word[128]={0};
			int valInt=0;
			char* valStr=NULL;
			double valFlo=9;
			bool valBool=false;
			Node* valNode=NULL;
			switch(type)
			{
			case INT:
				valInt=va_arg(args,int);
				result+=std::to_string(valInt);
				break;
			case FLOAT:
				valFlo=va_arg(args,double);
				memset(word,0,sizeof(char)*128);
				sprintf(word,"%.*lf",floNum,valFlo);
				result+=word;
				break;
			case STRING:
				valStr=va_arg(args,char*);
				if(valStr==NULL)
				{
					error="null input";
					return false;
				}
				result+='\"';
				result+=valStr;
				result+='\"';
				break;
			case EMPTY:
				result+="null";
				break;
			case BOOL:
				valBool=va_arg(args,int);
				if(valBool==true)
					result+="true";
				else
					result+="false";
				break;
			case OBJ:
			case ARRAY:
				valStr=va_arg(args,char*);
				valNode=(Node*)valStr;
				if(valStr==NULL)
				{
					error="null input";
					return false;
				}
				result+=(*valNode)();
				break;
			default:
				error="can not insert this type";
				result+='}';
				return false;
			}
			result+='}';
			return true;
		}
		template<typename T>
		bool addKeyValue(const char* key,T data)
		{
			if(std::is_same<T,int>::value)
				return addKeyValue(INT,key,data);
			else if(std::is_same<T,double>::value)
				return addKeyValue(FLOAT,key,data);
			else if(std::is_same<T,bool>::value)
				return addKeyValue(BOOL,key,data);
			else if(std::is_same<T,Node>::value)
				return addKeyValue(OBJ,key,&data);
			else if(std::is_same<T,const char*>::value||std::is_same<T,char*>::value)
				return addKeyValue(STRING,key,data);
			else
				return addKeyValue(EMPTY,key,data);
		}
		template<typename T>
		bool addKeyValue(const char* key,const std::vector<T>& data)
		{
			Node node(data,floNum);
			return addKeyValue(key,node);
		}
	};
private:
	char* text;
	const char* error;
	const char* nowKey;
	char* word;
	unsigned maxLen;
	unsigned floNum;
	unsigned defaultSize;
	bool isCheck;
	Object* obj;
	Node node;
	std::unordered_map<char*,int> memory;
	std::unordered_map<char*,char*> bracket;
public:
	Json()
	{
		error=NULL;
		obj=NULL;
		text=NULL;
		word=NULL;
		nowKey=NULL;
		isCheck=true;
		maxLen=256;
		floNum=3;
		defaultSize=128;
		word=(char*)malloc(sizeof(char)*maxLen);
		if(word==NULL)
		{
			error="malloc wrong";
			return;
		}
		memset(word,0,sizeof(char)*maxLen);
	}
	Json(std::initializer_list<std::pair<std::string,InitType>> initList):Json()
	{
		node.initWithList(initList);
	}
	Json(const char* jsonText):Json()
	{
		if(jsonText==NULL||strlen(jsonText)==0)
		{
			error="message error";
			return;
		}
		text=(char*)malloc(strlen(jsonText)+10);
		if(text==NULL)
		{
			error="malloc wrong";
			return;
		}
		memset(text,0,strlen(jsonText)+10);
		strcpy(text,jsonText);
		deleteComment();
		deleteSpace();
		if(false==pairBracket())
		{
			error="pair bracket wrong";
			return;
		}
		if(text[0]!='{')
		{
			error="text wrong";
			return;
		}
		obj=analyseObj(text,bracket[text]);
		if(obj==NULL)
		{
			error="malloc wrong";
			return;
		}
	}
	Json(const Json& old)=delete;
	~Json()
	{
		deleteNode(obj);
		if(word!=NULL)
		{
			free(word);
			word=NULL;
		}
		if(text!=NULL)
			free(text);
		for(auto iter=memory.begin();iter!=memory.end();iter++)
			if(iter->first!=NULL)
				free(iter->first);
	}
	bool analyseText(const char* jsonText)
	{
		if(jsonText==NULL||strlen(jsonText)==0)
		{
			error="message error";
			return false;
		}
		if(text!=NULL)
			free(text);
		text=(char*)malloc(strlen(jsonText)+10);
		if(text==NULL)
		{
			error="malloc wrong";
			return false;
		}
		memset(text,0,strlen(jsonText)+10);
		strcpy(text,jsonText);
		deleteComment();
		deleteSpace();
		if(false==pairBracket())
		{
			error="pair bracket wrong";
			return false;
		}
		if(text[0]!='{')
		{
			error="text wrong";
			return false;
		}
		if(obj!=NULL)
			deleteNode(obj);
		obj=analyseObj(text,bracket[text]);
		if(obj==NULL)
		{
			error="malloc wrong";
			return false;
		}
		return true;
	}
	const char* formatPrint(const Object* exmaple)
	{
		char* buffer=(char*)malloc(sizeof(char)*defaultSize*10);
		if(buffer==NULL)
		{
			error="malloc wrong";
			return NULL;
		}
		memset(buffer,0,sizeof(char)*defaultSize*10);
		memory.insert(std::pair<char*,int>{buffer,sizeof(char)*defaultSize*10});
		printObj(buffer,exmaple);
		return buffer;
	}
	inline Node createObject()
	{
		return Node();
	}
	inline Node createObject(std::initializer_list<std::pair<std::string,InitType>> initList)
	{
		return Node(initList);
	}
	template<typename T>
	inline Node createArray(T* data,unsigned len)
	{
		return Node(data,len);
	}
	template<typename T>
	Node createArray(std::vector<T>& data)
	{
		return Node(data);
	}
	inline Object* operator[](const char* key)
	{
		return (*obj)[key];
	}
	const char* operator()()
	{
		return node();
	}
	Json& operator()(const char* key)
	{
		nowKey=key;
		return *this;
	}
	template<typename T>
	Json& operator=(T value)
	{
		node(nowKey)=value;
		return *this;
	}
	inline Object* getRootObj()
	{
		return obj;
	}
	inline const char* lastError()
	{
		return error;
	}
	inline void changeSetting(unsigned floNum=3,unsigned defaultSize=128,unsigned keyValMaxLen=256)
	{
		this->defaultSize=defaultSize;
		this->maxLen=keyValMaxLen>maxLen?keyValMaxLen:maxLen;
		this->floNum=floNum;
		node.changeSetting(floNum);
	}
private:
	char* enlargeMemory(char* old)
	{
		if(memory.find(old)==memory.end())
			return old;
		int temp=memory[old];
		temp*=2;
		void* strTemp=realloc(old,temp);
		if(strTemp==NULL)
			return old;
		memory.erase(memory.find(old));
		memory.insert(std::pair<char*,int>{(char*)strTemp,temp});
		return (char*)strTemp;
	}
	Object* analyseObj(char* begin,char* end)
	{
		Object * root=new Object,*last=root;
		root->type=STRUCT;
		char* now=begin+1,*next=now;
		char temp=*end;
		if(word==NULL)
		{
			error="malloc wrong";
			return NULL;
		}
		memset(word,0,sizeof(char)*maxLen);
		*end=0;
		while(now<end)
		{
			Object* nextObj=new Object;
			memset(word,0,sizeof(char)*maxLen);
			findString(now,word,maxLen);
			nextObj->key=word;
			/* hashMap.insert(std::pair<std::string,Object*>{word,nextObj}); */
			now+=strlen(word)+3;
			memset(word,0,sizeof(char)*maxLen);
			if(*now=='\"')
			{
				nextObj->type=STRING;
				next=strchr(now+1,'\"');
				while(next!=NULL&&*(next-1)=='\\')
					next=strchr(next+1,'\"');
				if(next==NULL)
				{
					error="string wrong";
					return NULL;
				}
				if(next-now+3>maxLen)
				{
					char* tempStr=(char*)realloc(word,next-now+3);
					if(tempStr!=NULL)
					{
						word=tempStr;
						maxLen=next-now+3;
					}
				}
				for(unsigned i=0;now+i+1<next&&i<maxLen;i++)
					word[i]=*(now+i+1);
				word[strlen(word)]=0;
				nextObj->strVal=word;
				now=next+1;
				if(*now==',')
					now++;
			}
			else if(('0'<=*now&&'9'>=*now)||*now=='-')
			{
				next=now;
				nextObj->type=INT;
				while(*next!=','&&*next!=0)
				{
					next++;
					if(*next=='.')
						nextObj->type=FLOAT;
				}
				if(nextObj->type==INT)
					sscanf(now,"%d",&nextObj->intVal);
				else
					sscanf(now,"%lf",&nextObj->floVal);
				now=next+1;
			}
			else if(*now=='[')
			{
				next=bracket[now];
				if(next==NULL)
				{
					error="format wrong";
					return root;
				}
				nextObj->type=ARRAY;
				nextObj->arrType=analyseArray(now,next,nextObj->arr);
				now=next+1;
				if(*now==',')
					now++;
			}
			else if(*now=='{')
			{
				next=bracket[now];
				if(next==NULL)
				{
					error="format wrong";
					return root;
				}
				nextObj->type=OBJ;
				nextObj->objVal=analyseObj(now,next);
				now=next+1;
				if(*now==',')
					now++;
			}
			else if(strncmp(now,"true",4)==0)
			{
				nextObj->type=BOOL;
				now+=4;
				if(*now==',')
					now++;
				nextObj->boolVal=true;
			}
			else if(strncmp(now,"false",5)==0)
			{
				nextObj->type=BOOL;
				now+=5;
				if(*now==',')
					now++;
				nextObj->boolVal=false;
			}
			else if(strncmp(now,"null",4)==0)
			{
				nextObj->type=EMPTY;
				now+=4;
				if(*now==',')
					now++;
			}
			else
			{
				error="text wrong";
				return root;
			}
			last->nextObj=nextObj;
			last=nextObj;
		}
		*end=temp;
		return root;
	}
	TypeJson analyseArray(char* begin,char* end,std::vector<Object*>& arr)
	{
		char* now=begin+1,*next=end;
		if(word==NULL)
		{
			error="malloc wrong";
			return INT;
		}
		memset(word,0,sizeof(char)*maxLen);
		Object* nextObj=NULL;
		if((*now>='0'&&*now<='9')||*now=='-')
		{
			next=now;
			while(next<end&&*next!=',')
				next++;
			TypeJson type=judgeNum(now,next);
			while(now<end&&now!=NULL)
			{
				nextObj=new Object;
				nextObj->isData=true;
				nextObj->type=type;
				if(nextObj->type==INT)
				{
					findNum(now,type,&nextObj->intVal);
					arr.push_back(nextObj);
				}
				else
				{
					findNum(now,type,&nextObj->floVal);
					arr.push_back(nextObj);
				}
				now=strchr(now+1,',');
				if(now!=NULL)
					now++;
			}
			nextObj->arrType=type;
		}
		else if(*now=='\"')
		{
			while(now<end&&now!=NULL)
			{
				findString(now,word,maxLen);
				nextObj=new Object;
				nextObj->type=STRING;
				nextObj->isData=true;
				nextObj->strVal=word;
				arr.push_back(nextObj);
				now=strchr(now+1,',');
				if(now==NULL)
					break;
				now+=1;
			}
		}
		else if(strncmp(now,"true",4)==0||strncmp(now,"false",5)==0)
		{
			while(now<end&&now!=NULL)
			{
				nextObj=new Object;
				nextObj->type=BOOL;
				nextObj->isData=true;
				nextObj->boolVal=strncmp(now,"true",4)==0;
				arr.push_back(nextObj);
				now=strchr(now+1,',');
				if(now==NULL)
					break;
				now+=1;
			}
		}
		else if(*now=='{')
		{
			while(now<end&&now!=NULL)
			{
				next=bracket[now];
				nextObj=analyseObj(now,next);
				nextObj->type=OBJ;
				nextObj->isData=true;
				arr.push_back(nextObj);
				now=next;
				now=strchr(now+1,',');
				if(now==NULL)
					break;
				now+=1;
			}
		}
		else if(*now=='[')
		{
			while(now<end&&now!=NULL)
			{
				next=bracket[now];
				nextObj=new Object;
				TypeJson type=analyseArray(now,next,nextObj->arr);
				nextObj->type=ARRAY;
				nextObj->arrType=type;
				nextObj->isData=true;
				arr.push_back(nextObj);
				now=next;
				now=strchr(now+1,',');
				if(now==NULL)
					break;
				now+=1;
			}
		}
		else if(*now==']')
			return INT;
		else
		{
			error="array find wrong";
			return INT;
		}
		return nextObj->type;
	}
	void findString(const char* begin,char*& buffer,unsigned& buffLen)
	{
		const char* now=begin+1,*nextOne=now;
		nextOne=strchr(now+1,'\"');
		while(nextOne!=NULL&&*(nextOne-1)=='\\')
			nextOne=strchr(nextOne+1,'\"');
		if(nextOne==NULL)
		{
			error="text wrong";
			return;
		}
		if(buffLen<nextOne-now)
		{
			char* temp=(char*)realloc(buffer,sizeof(char)*(nextOne-now+10));
			if(temp!=NULL)
			{
				buffer=temp;
				buffLen=nextOne-now+10;
			}
		}
		for(unsigned i=0;now+i<nextOne&&i<buffLen-1;i++)
			buffer[i]=*(now+i);
		buffer[strlen(buffer)]=0;
	}
	void findNum(const char* begin,TypeJson type,void* pnum)
	{
		if(type==INT)
		{
			if(sscanf(begin,"%d",(int*)pnum)<1)
				error="num wrong";
		}
		else
		{
			if(sscanf(begin,"%lf",(double*)pnum)<1)
				error="num wrong";
		}
	}
	inline TypeJson judgeNum(const char* begin,const char* end)
	{
		for(unsigned i=0;i+begin<end;i++)
			if(begin[i]=='.')
				return FLOAT;
		return INT;
	}
	void deleteComment()
	{
		unsigned flag=0;
		for(unsigned i=0;i<strlen(text);i++)
		{
			if(text[i]=='\"'&&text[i-1]!='\\')
				flag++;
			else if(flag%2==0&&text[i]=='/'&&i+1<strlen(text)&&text[i+1]=='/')
				while(text[i]!='\n'&&i<strlen(text))
				{
					text[i]=' ';
					i++;
				}
			else if(flag%2==0&&text[i]=='/'&&i+1<strlen(text)&&text[i+1]=='*')
				while(i+1<strlen(text))
				{
					if(text[i+1]=='/'&&text[i]=='*')
					{
						text[i]=' ';
						text[i+1]=' ';
						break;
					}
					text[i]=' ';
					i++;
				}
			else 
				continue;
		}
	}
	void deleteSpace()
	{
		unsigned j=0,k=0;
		unsigned flag=0;
		for(j=0,k=0; text[j]!='\0'; j++)
		{
			if(text[j]!='\r'&&text[j]!='\n'&&text[j]!='\t'&&(text[j]!=' '||flag%2!=0))
				text[k++]=text[j];
			if(text[j]=='\"'&&j>0&&text[j-1]!='\\')
				flag++;
		}
		text[k]=0;
	}
	void deleteNode(Object* root)
	{
		if(root==NULL)
			return;
		if(root->nextObj!=NULL)
			deleteNode(root->nextObj);
		if(root->arr.size()>0)
			for(unsigned i=0;i<root->arr.size();i++)
				deleteNode(root->arr[i]);
		if(root->objVal!=NULL)
			deleteNode(root->objVal);
		delete root;
		root=NULL;
	}
	bool pairBracket()
	{
		unsigned flag=0;
		std::stack<char*> sta;
		for(unsigned i=0;i<strlen(text);i++)
		{
			if((text[i]=='['||text[i]=='{')&&flag%2==0)
				sta.push(text+i);
			else if((text[i]==']'||text[i]=='}')&&flag%2==0)
			{
				if(sta.empty())
					return false;
				if(text[i]==']'&&*sta.top()!='[')
					return false;
				if(text[i]=='}'&&*sta.top()!='{')
					return false;
				bracket.insert(std::pair<char*,char*>{sta.top(),&text[i]});
				sta.pop();
			}
			else if(text[i]=='\"'&&i>0&&text[i-1]!='\\')
				flag++;
			else
				continue;
		}
		if(!sta.empty())
			return false;
		return true;
	}
	bool printObj(char*& buffer,const Object* obj)
	{
		unsigned deep=0;
		char* line=strrchr(buffer,'\n');
		if(line==NULL)
			deep=1;
		else
			deep=buffer+strlen(buffer)-line;
		strcat(buffer,"{\n");
		Object* now=obj->nextObj;
		while(now!=NULL)
		{
			while(memory[buffer]-(int)strlen(buffer)<(int)now->key.size()+(int)now->strVal.size()+64+(int)deep*5)
				buffer=enlargeMemory(buffer);
			for(unsigned i=0;i<deep+4;i++)
				strcat(buffer," ");
			switch(now->type)
			{
			case INT:
				sprintf(buffer,"%s\"%s\":%d,",buffer,now->key.c_str(),now->intVal);
				break;
			case FLOAT:
				sprintf(buffer,"%s\"%s\":%.*lf,",buffer,now->key.c_str(),floNum,now->floVal);
				break;
			case STRING:
				sprintf(buffer,"%s\"%s\":\"%s\",",buffer,now->key.c_str(),now->strVal.c_str());
				break;
			case BOOL:
				if(now->boolVal)
					sprintf(buffer,"%s\"%s\":true,",buffer,now->key.c_str());
				else
					sprintf(buffer,"%s\"%s\":false,",buffer,now->key.c_str());
				break;
			case OBJ:
				sprintf(buffer,"%s\"%s\":",buffer,now->key.c_str());
				printObj(buffer,now->objVal);
				strcat(buffer,",");
				break;
			case ARRAY:
				sprintf(buffer,"%s\"%s\":",buffer,now->key.c_str());
				printArr(buffer,now->arrType,now->arr);
				strcat(buffer,",");
				break;
			case EMPTY:
				sprintf(buffer,"%s\"%s\":null",buffer,now->key.c_str());
				break;
			default:
				error="struct cannot print";
				return false;
			}
			strcat(buffer,"\n");
			now=now->nextObj;
			if(now==NULL)
				*strrchr(buffer,',')=' ';
		}
		for(unsigned i=0;i<deep-1;i++)
			strcat(buffer," ");
		strcat(buffer,"}");
		return true;
	}
	bool printArr(char*& buffer,TypeJson type,const std::vector<Object*>& arr)
	{
		unsigned deep=0;
		char* line=strrchr(buffer,'\n');
		if(line==NULL)
			deep=0;
		else
			deep=buffer+strlen(buffer)-line;
		strcat(buffer,"[\n");
		for(unsigned i=0;i<arr.size();i++)
		{
			while(memory[buffer]-(int)strlen(buffer)<(int)arr[i]->strVal.size()+64+(int)deep*5)
				buffer=enlargeMemory(buffer);
			for(unsigned i=0;i<deep+4;i++)
				strcat(buffer," ");
			switch(type)
			{
			case INT:
				sprintf(buffer,"%s%d,",buffer,arr[i]->intVal);
				break;
			case FLOAT:
				sprintf(buffer,"%s%.*lf,",buffer,floNum,arr[i]->floVal);
				break;
			case STRING:
				sprintf(buffer,"%s\"%s\",",buffer,arr[i]->strVal.c_str());
				break;
			case BOOL:
				if(arr[i]->boolVal)
					strcat(buffer,"true,");
				else
					strcat(buffer,"false,");
				break;
			case OBJ:
				printObj(buffer,arr[i]);
				strcat(buffer,",");
				break;
			case ARRAY:
				printArr(buffer,arr[i]->arrType,arr[i]->arr);
				strcat(buffer,",");
				break;
			default:
				error="struct cannot print";
				return false;
			}
			strcat(buffer,"\n");
			if(i==arr.size()-1)
				*strrchr(buffer,',')=' ';
		}
		for(unsigned i=0;i<deep-1;i++)
			strcat(buffer," ");
		strcat(buffer,"]");
		return true;
	}
};
class FileGet{
private:
	char* pbuffer;
public:
	FileGet()
	{
		pbuffer=NULL;
	}
	~FileGet()
	{
		if(pbuffer!=NULL)
		{
			free(pbuffer);
			pbuffer=NULL;
		}
	}
	static int getFileLen(const char* fileName)
	{
		int len=0;
		FILE* fp=fopen(fileName,"rb");
		if(fp==NULL)
			return -1;
		fseek(fp,0,SEEK_END);
		len=ftell(fp);
		fclose(fp);
		return len;
	}
	static bool getFileMsg(const char* fileName,char* buffer,unsigned int bufferLen)
	{
		unsigned int i=0,len=0;
		len=getFileLen(fileName);
		FILE* fp=fopen(fileName,"rb");
		if(fp==NULL)
			return false;
		for(i=0;i<len&&i<bufferLen;i++)
			buffer[i]=fgetc(fp);
		buffer[i+1]=0;
		fclose(fp);
		return true;
	}
	bool fileStrstr(const char* fileName,const char* strFind)
	{
		int len=0;
		char* pstr=NULL;
		len=this->getFileLen(fileName);
		if(pbuffer!=NULL)
		{
			free(pbuffer);
			pbuffer=NULL;
		}
		FILE* fp=fopen(fileName,"r");
		if(fp==NULL)
			return false;
		pbuffer=(char*)malloc(sizeof(char)*(len+10));
		char* ptemp=pbuffer;
		if(pbuffer==NULL)
			return false;
		memset(pbuffer,0,sizeof(char)*(len+5));
		if(false==this->getFileMsg(fileName,pbuffer,sizeof(char)*(len+10)))
			return false;
		while((*ptemp<65||*ptemp>122)&&ptemp<pbuffer+sizeof(char)*len)
			ptemp++;
		pstr=strstr(ptemp,strFind);
		if(pbuffer!=NULL)
		{
			free(pbuffer);
			pbuffer=NULL;
		}
		fclose(fp);
		if(pstr!=NULL)
			return true;
		else
			return false;
		return false;
	}
	const char* getFileBuff(const char* fileName)
	{
		if(pbuffer!=NULL)
			free(pbuffer);
		int len=getFileLen(fileName);
		if(len==-1)
			return NULL;
		pbuffer=(char*)malloc(sizeof(char)*getFileLen(fileName)+10);
		if(pbuffer==NULL)
			return NULL;
		getFileMsg(fileName,pbuffer,sizeof(char)*getFileLen(fileName)+10);
		return pbuffer;
	}
	static bool writeToFile(const char* fileName,const char* buffer,unsigned int writeLen)
	{
		FILE* fp=fopen(fileName,"wb+");
		if(fp==NULL)
			return false;
		for(unsigned int i=0;i<writeLen;i++)
			fputc(buffer[i],fp);
		fclose(fp);
		return true;
	}
};
#endif
