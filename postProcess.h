#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include <string>
#include <vector>

#define SEARCHFAIL (L"sorry, can not search the keyword!")
#define GETKEYWORDFAIL (L"can not get keyword from input sentence!") 

using namespace std;

/**
* ��ȡwordTable��wstring���͵�vector��
* @param tableName �ʱ�ģ�·������
* @param wordTableVec �洢�ʱ��wstring���͵�vector
* @return ����-1��ʾ��ȡʧ�ܣ�����0��ʾ��ȡ�ɹ�
**/
int readWordTable(string tableName, vector<wstring> &wordTableVec);

/**
* �ùؼ��ʴӴʱ�����������Ӧ�Ĵʣ������ر�׼���
* @param src ��������
* @param tableName �ʱ��·���� 
* @return ���ص�ƥ��ı�׼���
**/
wstring kySearch(wstring src, string tableName);

/**
* ������ƥ�䷽������������ķִʽ�������Ӳ�ֳ�һ�����ؼ��ʽ���ƥ��
* @param src ��������
* @param wordTableVec �ʿ�
* @return ���ص�ƥ���ı�׼���
**/
wstring kyMatchNormal(wstring src, vector<wstring> wordTableVec);

/**
* ������ľ��Ӳ�ֳ�һ�������ַ���������ƥ�䣬ͳ�ƴʿ���ÿ��
* ƥ���ϵĴ���������ƥ��ɹ�����������һ�еı�׼���
* @param src ��������
* @param wordTableVec �ʿ�
* @return ���ص�ƥ���ı�׼���
**/
wstring kyMatchByOne(wstring src, vector<wstring> wordTableVec);

/**
* ���Կո�λ�ָ����ľ��ӷ����һ�����Ĵ�
* @param src ����ľ���
* @param choice ѡ��ؼ�����ȡ�ķ�����
* choice=1��������������еĿո�����ȡ�ؼ���
* choice=2��������ȡ�����е�ÿ���ַ�
* @return ���صĴ�
**/
vector<wstring> getWordFromSentence(wstring sentence, int choice);

/**
* ��ȡwordTableVec�ʿ��е�line�еı�׼�ؼ���
**/
wstring getStandardKeyword(vector<wstring> wordTableVec, int line);

/**
* �Ժ��ֽ���ת��
* @param src ����ľ���
* @return ���صĴ�
**/
vector<int> stringToVecInt(const string &str);


#endif