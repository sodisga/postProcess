#include "postProcess.h"
#include <fstream>
#include <iostream>
//#include <atlconv.h>
#include <algorithm>


int readWordTable(string tableName, vector<wstring> &wordTableVec)
{
	wifstream wordTable(tableName);
	wordTable.imbue(locale("chs"));
	wchar_t wordTemp[10240];

	if (!wordTable)
	{
		cerr << "Unable to open word table" << endl;
		return -1;
	}
	while (!wordTable.eof())
	{
		wordTable.getline(wordTemp, 10240);
		//USES_CONVERSION; //char*转wchar_t*
		wordTableVec.push_back(wordTemp);
	}
	wordTable.close();

	return 0;
}

wstring  kySearch(wstring src, string tableName)
{
	// 读取词库到内存
	vector<wstring> wordTableVec;
	readWordTable(tableName, wordTableVec);
	
	// 返回正常匹配的结果
	//return kyMatchNormal(src, wordTableVec);

	// 
	return kyMatchByOne(src, wordTableVec);
}

wstring kyMatchNormal(wstring src, vector<wstring> wordTableVec)
{
	// 将分词后的句子拆分成单个的词语
	vector<wstring> srcVec = getWordFromSentence(src, 1);
	if (srcVec.size() == 0)
	{
		return GETKEYWORDFAIL;
	}

	// 匹配
	for (size_t i = 0; i < srcVec.size(); i++)
	{
		for (size_t j = 0; j < wordTableVec.size(); j++)
		{

			if (wordTableVec[j].find(srcVec[i]) != -1)
			{
				getStandardKeyword(wordTableVec, j);
			}
		}
	}

	return SEARCHFAIL;
}

wstring kyMatchByOne(wstring src, vector<wstring> wordTableVec)
{
	// 从分词后的句子中提取每个字符
	vector<wstring> srcVec = getWordFromSentence(src, 2);

	vector<int> matchTimesVec;
	int matchTimes;
	for (size_t i = 0; i < wordTableVec.size(); i++)
	{
		matchTimes = 0;
		for (size_t j = 0; j < src.size(); j++)
		{
			if (-1 != wordTableVec[i].find(src[j]))
			{
				matchTimes += 1;
			}
		}
		matchTimesVec.push_back(matchTimes);
	}

	vector<int>::iterator biggest = max_element(begin(matchTimesVec), end(matchTimesVec));
	int bestLine = distance(begin(matchTimesVec), biggest);
	
	if (*biggest > 0)
	{
		return getStandardKeyword(wordTableVec, bestLine);
	}

	return SEARCHFAIL;
}

vector<wstring> getWordFromSentence(wstring sentence, int choice)
{
	vector<wstring> result;

	wstring temp;

	// 根据输入句子中的空格来提取关键词
	if (choice == 1) 
	{
		int i1 = -1, i2; //i1用来指示字符串前的空格, i2用来指示字符串后的空格
		for (i2 = 0; i2 < sentence.size() + 1; i2++)
		{
			if ((sentence[i2] == ' ') || (sentence[i2] == '\0'))
			{
				temp = sentence.substr(i1 + 1, i2 - i1 - 1);
				result.push_back(temp);

				i1 = i2;
			}
		}
	}
	
	// 单独提取句子中的每个字符
	if (choice == 2)
	{
		for (size_t i = 0; i < sentence.size(); i++)
		{
			if (sentence[i] != ' ')
			{
				temp = sentence[i];
				result.push_back(temp);
			}
		}
	}

	return result;
}

wstring getStandardKeyword(vector<wstring> wordTableVec, int line)
{
	int n1 = wordTableVec[line].find_first_of(',');
	int n2 = wordTableVec[line].find_first_of(',', n1 + 1);
	if (-1 == n2)
	{
		n2 = wordTableVec[line].find_first_of('\n', n2);
	}

	return wordTableVec[line].substr(n1 + 1, n2 - n1 - 1); //返回字库中该行的第二个词语
}

vector<int> stringToVecInt(const string &str)
{
	union 
	{
		char c[2];
		int  i;
	} convert;

	// 段位清零
	convert.i = 0;

	vector<int> vec;

	for (unsigned i = 0; i < str.length(); i++) 
	{
		// GBK编码首字符大于0x80
		if ((unsigned)str[i] > 0x80) 
		{
			// 利用union进行转化，注意是大端序
			convert.c[1] = str[i];
			convert.c[0] = str[i + 1];
			vec.push_back(convert.i);
			i++;
		}
		else
			// 小于0x80，为ASCII编码，一个字节
			vec.push_back(str[i]);
	}
	return vec;
}
