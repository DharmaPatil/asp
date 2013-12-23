#ifndef MYINI_H
#define MYINI_H

#include <string>
#include <vector>
#include <map>
using namespace std;
class MyIni
{
public:
	MyIni(void);
	MyIni(char *_iniFilePath);
	~MyIni(void);

	//��ȡ���е�section��
	void getAllSections(vector<string>& vecSections);
	//ͨ��section������section�����е�key-value
	void getKeyValueBySection(const char* sectionName,map<string,string>& kv);
	
	//�ж��Ƿ����ָ����section
	bool isSectionExists(const char* sectionName);
	//�ж��Ƿ����ָ����key
	bool isKeyExists(const char* sectionName,const char* keyName);

	//��ȡsection��ĳ��key��ֵ
	string getValueString(const char* sectionName,const char* keyName);
	//��ȡsection��ĳ��key��ֵ�������section��key�����ڣ�����false
	bool getValueInt(const char* sectionName,const char* keyName,int &result);
	bool getValueFloat(const char* sectionName,const char* keyName,float &result);
	//д���ַ�ֵ
	void writeKeyValue(const char* sectionName,const char* keyName,const char* value);
	//void writeFloatValue(const char* sectionName,const char* keyName,const float value);
	//ɾ��section
	void deleteSection(const char* sectionName);
	//ɾ��section�е�ĳ����
	void deleteKey(const char* sectionName,const char* keyName);

	//��ȡsection�ĸ���
	int getSectionCount();
	//��ȡ��section��key-value�ĸ���
	int getKeyValueCount(const char* sectionName);

private:
	//����ini�ļ��ľ���·��
	char iniFilePath[260];
	//���뻺������Ĭ�ϳ���
	static int _bufLen;
	//private ��getValueString��getKeyValueBySection����
	void myGetPrivateProfileString(const char*sectionName,const char*keyName,char **buf,int bufLen);
	//private //private ��getSectionCount��getAllSections����
	void myGetPrivateProfileSectionNames(char** buf,int bufLen);
};

#else
#endif
