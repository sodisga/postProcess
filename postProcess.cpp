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
		//USES_CONVERSION; //char*תwchar_t*
		wordTableVec.push_back(wordTemp);
	}
	wordTable.close();

	return 0;
}

wstring  kySearch(wstring src, string tableName)
{
	// ��ȡ�ʿ⵽�ڴ�
	vector<wstring> wordTableVec;
	readWordTable(tableName, wordTableVec);
	
	// ��������ƥ��Ľ��
	//return kyMatchNormal(src, wordTableVec);

	// 
	return kyMatchByOne(src, wordTableVec);
}

wstring kyMatchNormal(wstring src, vector<wstring> wordTableVec)
{
	// ���ִʺ�ľ��Ӳ�ֳɵ����Ĵ���
	vector<wstring> srcVec = getWordFromSentence(src, 1);
	if (srcVec.size() == 0)
	{
		return GETKEYWORDFAIL;
	}

	// ƥ��
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
	// �ӷִʺ�ľ�������ȡÿ���ַ�
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

	// ������������еĿո�����ȡ�ؼ���
	if (choice == 1) 
	{
		int i1 = -1, i2; //i1����ָʾ�ַ���ǰ�Ŀո�, i2����ָʾ�ַ�����Ŀո�
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
	
	// ������ȡ�����е�ÿ���ַ�
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

	return wordTableVec[line].substr(n1 + 1, n2 - n1 - 1); //�����ֿ��и��еĵڶ�������
}

vector<int> stringToVecInt(const string &str)
{
	union 
	{
		char c[2];
		int  i;
	} convert;

	// ��λ����
	convert.i = 0;

	vector<int> vec;

	for (unsigned i = 0; i < str.length(); i++) 
	{
		// GBK�������ַ�����0x80
		if ((unsigned)str[i] > 0x80) 
		{
			// ����union����ת����ע���Ǵ����
			convert.c[1] = str[i];
			convert.c[0] = str[i + 1];
			vec.push_back(convert.i);
			i++;
		}
		else
			// С��0x80��ΪASCII���룬һ���ֽ�
			vec.push_back(str[i]);
	}
	return vec;
}
