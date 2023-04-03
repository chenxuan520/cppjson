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
namespace cppweb{
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
		bool operator==(const Object& old)
		{
			return &old==this;
		}
		bool operator!=(const Object& old)
		{
			return &old!=this;
		}
		Object& operator[](const char* key)
		{
			Object* now=this->nextObj;
			while(now!=NULL)
			{
				if(now->key==key)
				{
					if(now->type==OBJ)
						return *now->objVal;
					else
						return *now;
				}
				now=now->nextObj;
			}
			return Json::npos;
		}
		Object& operator[](unsigned pos)
		{
			if(this->type!=ARRAY||this->arr.size()<=pos)
				return Json::npos;
			else
				return *this->arr[pos];
		}
		Object& excapeChar()
		{
			auto temp=strVal;
			strVal.clear();
			for(unsigned i=0;i<temp.size();i++)
			{
				if(temp[i]=='\\')
					i++;
				if(i>=temp.size())
					break;
				strVal+=temp[i];
			}
			return *this;
		}
	};
	static Object npos;
	class Node;
private:
	struct InitType{//struct for ctreate such as {{"as","ds"}} in init;
		std::string val;
		InitType(std::initializer_list<std::pair<std::string,InitType>> listInit)
		{
			Json::Node node(listInit);
			val=node();
		}
		template<typename T>
		InitType(std::initializer_list<T> listInit)
		{
			std::vector<T> arr(listInit.size());
			int i=0;
			for(auto iter=listInit.begin();iter!=listInit.end();iter++)
			{
				arr[i]=*iter;
				i++;
			}
			int arrlen=listInit.size();
			Json::Node node(arr,arrlen);
			val=node();
		}
		InitType(const Json::Node& val)
		{
			Json::Node temp=val;
			this->val=temp();
		}
		template<typename T>
		InitType(T val)
		{
			this->val=std::to_string(val);
		}
		InitType(bool val)
		{
			if(val)
				this->val="true";
			else
				this->val="false";
		}
		InitType(std::nullptr_t)
		{
			this->val="null";
		}
		template<typename T>
		InitType(const std::vector<T> data)
		{
			Json::Node node(data);
			val=node();
		}
		InitType(std::string strVal)
		{
			createChar(strVal);
		}
		InitType(const char* pt)
		{
			createChar(pt);
		}
		InitType(char* pt)
		{
			createChar(pt);
		}
		template<typename T>
		void createChar(T data)
		{
			val+='\"';
			val+=data;
			val+='\"';
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
				addKeyValue(ARRAY,iter->first.c_str(),iter->second.val.c_str());
		}
		inline const char* operator()() const
		{
			return result.c_str();
		}
		inline const char* getStr() const
		{
			return result.c_str();
		}
		Node& operator[](const char* key)
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
		Node& operator=(std::string data)
		{
			addKeyValue(key.c_str(),data.c_str());
			return *this;
		}
		Node& operator=(std::initializer_list<std::pair<std::string,InitType>> initList)
		{
			Node node(initList);
			addKeyValue(key.c_str(),node);
			return *this;
		}
		unsigned inline getLen()
		{
			return result.size();
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
		template<typename T>
		bool addArray(const std::vector<std::vector<T>>& arr)
		{
			std::vector<Node> temp;
			for(auto& now:arr)
				temp.push_back(Node(now));
			return addArray(temp);
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
					result+=arrStr[i];
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
			case ARRAY:
				valStr=va_arg(args,char*);
				if(valStr==NULL)
				{
					error="null input";
					return false;
				}
				result+=valStr;
				break;
			case OBJ:
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
		inline bool addKeyValue(const char* key,int data)
		{
			return addKeyValue(INT,key,data);
		}
		inline bool addKeyValue(const char* key,double data)
		{
			return addKeyValue(FLOAT,key,data);
		}
		inline bool addKeyValue(const char* key,bool data)
		{
			return addKeyValue(BOOL,key,data);
		}
		inline bool addKeyValue(const char* key,Node data)
		{
			return addKeyValue(OBJ,key,&data);
		}
		inline bool addKeyValue(const char* key,char* data)
		{
			return addKeyValue(STRING,key,data);
		}
		inline bool addKeyValue(const char* key,const char* data)
		{
			return addKeyValue(STRING,key,data);
		}
		template<typename T>
		bool addKeyValue(const char* key,T)
		{
			return addKeyValue(EMPTY,key,NULL);
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
	Object* obj;
	Node node;
	unsigned floNum;
	std::string wordStr;
	std::string formatStr;
	std::unordered_map<char*,char*> bracket;
public:
	Json():text(NULL),error(NULL),nowKey(NULL),obj(NULL),floNum(3){}
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
			return;
	}
	Json(const Json& old)=delete;
	~Json()
	{
		deleteNode(obj);
		if(text!=NULL)
			free(text);
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
			return false;
		return true;
	}
	const char* formatPrint(Object& example)
	{
		formatStr.clear();
		if(obj==NULL)
			return NULL;
		printObj(formatStr,&example);
		return formatStr.c_str();
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
	inline const char* operator()() const
	{
		return node();
	}
	inline const char* getStr() const
	{
		return node();
	}
	Json& operator[](const char* key)
	{
		nowKey=key;
		return *this;
	}
	template<typename T>
	Json& operator=(T value)
	{
		node[nowKey]=value;
		return *this;
	}
	inline Object& getRootObj()
	{
		if(obj!=NULL)
			return *obj;
		else
			return Json::npos;
	}
	inline const char* lastError()
	{
		return error;
	}
	inline bool changeSetting(unsigned floNum)
	{
		if(floNum>255)
		{
			error="float num max is 255";
			return false;
		}
		this->floNum=floNum;
		node.changeSetting(floNum);
		return true;
	}
	static const char* createJson(const Node& temp)
	{
		return temp();
	}
private:
	Object* analyseObj(char* begin,char* end)
	{
		Object * root=new Object,*last=root;
		root->type=STRUCT;
		char* now=begin+1,*next=now;
		char temp=*end;
		*end=0;
		while(now<end)
		{
			Object* nextObj=new Object;
			now=findString(now,wordStr);
			if(now==NULL)
			{
				error="find key wrong";
				return NULL;
			}
			nextObj->key=wordStr;
			now+=2;
			if(*now=='\"')
			{
				nextObj->type=STRING;
				now=findString(now,wordStr);
				if(now==NULL)
				{
					error="string judge wrong";
					return NULL;
				}
				nextObj->strVal=wordStr;
				now++;
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
				now=findString(now,wordStr);
				if(now==NULL)
				{
					error="fing string wrong";
					return STRING;
				}
				nextObj=new Object;
				nextObj->type=STRING;
				nextObj->isData=true;
				nextObj->strVal=wordStr;
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
	char* findString(char* begin,std::string& buffer)
	{
		char* now=begin+1,*nextOne=now;
		buffer.clear();
		nextOne=strchr(now,'\"');
		while(nextOne!=NULL&&*(nextOne-1)=='\\')
			nextOne=strchr(nextOne+1,'\"');
		if(nextOne==NULL)
		{
			printf("wrong:\n\%s\n\n",begin);
			error="text wrong";
			return NULL;
		}
		for(unsigned i=0;now+i<nextOne;i++){
			if (*(now+i)=='\\') {
				i+=1;
				if(now+i>=nextOne){
					break;
				}
			}
			buffer+=*(now+i);
		}
		return nextOne;
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
		bracket.clear();
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
	void printObj(std::string& buffer,const Object* obj)
	{
		if(obj==NULL)
		{
			buffer+="{}";
			error="message wrong";
			return;
		}
		unsigned deep=0;
		char word[256]={0};
		auto line=buffer.find_last_of('\n');
		if(line==buffer.npos)
			deep=1;
		else
			deep=buffer.c_str()+buffer.size()-&buffer[line];
		buffer+="{\n";
		Object* now=obj->nextObj;
		while(now!=NULL)
		{
			for(unsigned i=0;i<deep+4;i++)
				buffer+=' ';
			buffer+='\"'+now->key+"\":";
			switch(now->type)
			{
			case INT:
				buffer+=std::to_string(now->intVal)+',';
				break;
			case FLOAT:
				sprintf(word,"%.*lf,",floNum,now->floVal);
				buffer+=word;
				break;
			case STRING:
				buffer+='\"'+now->strVal+"\",";
				break;
			case BOOL:
				if(now->boolVal)
					buffer+="true,";
				else
					buffer+="false,";
				break;
			case OBJ:
				printObj(buffer,now->objVal);
				buffer+=',';
				break;
			case ARRAY:
				printArr(buffer,now->arrType,now->arr);
				buffer+=',';
				break;
			case EMPTY:
				buffer+="null,";
				break;
			default:
				error="struct cannot print";
			}
			buffer+='\n';
			now=now->nextObj;
			if(now==NULL)
			{
				auto pos=buffer.find_last_of(',');
				if(pos!=buffer.npos)
					buffer[pos]=' ';
			}
		}
		for(unsigned i=0;i<deep-1;i++)
			buffer+=" ";
		buffer+="}";
		return;
	}
	void printArr(std::string& buffer,TypeJson type,const std::vector<Object*>& arr)
	{
		unsigned deep=0;
		char word[256]={0};
		auto line=buffer.find_last_of('\n');
		if(line==buffer.npos)
			deep=1;
		else
			deep=buffer.c_str()+buffer.size()-&buffer[line];
		buffer+="[\n";
		for(unsigned i=0;i<arr.size();i++)
		{
			for(unsigned i=0;i<deep+4;i++)
				buffer+=" ";
			switch(type)
			{
			case INT:
				buffer+=std::to_string(arr[i]->intVal)+',';
				break;
			case FLOAT:
				sprintf(word,"%.*lf,",floNum,arr[i]->floVal);
				buffer+=word;
				break;
			case STRING:
				buffer+='\"'+arr[i]->strVal+"\",";
				break;
			case BOOL:
				if(arr[i]->boolVal)
					buffer+="true,";
				else
					buffer+="false,";
				break;
			case OBJ:
				printObj(buffer,arr[i]);
				buffer+=',';
				break;
			case ARRAY:
				printArr(buffer,arr[i]->arrType,arr[i]->arr);
				buffer+=',';
				break;
			case EMPTY:
				buffer+="null,";
				break;
			default:
				error="struct cannot print";
			}
			buffer+='\n';
			if(i==arr.size()-1)
			{
				auto pos=buffer.find_last_of(',');
				if(pos!=buffer.npos)
					buffer[pos]=' ';
			}
		}
		for(unsigned i=0;i<deep-1;i++)
			buffer+=" ";
		buffer+="]";
	}
};
Json::Object Json::npos;
}
#endif
