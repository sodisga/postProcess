#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include <string>
#include <vector>

#define SEARCHFAIL (L"sorry, can not search the keyword!")
#define GETKEYWORDFAIL (L"can not get keyword from input sentence!") 

using namespace std;

/**
* 读取wordTable到wstring类型的vector中
* @param tableName 词表的（路径）名
* @param wordTableVec 存储词表的wstring类型的vector
* @return 返回-1表示读取失败，返回0表示读取成功
**/
int readWordTable(string tableName, vector<wstring> &wordTableVec);

/**
* 用关键词从词表中搜索出相应的词，并返回标准结果
* @param src 输入的语句
* @param tableName 词表的路径名 
* @return 返回的匹配的标准结果
**/
wstring kySearch(wstring src, string tableName);

/**
* 正常的匹配方法：根据输入的分词结果将句子拆分成一个个关键词进行匹配
* @param src 输入的语句
* @param wordTableVec 词库
* @return 返回的匹配后的标准结果
**/
wstring kyMatchNormal(wstring src, vector<wstring> wordTableVec);

/**
* 将输入的句子拆分成一个个的字符单独进行匹配，统计词库中每行
* 匹配上的次数，返回匹配成功次数最多的那一行的标准结果
* @param src 输入的语句
* @param wordTableVec 词库
* @return 返回的匹配后的标准结果
**/
wstring kyMatchByOne(wstring src, vector<wstring> wordTableVec);

/**
* 将以空格位分隔符的句子分离成一个个的词
* @param src 输入的句子
* @param choice 选择关键词提取的方法：
* choice=1：根据输入句子中的空格来提取关键词
* choice=2：单独提取句子中的每个字符
* @return 返回的词
**/
vector<wstring> getWordFromSentence(wstring sentence, int choice);

/**
* 获取wordTableVec词库中第line行的标准关键词
**/
wstring getStandardKeyword(vector<wstring> wordTableVec, int line);

/**
* 对汉字进行转码
* @param src 输入的句子
* @return 返回的词
**/
vector<int> stringToVecInt(const string &str);


#endif