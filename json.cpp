#include <string.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <stack>
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
//private:
	char* text;
	const char* error;
	Object* obj;
	std::unordered_map<std::string,Object*> hashMap;
	std::unordered_map<char*,char*> bracket;
public:
	Json(unsigned bufflen)
	{
		text=(char*)malloc(sizeof(char)*bufflen);
	}
	Json(const char* jsonText)
	{
		error=NULL;
		text=(char*)malloc(strlen(jsonText)+10);
		if(text==NULL)
		{
			error="malloc wrong";
			return;
		}
		memset(text,0,strlen(jsonText)+10);
		strcpy(text,jsonText);
		deleteSpace();
		pairBracket();
		printf("%s\n",text);
		char* end=bracket[text];
		obj=analyseObj(text,end);
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
	}
	Object* operator[](const char* key)
	{
		if(hashMap.find(std::string(key))==hashMap.end())
			return NULL;
		return hashMap.find(std::string(key))->second;
	}
	inline const char* lastError()
	{
		return error;
	}
private:
	Object* analyseObj(char* begin,char* end)
	{
		Object * root=new Object,*last=root;
		root->type=STRUCT;
		char* now=begin+1,*next=now;
		char word[256]={0},val[256]={0},temp=*end;
		*end=0;
		while(now<end)
		{
			Object* nextObj=new Object;
			memset(word,0,sizeof(char)*256);
			findString(now,word,256);
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
		return root;
	}
	TypeJson analyseArray(char* begin,char* end,std::vector<Object*>& array)
	{
		char* now=begin+1,*next=end,word[256]={0};
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
				findString(now,word,256);
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
	void pairBracket()
	{
		std::stack<char*> sta;
		for(unsigned i=0;i<strlen(text);i++)
		{
			if(text[i]=='['||text[i]=='{')
				sta.push(text+i);
			else if(text[i]==']'||text[i]=='}')
			{
				bracket.insert(std::pair<char*,char*>{sta.top(),&text[i]});
				sta.pop();
			}
			else
				continue;
		}
	}
};
class JsonOld{//a easy json class to create json
private:
	char* buffer;
	char word[30];
	const char* text;
	const char* obj;
	const char* error;
	unsigned int nowLen;
	unsigned int maxLen;
public:
	enum TypeJson{
		INT=0,FLOAT=1,ARRAY=2,OBJ=3,STRING=4,STRUCT=5,
	};
	struct Object{
		TypeJson type;
		TypeJson arrTyp;
		const char* key;
		int valInt;
		float valFlo;
		unsigned int floOut;
		unsigned int arrLen;
		const char* valStr;
		void** array;
		Object* pobj;
	public:
		Object()
		{
			type=INT;
			arrTyp=INT;
			key=NULL;
			valFlo=0;
			valInt=0;
			valStr=NULL;
			array=NULL;
			pobj=NULL;
		}
	};
public:
	JsonOld()
	{
		buffer=NULL;
		text=NULL;
		obj=NULL;
		error=NULL;
		nowLen=0;
		maxLen=0;
		memset(this->word,0,sizeof(char)*30);
	}
	JsonOld(const char* jsonText)
	{
		text=jsonText;
		buffer=NULL;
		nowLen=0;
		maxLen=0;
		memset(this->word,0,sizeof(char)*30);		
	}
	~JsonOld()
	{
		if(this->buffer!=NULL)
			free(buffer);
	}
	bool init(unsigned int bufferLen)//malloc len size of buff
	{
		if(bufferLen<=10)
		{
			error="buffer len too small";
			return false;
		}
		buffer=(char*)malloc(sizeof(char)*bufferLen);
		memset(buffer,0,sizeof(char)*bufferLen);
		if(buffer==NULL)
		{
			error="malloc wrong";
			return false;
		}
		this->maxLen=bufferLen;
		strcat(this->buffer,"{");
		this->nowLen+=2;
		return true;
	}
	int httpJsonCreate(void* buffer,unsigned int buffLen)//create http top of json
	{
		if(buffLen<this->nowLen+100)
			return -1;
		sprintf((char*)buffer,"HTTP/1.1 200 OK\r\n"
			"Server LCserver/1.1\r\n"
			"Connection: keep-alive\r\n"
			"Content-Type: application/json\r\n"
			"Content-Length: %ld\r\n\r\n"
			"%s",strlen(this->buffer),this->buffer);
		return strlen((char*)buffer);
	}
	void addOBject(const Object& obj)
	{
		switch(obj.type)
		{
		case INT:
			this->addKeyValInt(obj.key,obj.valInt);
			break;
		case FLOAT:
			this->addKeyValFloat(obj.key,obj.valFlo,obj.floOut);
			break;
		case STRING:
			this->addKeyValue(obj.key,obj.valStr);
			break;
		case ARRAY:
			this->addArray(obj.arrTyp,obj.key,obj.array,obj.arrLen,obj.floOut);
			break;
		case OBJ:
		case STRUCT:
			strcat(this->buffer,"\"");
			if(obj.key!=NULL)
				strcat(this->buffer,obj.key);
			strcat(this->buffer,"\":{");
			for(unsigned int i=0;i<obj.arrLen;i++)
				this->addOBject(obj.pobj[0]);
			strcat(this->buffer,"}");
			break;
		}
	}
	bool addKeyValue(const char* key,const char* value)
	{
		char temp[200]={0};
		if(key==NULL||value==NULL)
		{
			error="key or value NULL";
			return false;
		}
		if(nowLen+strlen(key)+strlen(value)>maxLen)
		{
			error="buffer too short";
			return false;
		}
		if(strlen(key)+strlen(value)>=180)
		{
			error="buffer too short";
			return false;
		}
		if(buffer[strlen(buffer)-1]=='}')
			buffer[strlen(buffer)-1]=',';
		int len=sprintf(temp,"\"%s\":\"%s\"}",key,value);
		strcat(this->buffer,temp);
		nowLen+=len;
		return true;
	}
	bool addKeyValInt(const char* key,int value)
	{
		char temp[50]={0};
		if(key==NULL)
		{
			error="key is NULL";
			return false;
		}
		if(nowLen+50>maxLen)
		{
			error="buffer too short";
			return false;
		}
		if(strlen(key)>=45)
		{
			error="buffer too short";
			return false;	
		}
		if(buffer[strlen(buffer)-1]=='}')
			buffer[strlen(buffer)-1]=',';
		int len=sprintf(temp,"\"%s\":%d}",key,value);
		strcat(this->buffer,temp);
		nowLen+=len;
		return true;	
	}
	bool addKeyObj(const char* key,const char* value)
	{
		char temp[1000]={0};
		if(key==NULL||value==NULL)
		{
			error="key or value NULL";
			return false;
		}
		if(nowLen+strlen(key)+strlen(value)>maxLen)
		{
			error="buffer too short";
			return false;
		}
		if(strlen(key)+strlen(value)>=980)
		{
			error="buffer too short";
			return false;
		}
		if(buffer[strlen(buffer)-1]=='}')
			buffer[strlen(buffer)-1]=',';
		int len=sprintf(temp,"\"%s\":%s}",key,value);
		strcat(this->buffer,temp);
		nowLen+=len;
		return true;		
	}
	bool addArray(TypeJson type,const char* key,void** array,unsigned int arrLen,unsigned int floatNum=1)
	{
		char temp[1000]={0};
		int len=0;
		if(array==NULL||arrLen==0)
			return false;
		if(buffer[strlen(buffer)-1]=='}')
			buffer[strlen(buffer)-1]=',';
		sprintf(temp,"\"%s\":[",key);
		strcat(buffer,temp);
		int* arr=(int*)array;
		float* arrF=(float*)array;
		Object* pobj=(Object*)array;
		switch(type)
		{
		case OBJ:
			for(unsigned int i=0;i<arrLen;i++)
			{
				strcat(buffer,"{");
				switch(pobj[i].type)
				{
				case OBJ:
					this->addOBject(pobj[i]);
					break;
				case INT:
					this->addKeyValInt(pobj[i].key,pobj[i].valInt);
					break;
				case STRING:
					this->addKeyValue(pobj[i].key,pobj[i].valStr);
					break;
				case FLOAT:
					this->addKeyValFloat(pobj[i].key,pobj[i].valFlo,pobj[i].floOut);
					break;
				case ARRAY:
					this->addArray(pobj[i].arrTyp,pobj[i].key,pobj[i].array,pobj[i].arrLen,pobj[i].floOut);
					break;
				case STRUCT:
					strcat(buffer,pobj[i].valStr);
					break;
				}
				strcat(buffer,",");
			}
			buffer[strlen(buffer)-1]=']';
			strcat(buffer,"}");
			nowLen+=len;
			break;
		case STRING:
			for(unsigned int i=0;i<arrLen;i++)
			{
				len=sprintf(temp,"\"%s\",",(char*)array[i]);
				strcat(buffer,temp);
			}
			buffer[strlen(buffer)-1]=']';
			strcat(buffer,"}");
			nowLen+=len;
			break;				
		case INT:
			for(unsigned int i=0;i<arrLen;i++)
			{
				len=sprintf(temp,"%d,",arr[i]);
				strcat(buffer,temp);
			}
			buffer[strlen(buffer)-1]=']';
			strcat(buffer,"}");
			nowLen+=len;
			break;
		case FLOAT:
			for(unsigned int i=0;i<arrLen;i++)
			{
				len=sprintf(temp,"%.*f,",floatNum,arrF[i]);
				strcat(buffer,temp);
			}
			buffer[strlen(buffer)-1]=']';
			strcat(buffer,"}");
			nowLen+=len;
			break;
		case STRUCT:
			for(unsigned int i=0;i<arrLen;i++)
			{
				if((char*)array[i]!=NULL)
				{
					strcat(buffer,(char*)array[i]);
					len+=strlen((char*)array[i]);
				}
				strcat(buffer,",");	
			}
			buffer[strlen(buffer)-1]=']';
			strcat(buffer,"}");
			nowLen+=len;
			break;
		default:
			return false;
		}
		return true;
	}
	bool addKeyValFloat(const char* key,float value,int output)
	{
		char temp[70]={0};
		if(nowLen+50>maxLen)
		{
			error="buffer too short";
			return false;
		}
		if(NULL==key)
		{
			error="key is NULL";
			return false;
		}
		if(strlen(key)>=45)
		{
			error="buffer too short";
			return false;
		}
		if(buffer[strlen(buffer)-1]=='}')
			buffer[strlen(buffer)-1]=',';
		int len=sprintf(temp,"\"%s\":%.*f}",key,output,value);
		strcat(this->buffer,temp);
		nowLen+=len;
		return true;		
	}
	void createObject(char* pbuffer,int bufferLen,const Object& obj)
	{
		switch(obj.type)
		{
		case INT:
			this->createObjInt(pbuffer,bufferLen,obj.key,obj.valInt);
			break;
		case FLOAT:
			this->createObjFloat(pbuffer,bufferLen,obj.key,obj.valFlo,obj.floOut);
			break;
		case STRING:
			this->createObjValue(pbuffer,bufferLen,obj.key,obj.valStr);
			break;
		case ARRAY:
			this->createObjArray(pbuffer,bufferLen,obj.arrTyp,obj.key,obj.array,obj.arrLen,obj.floOut);
			break;
		case OBJ:
		case STRUCT:
			strcat(this->buffer,"\"");
			if(obj.key!=NULL)
				strcat(this->buffer,obj.key);
			strcat(this->buffer,"\":{");
			for(unsigned int i=0;i<obj.arrLen;i++)
				this->addOBject(obj.pobj[0]);
			strcat(this->buffer,"}");
			break;
		}	
	}
	int createObjInt(char* pbuffer,unsigned int bufferLen,const char* key,int value)
	{
		if(pbuffer==NULL||key==NULL)
		{
			error="buffer or key NULL";
			return -1;
		}
		if(pbuffer[strlen(pbuffer)-1]=='}')
			pbuffer[strlen(pbuffer)-1]=',';
		if(strlen(pbuffer)==0)
			strcat(pbuffer,"{");
		if(bufferLen<strlen(pbuffer)+strlen(key))
		{
			error="buffer is too short";
			return -1;
		}
		char temp[100]={0};
		int len=sprintf(temp,"\"%s\":%d}",key,value);
		strcat(pbuffer,temp);
		len=strlen(pbuffer);
		return len;
	}
	int createObjFloat(char* pbuffer,unsigned int bufferLen,const char* key,float value,int output=1)
	{
		if(pbuffer==NULL||key==NULL)
		{
			error="buffer or key NULL";
			return -1;
		}
		if(pbuffer[strlen(pbuffer)-1]=='}')
			pbuffer[strlen(pbuffer)-1]=',';
		if(strlen(pbuffer)==0)
			strcat(pbuffer,"{");
		if(bufferLen<strlen(pbuffer)+strlen(key))
		{
			error="buffer is too short";
			return -1;
		}
		char temp[100]={0};
		int len=sprintf(temp,"\"%s\":%.*f}",key,output,value);
		strcat(pbuffer,temp);
		len=strlen(pbuffer);
		return len;
	}
	int createObjValue(char* pbuffer,unsigned int bufferLen,const char* key,const char* value)
	{
		if(pbuffer==NULL||key==NULL||value==NULL)
		{
			error="buffer or key NULL";
			return -1;
		}
		char temp[200]={0};
		if(strlen(pbuffer)+strlen(key)+strlen(value)>bufferLen)
		{
			error="buffer is too short";
			return -1;
		}
		if(strlen(key)+strlen(value)>=180)
		{
			error="buffer is too short";
			return -1;
		}
		if(pbuffer[strlen(pbuffer)-1]=='}')
			pbuffer[strlen(pbuffer)-1]=',';
		if(strlen(pbuffer)==0)
			strcat(pbuffer,"{");
		int len=sprintf(temp,"\"%s\":\"%s\"}",key,value);
		strcat(pbuffer,temp);
		len=strlen(pbuffer);
		return len;
	}
	bool createObjArray(char* pbuffer,unsigned int bufferLen,TypeJson type,const char* key,void** array,unsigned int arrLen,unsigned int floatNum=1)
	{
		char temp[200]={0};
		if(array==NULL||arrLen==0||pbuffer==NULL)
		{
			error="buffer is NULL";
			return false;
		}
		if(strlen(pbuffer)+strlen(key)>bufferLen)
		{
			error="buffer is too short";
			return false;
		}
		if(pbuffer[strlen(pbuffer)-1]=='}')
			pbuffer[strlen(pbuffer)-1]=',';
		if(strlen(pbuffer)==0)
			strcat(pbuffer,"{");
		sprintf(temp,"\"%s\":[",key);
		strcat(pbuffer,temp);
		int* arr=(int*)array;
		float* arrF=(float*)array;
		switch(type)
		{
		case STRING:
			for(unsigned int i=0;i<arrLen;i++)
			{
				sprintf(temp,"\"%s\",",(char*)array[i]);
				strcat(pbuffer,temp);
			}
			pbuffer[strlen(pbuffer)-1]=']';
			strcat(pbuffer,"}");
			break;
		case INT:
			for(unsigned int i=0;i<arrLen;i++)
			{
				sprintf(temp,"%d,",arr[i]);
				strcat(pbuffer,temp);
			}
			pbuffer[strlen(pbuffer)-1]=']';
			strcat(pbuffer,"}");
			break;
		case FLOAT:
			for(unsigned int i=0;i<arrLen;i++)
			{
				sprintf(temp,"%.*f,",floatNum,arrF[i]);
				strcat(pbuffer,temp);
			}
			pbuffer[strlen(pbuffer)-1]=']';
			strcat(pbuffer,"}");
			break;
		default:
			return false;
		}
		return true;
	}
	int createObjObj(char* pbuffer,unsigned int bufferLen,const char* key,const char* value)
	{
		char temp[500]={0};
		if(pbuffer==NULL||key==NULL||value==NULL)
		{
			error="pbuffer NULL or key NULL or value NULL";
			return -1;
		}
		if(strlen(pbuffer)+strlen(key)+strlen(value)>bufferLen)
		{
			error="buffer is too short";
			return -1;
		}
		if(strlen(key)+strlen(value)>=490)
		{
			error="buffer is too short";
			return -1;
		}
		if(pbuffer[strlen(pbuffer)-1]=='}')
			pbuffer[strlen(pbuffer)-1]=',';
		if(strlen(pbuffer)==0)
			strcat(pbuffer,"{");
		int len=sprintf(temp,"\"%s\":%s}",key,value);
		strcat(pbuffer,temp);
		nowLen+=len;
		len=strlen(pbuffer);
		return len;
	}
	inline const char* resultText()
	{
		return buffer;
	}
	bool jsonToFile(const char* fileName)
	{
		FILE* fp=fopen(fileName,"w+");
		if(fp==NULL)
			return false;
		fprintf(fp,"%s",this->buffer);
		fclose(fp);
		return true;
	}
	const char* operator[](const char* key)
	{
		char* temp=NULL;
		if(key==NULL)
			return NULL;
		if((temp=strstr((char*)this->text,key))==NULL)
			return NULL;
		temp=strchr(temp,'\"');
		if(temp==NULL)
			return NULL;
		temp=strchr(temp+1,'\"');
		if(temp==NULL)
			return NULL;
		temp++;
		if(strchr(temp,'\"')-temp>30)
			return NULL;
		memset(this->word,0,sizeof(char)*30);
		for(unsigned int i=0;*temp!='\"';i++,temp++)
			word[i]=*temp;
		return word;
	}
	float getValueFloat(const char* key,bool& flag)
	{
		float value=0;
		if(key==NULL)
			return -1;
		char* temp=strstr((char*)text,key);
		if(temp==NULL)
		{
			flag=false;
			return -1;
		}
		temp=strchr(temp,'\"');
		if(temp==NULL)
		{
			flag=false;
			return -1;
		}
		temp=strchr(temp+1,':');
		if(temp==NULL)
		{
			flag=false;
			return -1;
		}
		if(sscanf(temp+1,"%f",&value)<=0)
		{
			flag=true;
			return -1;
		}
		flag=true;
		return value;
	}
	int getValueInt(const char* key,bool& flag)
	{
		int value=0;
		if(key==NULL)
			return -1;
		char* temp=strstr((char*)text,key);
		if(temp==NULL)
		{
			flag=false;
			return -1;
		}
		temp=strchr(temp,'\"');
		if(temp==NULL)
		{
			flag=false;
			return -1;
		}
		temp=strchr(temp+1,':');
		if(temp==NULL)
		{
			flag=false;
			return -1;
		}
		if(sscanf(temp+1,"%d",&value)<=0)
		{
			flag=true;
			return -1;
		}
		flag=true;
		return value;
	}
	inline const char* lastError()
	{
		return this->error;
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
	int getFileLen(const char* fileName)
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
	bool getFileMsg(const char* fileName,char* buffer,unsigned int bufferLen)
	{
		unsigned int i=0,len=0;
		len=this->getFileLen(fileName);
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
	printf("%s\n",json.text);
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
	return 0;
}
