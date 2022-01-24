#include <string.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <stack>
#include <unordered_map>
class Json{
public:
	enum TypeJson{
		INT=0,FLOAT=1,ARRAY=2,OBJ=3,STRING=4,BOOL=5,STRUCT=6
	};
	struct Object{
		TypeJson type;
		TypeJson arrType;
		std::string key;
		std::string strVal;
		std::vector<Object*> arr;
		Object* objVal;
		Object* nextObj;
		float floVal;
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
	char* text;
	const char* error;
	unsigned maxLen;
	unsigned floNum;
	Object* obj;
	std::vector<char*> memory;
	std::unordered_map<std::string,Object*> hashMap;
	std::unordered_map<char*,char*> bracket;
public:
	Json(unsigned bufflen)
	{
		error=NULL;
		obj=NULL;
		maxLen=256;
		floNum=3;
		text=(char*)malloc(sizeof(char)*bufflen+10);
		if(text==NULL)
		{
			error="malloc wrong";
			return;
		}
		memset(text,0,sizeof(char)*bufflen+10);
	}
	Json(const char* jsonText)
	{
		error=NULL;
		obj=NULL;
		maxLen=256;
		floNum=3;
		if(jsonText==NULL||strlen(jsonText)==0)
		{
			error="message error";
			return;
		}
		text=(char*)malloc(strlen(jsonText)+10);
		memset(text,0,strlen(jsonText)+10);
		if(text==NULL)
		{
			error="malloc wrong";
			return;
		}
		memset(text,0,strlen(jsonText)+10);
		strcpy(text,jsonText);
		deleteSpace();
		if(false==pairBracket())
		{
			error="pair bracket wrong";
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
		if(text!=NULL)
			free(text);
		if(!memory.empty())
			for(unsigned i=0;i<memory.size();i++)
				free(memory[i]);
	}
	Object* operator[](const char* key)
	{
		if(hashMap.find(std::string(key))==hashMap.end())
			return NULL;
		return hashMap.find(std::string(key))->second;
	}
	bool addKeyVal(char* obj,TypeJson type,const char* key,...)
	{
		if(obj==NULL)
		{
			error="null buffer";
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
		sprintf(obj,"%s\"%s\":",obj,key);
		int valInt=0;
		char* valStr=NULL;
		float valFlo=9;
		bool valBool=false;
		switch(type)
		{
		case INT:
			valInt=va_arg(args,int);
			sprintf(obj,"%s%d",obj,valInt);
			break;
		case FLOAT:
			valFlo=va_arg(args,double);
			sprintf(obj,"%s%.*f",obj,floNum,valFlo);
			break;
		case STRING:
			valStr=va_arg(args,char*);
			if(valStr==NULL)
			{
				error="null input";
				return false;
			}
			sprintf(obj,"%s\"%s\"",obj,valStr);
			break;
		case BOOL:
			valBool=va_arg(args,int);
			if(valBool==true)
				strcat(obj,"true");
			else
				strcat(obj,"false");
			break;
		case OBJ:
		case ARRAY:
			valStr=va_arg(args,char*);
			if(valStr==NULL)
			{
				error="null input";
				return false;
			}
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
	char* createObject(unsigned maxBuffLen)
	{
		char* now=(char*)malloc(sizeof(char)*maxBuffLen);
		if(now==NULL||maxBuffLen<4)
		{
			error="init worng";
			return NULL;
		}
		else
			memory.push_back(now);
		memset(now,0,sizeof(char)*maxBuffLen);
		strcpy(now,"{}");
		return now;
	}
	char* createArray(unsigned maxBuffLen,TypeJson type,unsigned arrLen,void* arr)
	{
		if(arr==NULL||maxBuffLen<4)
		{
			error="null input";
			return NULL;
		}
		char* now=(char*)malloc(sizeof(char)*maxBuffLen);
		if(now==NULL)
		{
			error="malloc worng";
			return NULL;
		}
		else
			memory.push_back(now);
		memset(now,0,sizeof(char)*maxBuffLen);
		strcat(now,"[");
		int* arrInt=(int*)arr;
		float* arrFlo=(float*)arr;
		char** arrStr=(char**)arr;
		bool* arrBool=(bool*)arr;
		unsigned i=0;
		switch(type)
		{
		case INT:
			for(i=0;i<arrLen;i++)
				sprintf(now,"%s%d,",now,arrInt[i]);
			break;
		case FLOAT:
			for(i=0;i<arrLen;i++)
				sprintf(now,"%s%.*f,",now,floNum,arrFlo[i]);
			break;
		case STRING:
			for(i=0;i<arrLen;i++)
				sprintf(now,"%s\"%s\",",now,arrStr[i]);
			break;
		case OBJ:
		case ARRAY:
			for(i=0;i<arrLen;i++)
				sprintf(now,"%s%s,",now,arrStr[i]);
			break;
		case BOOL:
			for(i=0;i<arrLen;i++)
			{
				if(arrBool)
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
	inline Object* getRootObj()
	{
		return obj;
	}
	inline const char* lastError()
	{
		return error;
	}
	inline void changeSetting(unsigned keyValMaxLen,unsigned floNum)
	{
		this->maxLen=keyValMaxLen>maxLen?keyValMaxLen:maxLen;
		this->floNum=floNum;
	}
private:
	Object* analyseObj(char* begin,char* end)
	{
		Object * root=new Object,*last=root;
		root->type=STRUCT;
		char* now=begin+1,*next=now;
		char* word=(char*)malloc(sizeof(char)*maxLen),*val=(char*)malloc(sizeof(char)*maxLen),temp=*end;
		if(word==NULL||val==NULL)
		{
			error="malloc wrong";
			return NULL;
		}
		memset(word,0,sizeof(char)*maxLen);
		memset(val,0,sizeof(char)*maxLen);
		*end=0;
		while(now<end)
		{
			Object* nextObj=new Object;
			memset(word,0,sizeof(char)*maxLen);
			findString(now,word,maxLen);
			nextObj->key=word;
			hashMap.insert(std::pair<std::string,Object*>{word,nextObj});
			now+=strlen(word)+3;
			if(*now=='\"')
			{
				nextObj->type=STRING;
				next=strchr(now+1,'\"');
				while(*(next-1)=='\\')
					next=strchr(next+1,'\"');
				if(next==NULL)
				{
					error="string wrong";
					return NULL;
				}
				for(unsigned i=0;now+i+1<next;i++)
					val[i]=*(now+i+1);
				val[strlen(val)]=0;
				nextObj->strVal=val;
				now=next+1;
				if(*now==',')
					now++;
			}
			else if('0'<*now&&'9'>*now)
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
					sscanf(now,"%f",&nextObj->floVal);
				now=next+1;
			}
			else if(*now=='[')
			{
				next=bracket[now];
				if(next==NULL)
				{
					error="format wrong";
					return NULL;
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
					return NULL;
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
			last->nextObj=nextObj;
			last=nextObj;
		}
		*end=temp;
		free(word);
		free(val);
		return root;
	}
	TypeJson analyseArray(char* begin,char* end,std::vector<Object*>& array)
	{
		char* now=begin+1,*next=end,*word=(char*)malloc(sizeof(maxLen));
		if(word==NULL)
		{
			error="malloc wrong";
			return INT;
		}
		memset(word,0,sizeof(char)*maxLen);
		Object* nextObj=NULL;
		if(*now>='0'&&*now<='9')
		{
			next=now;
			while(next<end&&*next!=',')
				next++;
			TypeJson type=judgeNum(now,next);
			while(now<end&&now!=NULL)
			{
				nextObj=new Object;
				nextObj->isData=true;
				if(nextObj->type==INT)
				{
					findNum(now,type,&nextObj->intVal);
					array.push_back(nextObj);
				}
				else
				{
					findNum(now,type,&nextObj->floVal);
					array.push_back(nextObj);
				}
				now=strchr(now+1,',');
				if(now!=NULL)
					now++;
			}
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
				array.push_back(nextObj);
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
				array.push_back(nextObj);
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
				array.push_back(nextObj);
				now=next;
				now=strchr(now+1,',');
				if(now==NULL)
					break;
				now+=1;
			}
		}
		free(word);
		return nextObj->type;
	}
	void findString(const char* begin,char* buffer,unsigned buffLen)
	{
		const char* now=begin+1,*next=now;
		next=strchr(now+1,'\"');
		while(*(next-1)=='\\')
			next=strchr(next+1,'\"');
		for(unsigned i=0;now+i<next&&i<buffLen;i++)
			buffer[i]=*(now+i);
		buffer[strlen(buffer)]=0;
	}
	void findNum(const char* begin,TypeJson type,void* pnum)
	{
		if(type==INT)
			sscanf(begin,"%d",(int*)pnum);
		else
			sscanf(begin,"%f",(float*)pnum);
	}
	inline TypeJson judgeNum(const char* begin,const char* end)
	{
		for(unsigned i=0;i+begin<end;i++)
			if(begin[i]=='.')
				return FLOAT;
		return INT;
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
		std::stack<char*> sta;
		for(unsigned i=0;i<strlen(text);i++)
		{
			if(text[i]=='['||text[i]=='{')
				sta.push(text+i);
			else if(text[i]==']'||text[i]=='}')
			{
				if(text[i]==']'&&*sta.top()!='[')
					return false;
				if(text[i]=='}'&&*sta.top()!='{')
					return false;
				bracket.insert(std::pair<char*,char*>{sta.top(),&text[i]});
				sta.pop();
			}
			else
				continue;
		}
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
int main()
{
	char temp[1000]={0};
	FileGet file;
	file.getFileMsg("./a.json",temp,1000);
	printf("%s\n",temp);
	Json json(temp);
	Json::Object* name=json["last"]->objVal;
	std::cout<<(*name)["ad"]->strVal<<std::endl;
	if(name==NULL)
		return 0;
	name=name->nextObj;
	while(name!=NULL)
	{
		std::cout<<name->key<<":"<<name->strVal<<std::endl;
		name=name->nextObj;
	}
	std::cout<<json["first Name"]->strVal<<std::endl;
	std::cout<<json["ad"]->strVal<<std::endl;
	std::cout<<json["money"]->intVal<<std::endl;
	std::cout<<json["haha"]->boolVal<<std::endl;
	std::cout<<json["email"]->strVal<<std::endl;
	std::cout<<json["try"]->arr.size()<<std::endl;
	for(unsigned i=0;i<json["try"]->arr.size();i++)
	{
		std::cout<<json["try"]->arr[i]<<std::endl;
		auto obj=*json["try"]->arr[i];
		std::cout<<" "<<obj["ad"]->strVal<<std::endl;
	}
	char* result=json.createObject(300);
	char* tempNow=json.createObject(200);
	int arrInt[10]={1,2,3,4,5,6,7,8,9,0};
	char* arr=json.createArray(200,Json::INT,10,arrInt);
	if(result==NULL)
		return -1;
	json.addKeyVal(tempNow,Json::STRING,"io","kl");
	json.addKeyVal(tempNow,Json::ARRAY,"er",arr);
	json.addKeyVal(result,Json::FLOAT,"ha",89.1123);
	json.addKeyVal(result,Json::STRING,"io","kl");
	json.addKeyVal(result,Json::OBJ,"lp",tempNow);
	json.addKeyVal(result,Json::BOOL,"45",true);
	printf("json:%s\n",result);
	return 0;
}
