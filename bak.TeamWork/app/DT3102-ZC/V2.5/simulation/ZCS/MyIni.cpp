#include "MyIni.h"
#include <string>
#include <windows.h>
#include <utility>
using namespace std;

int MyIni::_bufLen=1024;

MyIni::MyIni(void)
{
}

MyIni::MyIni(char*_iniFilePath)
{
	strcpy_s(iniFilePath,_iniFilePath);
}

MyIni::~MyIni(void)
{
}

//��ȡ���е�section��
void MyIni::getAllSections(vector<string>& vecSections)
{
	vecSections.clear();

	char *buf=new char[_bufLen];
	myGetPrivateProfileSectionNames(&buf,_bufLen);

	/*
	**ͨ��string��ȡ��ÿ��section������
	**GetPrivateProfileSectionNamesȡ����sections��\0���������buf��
	**string����\0����ֹ
	*/
	char*p=buf;
	string strName=p;
	while(strName.length()!=0)
	{
		vecSections.push_back(strName);
		p+=strName.length()+1;
		strName=p;
	}

	delete []buf;
}

//ͨ��section������section�����е�key-value
void MyIni::getKeyValueBySection(const char* sectionName,map<string,string>& kv)
{
	char *buf=new char[_bufLen];
	myGetPrivateProfileString(sectionName,NULL,&buf,_bufLen);

	kv.clear();
	char *p=buf;
	string strKey=p;
	while(strKey.length()!=0)
	{
		kv.insert(make_pair(strKey,getValueString(sectionName,strKey.c_str())));
		p+=strKey.length()+1;
		strKey=p;
	}
	delete []buf;
}

//�ж��Ƿ����ָ����section
bool MyIni::isSectionExists(const char* sectionName)
{
	vector<string> vec;
	getAllSections(vec);
	
	string strSec=sectionName;
	int count=vec.size();
	for(int i=0;i<count;i++)
	{
		if(vec[i]==strSec)
		{
			return true;
		}
	}

	return false;
}
//�ж��Ƿ����ָ����key
bool MyIni::isKeyExists(const char*sectionName,const char*keyName)
{
	bool isExists=false;
	//�����ж�section�Ƿ����
	char *buf=new char[_bufLen];
	myGetPrivateProfileString(sectionName,NULL,&buf,_bufLen);

	char *p=buf;
	string strKey=p;
	string strKeyTemp=keyName;
	while(strKey.length()!=0)
	{
		if(strKey==strKeyTemp)
		{
			isExists=true;
			break;
		}
		p+=strKey.length()+1;
		strKey=p;
	}
	delete []buf;

	return isExists;
}

//private ��getValueString��getKeyValueBySection����
void MyIni::myGetPrivateProfileString(const char*sectionName,const char*keyName,char **buf,int bufLen)
{
	DWORD dwRes=GetPrivateProfileString((LPCWSTR)sectionName,(LPCWSTR)keyName,NULL,(LPWSTR)*buf,bufLen,(LPCWSTR)iniFilePath);
	while(dwRes==bufLen-2)
	{
		delete [](*buf);
		bufLen+=1024;
		*buf=new char[bufLen];
		dwRes=GetPrivateProfileString((LPCWSTR)sectionName,(LPCWSTR)keyName,NULL,(LPWSTR)*buf,bufLen,(LPCWSTR)iniFilePath);
	}
}

//private ��getSectionCount��getAllSections����
void MyIni::myGetPrivateProfileSectionNames(char** buf,int bufLen)
{
	DWORD dwRes=GetPrivateProfileSectionNames((LPWSTR)*buf,bufLen,(LPCWSTR)iniFilePath);
	//��������С,�������û�����
	while(dwRes==bufLen-2)
	{
		delete []buf;
		bufLen+=1024;
		*buf=new char[bufLen];
		dwRes=GetPrivateProfileSectionNames((LPWSTR)*buf,bufLen,(LPCWSTR)iniFilePath);
	}
}
//��ȡsection�ĸ���
int MyIni::getSectionCount()
{
	char *buf=new char[_bufLen];
	myGetPrivateProfileSectionNames(&buf,_bufLen);

	int count=0;
	char *p=buf;
	string str=p;
	while(str.length()!=0)
	{
		count++;
		p+=str.length()+1;
		str=p;
	}
	delete []buf;
	return count;
}
//��ȡ��section��key-value�ĸ���
int MyIni::getKeyValueCount(const char* sectionName)
{
	char *buf=new char[_bufLen];
	myGetPrivateProfileString(sectionName,NULL,&buf,_bufLen);

	int count=0;
	char *p=buf;
	string strKey=p;
	while(strKey.length()!=0)
	{
		count++;
		p+=strKey.length()+1;
		strKey=p;
	}
	delete []buf;
	return count;
}

//д���ַ�ֵ
void MyIni::writeKeyValue(const char*sectionName,const char*keyName,const char*value)
{
	::WritePrivateProfileString((LPCWSTR)sectionName,(LPCWSTR)keyName,(LPCWSTR)value,(LPCWSTR)iniFilePath);
}

//ɾ��section
void MyIni::deleteSection(const char* sectionName)
{
	::WritePrivateProfileString((LPCWSTR)sectionName,NULL,NULL,(LPCWSTR)iniFilePath);
}
//ɾ��section�е�ĳ����
void MyIni::deleteKey(const char* sectionName,const char* keyName)
{
	::WritePrivateProfileString((LPCWSTR)sectionName,(LPCWSTR)keyName,NULL,(LPCWSTR)iniFilePath);
}

//��ȡsection��ĳ��key��ֵ������ȷ��key����
string MyIni::getValueString(const char*sectionName,const char*keyName)
{
	char *buf=new char[_bufLen];
	myGetPrivateProfileString(sectionName,keyName,&buf,_bufLen);
	string str=buf;
	delete []buf;
	return str;
}

//��ȡsection��ĳ��key��ֵ�������section��key�����ڣ�����false
bool MyIni::getValueInt(const char*sectionName,const char*keyName,int& result)
{
	if(this->isKeyExists(sectionName,keyName))
	{
		result=GetPrivateProfileInt((LPCWSTR)sectionName,(LPCWSTR)keyName,-1,(LPCWSTR)iniFilePath);
		return true;
	}
	else
		return false;
}
std::string& replaceInPlace(std::string& str, const std::string::value_type from, const std::string::value_type to, std::string::size_type start)
{
	if (from == to) return str;

	std::string::size_type pos = 0;
	do
	{
		pos = str.find(from, start);
		if (pos != std::string::npos)
		{
			if (to) str[pos] = to;
			else str.erase(pos, 1);
		}
	} while (pos != std::string::npos);

	return str;
}


std::string& removeInPlace(std::string& str, const std::string::value_type ch, std::string::size_type start)
{
	return replaceInPlace(str, ch, 0, start);
}
inline bool isInfiniteImpl(double value)
{
	return _finite(value) == 0;
}
inline bool isNaNImpl(double value)
{
	return _isnan(value) != 0;
}
bool strToDouble(const std::string& str, double& result, char decSep, char thSep)
{
	if (str.empty()) return false;

	//using namespace double_conversion;

	std::string tmp(str);
	removeInPlace(tmp, thSep,0);
	replaceInPlace(tmp, decSep, '.',0);
	removeInPlace(tmp, 'f',0);
	result = strToDouble(tmp.c_str(),result,decSep,thSep);
	return !isInfiniteImpl(result) &&
		!isNaNImpl(result);
}

bool tryParseFloat(const std::string& s, double& value, char decSep, char thSep)
{
	return strToDouble(s.c_str(), value, decSep, thSep);
}
bool MyIni::getValueFloat(const char* sectionName,const char* keyName,float &result)
{
	std::string value = getValueString(sectionName,keyName);
	double tmp;
	if (tryParseFloat(value,tmp,'.',','))
	{
		result = tmp;
		return true;
	}
	return false;
	
}