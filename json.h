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
			Json json;
			type=ARRAY;
			char* now=NULL;
			T* arr=new T[listInit.size()];
			int i=0;
			for(auto iter=listInit.begin();iter!=listInit.end();iter++)
			{
				arr[i]=*iter;
				i++;
			}
			int arrlen=listInit.size();
			if(std::is_same<T,int>::value)
				now=json.createArray(INT,arrlen,(void*)arr);
			else if(std::is_same<T,double>::value)
				now=json.createArray(FLOAT,arrlen,(void*)arr);
			else if(std::is_same<T,const char*>::value)
				now=json.createArray(STRING,arrlen,(void*)arr);
			else if(std::is_same<T,bool>::value)
				now=json.createArray(BOOL,arrlen,(void*)arr);
			else
			{
				error="type wrong";
				type=EMPTY;
			}
			pval=malloc(sizeof(char)*strlen(now)+10);
			len=sizeof(char)*strlen(now)+10;
			strcpy((char*)pval,now);
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
private:
	char* text;
	const char* error;
	const char* nowKey;
	char* word;
	char* result;
	unsigned maxLen;
	unsigned floNum;
	unsigned defaultSize;
	bool isCheck;
	Object* obj;
	std::unordered_map<char*,unsigned> memory;
	std::unordered_map<char*,char*> bracket;
public:
	Json()
	{
		error=NULL;
		obj=NULL;
		text=NULL;
		word=NULL;
		result=NULL;
		nowKey=NULL;
		isCheck=true;
		maxLen=256;
		floNum=3;
		defaultSize=128;
		result=this->createObject();
		word=(char*)malloc(sizeof(char)*maxLen);
		if(word==NULL||result==NULL)
		{
			error="malloc wrong";
			return;
		}
		memset(word,0,sizeof(char)*maxLen);
	}
	Json(std::initializer_list<std::pair<std::string,InitType>> initList):Json()
	{
		isCheck=false;
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
				if(false==addKeyVal(result,STRING,iter->first.c_str(),iter->second.pval))
					return;
				break;
			case INT:
				if(false==addKeyVal(result,INT,iter->first.c_str(),*(int*)iter->second.pval))
					return;
				break;
			case FLOAT:
				if(false==addKeyVal(result,FLOAT,iter->first.c_str(),*(double*)iter->second.pval))
					return;
				break;
			case BOOL:
				if(false==addKeyVal(result,BOOL,iter->first.c_str(),*(bool*)iter->second.pval))
					return;
				break;
			case EMPTY:
				if(false==addKeyVal(result,EMPTY,iter->first.c_str(),NULL))
					return;
				break;
			case OBJ:
				if(false==addKeyVal(result,OBJ,iter->first.c_str(),(char*)iter->second.pval))
					return;
				break;
			case ARRAY:
				if(false==addKeyVal(result,ARRAY,iter->first.c_str(),(char*)iter->second.pval))
					return;
				break;
			default:
				return;
			}
		}
		isCheck=true;
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
	~Json()
	{
		deleteNode(obj);
		if(word!=NULL)
		{
			free(word);
			word=NULL;
		}
		if(text!=NULL)
		{
			free(text);
			result=NULL;
		}
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
		memory.insert(std::pair<char*,unsigned>{buffer,sizeof(char)*defaultSize*10});
		printObj(buffer,exmaple);
		return buffer;
	}
	template<typename T>
	bool addKeyVal(char*& obj,const char* key,T value)
	{
		if(std::is_same<T,int>::value)
			return addKeyVal(obj,INT,key,value);
		else if(std::is_same<T,double>::value)
			return addKeyVal(obj,FLOAT,key,value);
		else if(std::is_same<T,char*>::value)
			return addKeyVal(obj,OBJ,key,value);
		else if(std::is_same<T,const char*>::value||std::is_same<T,std::string>::value)
			return addKeyVal(obj,STRING,key,value);
		else if(std::is_same<T,bool>::value)
			return addKeyVal(obj,BOOL,key,value);
		else 
			return addKeyVal(obj,EMPTY,key,NULL);
	}
	bool addKeyVal(char*& obj,TypeJson type,const char* key,...)
	{
		if(obj==NULL)
		{
			error="null buffer";
			return false;
		}
		if(key==NULL)
		{
			error="key null";
			return false;
		}
		va_list args;
		va_start(args,key);
		if(obj[strlen(obj)-1]=='}')
		{
			if(obj[strlen(obj)-2]!='{')
				obj[strlen(obj)-1]=',';
			else
				obj[strlen(obj)-1]=0;
		}
		if(memory.find(obj)==memory.end())
		{
			error="wrong object";
			return false;
		}
		while(memory[obj]-strlen(obj)<strlen(key)+4)
			obj=enlargeMemory(obj);
		sprintf(obj,"%s\"%s\":",obj,key);
		int valInt=0;
		char* valStr=NULL;
		double valFlo=9;
		bool valBool=false;
		switch(type)
		{
		case INT:
			valInt=va_arg(args,int);
			while(memory[obj]-strlen(obj)<15)
				obj=enlargeMemory(obj);
			sprintf(obj,"%s%d",obj,valInt);
			break;
		case FLOAT:
			valFlo=va_arg(args,double);
			while(memory[obj]-strlen(obj)<15)
				obj=enlargeMemory(obj);
			sprintf(obj,"%s%.*lf",obj,floNum,valFlo);
			break;
		case STRING:
			valStr=va_arg(args,char*);
			if(valStr==NULL)
			{
				error="null input";
				return false;
			}
			while(memory[obj]-strlen(obj)<strlen(obj)+5)
				obj=enlargeMemory(obj);
			sprintf(obj,"%s\"%s\"",obj,valStr);
			break;
		case EMPTY:
			while(memory[obj]-strlen(obj)<5)
				obj=enlargeMemory(obj);
			strcat(obj,"null");
			break;
		case BOOL:
			valBool=va_arg(args,int);
			while(memory[obj]-strlen(obj)<5)
				obj=enlargeMemory(obj);
			if(valBool==true)
				strcat(obj,"true");
			else
				strcat(obj,"false");
			break;
		case OBJ:
		case ARRAY:
			valStr=va_arg(args,char*);
			while(memory[obj]-strlen(obj)<strlen(obj)+5)
				obj=enlargeMemory(obj);
			if(valStr==NULL)
			{
				error="null input";
				return false;
			}
			if(isCheck)
			{
				if(memory.find(valStr)!=memory.end())
					sprintf(obj,"%s%s",obj,valStr);
				else
					sprintf(obj,"%s\"%s\"",obj,valStr);
			}
			else
				sprintf(obj,"%s%s",obj,valStr);
			break;
		default:
			error="can not insert this type";
			strcat(obj,"}");
			return false;
		}
		strcat(obj,"}");
		return true;
	}
	char* createObject()
	{
		char* now=(char*)malloc(sizeof(char)*defaultSize);
		if(now==NULL)
		{
			error="init worng";
			return NULL;
		}
		else
			memory.insert(std::pair<char*,unsigned>{now,defaultSize});
		memset(now,0,sizeof(char)*defaultSize);
		strcpy(now,"{}");
		return now;
	}
	template<typename T>
	char* createArray(std::vector<T>& arr)
	{
		char* result=NULL;
		if(std::is_same<T,int>::value)
			result=createArray(INT,arr.size(),&arr[0]);
		else if(std::is_same<T,double>::value)
			result=createArray(FLOAT,arr.size(),&arr[0]);
		else if(std::is_same<T,std::string>::value)
			result=createArray(STRUCT,arr.size(),&arr);
		else if(std::is_same<T,char*>::value)
			result=createArray(OBJ,arr.size(),&arr[0]);
		else if(std::is_same<T,const char*>::value)
			result=createArray(STRING,arr.size(),&arr[0]);
		else if(std::is_same<T,bool>::value)
			result=createArray(BOOL,arr.size(),&arr[0]);
		else
			error="wrong vector type";
		return result;
	}
	char* createArray(TypeJson type,unsigned arrLen,void* arr)
	{
		if(arr==NULL)
		{
			error="null input";
			return NULL;
		}
		char* now=(char*)malloc(sizeof(char)*defaultSize);
		if(now==NULL)
		{
			error="malloc worng";
			return NULL;
		}
		else
			memory.insert(std::pair<char*,unsigned>{now,defaultSize});
		memset(now,0,sizeof(char)*defaultSize);
		strcat(now,"[");
		int* arrInt=(int*)arr;
		double* arrFlo=(double*)arr;
		char** arrStr=(char**)arr;
		bool* arrBool=(bool*)arr;
		auto& pvect=*(std::vector<std::string>*)arr;
		unsigned i=0;
		switch(type)
		{
		case INT:
			for(i=0;i<arrLen;i++)
			{
				while(memory[now]-strlen(now)<std::to_string(arrInt[i]).size()+3)
					now=enlargeMemory(now);
				sprintf(now,"%s%d,",now,arrInt[i]);
			}
			break;
		case FLOAT:
			for(i=0;i<arrLen;i++)
			{
				while(memory[now]-strlen(now)<std::to_string(arrInt[i]).size()+3)
					now=enlargeMemory(now);
				sprintf(now,"%s%.*lf,",now,floNum,arrFlo[i]);
			}
			break;
		case STRING:
			for(i=0;i<arrLen;i++)
			{
				while(memory[now]-strlen(now)<strlen(arrStr[i])+5)
					now=enlargeMemory(now);
				sprintf(now,"%s\"%s\",",now,arrStr[i]);
			}
			break;
		case STRUCT:
			for(i=0;i<arrLen;i++)
			{
				while(memory[now]-strlen(now)<pvect[i].size()+5)
					now=enlargeMemory(now);
				sprintf(now,"%s\"%s\",",now,pvect[i].c_str());
			}
			break;
		case OBJ:
		case ARRAY:
			for(i=0;i<arrLen;i++)
			{
				while(memory[now]-strlen(now)<strlen(arrStr[i])+4)
					now=enlargeMemory(now);
				if(isCheck)
				{
					if(memory.find(arrStr[i])!=memory.end())
						sprintf(now,"%s%s,",now,arrStr[i]);
					else
						sprintf(now,"%s\"%s\",",now,arrStr[i]);
				}
				else
					sprintf(now,"%s%s,",now,arrStr[i]);
			}
			break;
		case BOOL:
			for(i=0;i<arrLen;i++)
			{
				while(memory[now]-strlen(now)<6)
					now=enlargeMemory(now);
				if(arrBool[i])
					strcat(now,"true,");
				else
					strcat(now,"false,");
			}
			break;
		default:
			error="struct cannot be a array";
			break;
		}
		if(now[strlen(now)-1]==',')
			now[strlen(now)-1]=']';
		else
			strcat(now,"]");
		now[strlen(now)]=0;
		return now;
	}
	inline Object* operator[](const char* key)
	{
		return (*obj)[key];
	}
	char*& operator()()
	{
		return result;
	}
	Json& operator()(const char* key)
	{
		nowKey=key;
		return *this;
	}
	template<typename T>
	Json& operator=(T value)
	{
		this->addKeyVal(this->result,nowKey,value);
		return *this;
	}
	template<typename T>
	Json& operator=(std::vector<T> value)
	{
		char* arr=this->createArray(value);
		this->addKeyVal(this->result,nowKey,arr);
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
	inline void changeSetting(unsigned defaultSize=128,unsigned keyValMaxLen=256,unsigned floNum=3)
	{
		this->defaultSize=defaultSize;
		this->maxLen=keyValMaxLen>maxLen?keyValMaxLen:maxLen;
		this->floNum=floNum;
	}
private:
	char* enlargeMemory(char* old)
	{
		if(memory.find(old)==memory.end())
			return old;
		unsigned temp=memory[old];
		temp*=2;
		void* strTemp=realloc(old,temp);
		if(strTemp==NULL)
			return old;
		memory.erase(memory.find(old));
		memory.insert(std::pair<char*,unsigned>{(char*)strTemp,temp});
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
			while(memory[buffer]-strlen(buffer)<now->key.size()+now->strVal.size()+20+deep*5)
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
			for(unsigned i=0;i<deep+4;i++)
				strcat(buffer," ");
			while(memory[buffer]-strlen(buffer)<arr[i]->strVal.size()+20+deep*5)
				buffer=enlargeMemory(buffer);
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
