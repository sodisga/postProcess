#include "postProcess3.h"
#include <iostream>
#include <fstream>

#define TESTALL
//#define TESTONE

#ifdef TESTALL
int main()
{
	locale lc("zh_CN.UTF-8");
	locale::global(lc);
	//wcout.imbue(locale("zh_CN.UTF-8"));

	struct timeval start;
	gettimeofday( &start, NULL );

	string channelListName = "channelList20161118.txt";
	//string channelListName = "testChannel";
	string ditcName = "chinese2syl_sphnix_utf8_addletter.dict";

	CPostProcess2 postProcess(channelListName, ditcName);

	if(-1 == postProcess.initial())
	{
		wcerr << "Initialize failed" << endl;
		return -1;
	}

	struct timeval initial;
	gettimeofday( &initial, NULL );
	unsigned long timer = 1000000 * (initial.tv_sec-start.tv_sec)+ initial.tv_usec-start.tv_usec;
	wcout << L"initial time(ms): " << timer / 1000.0 << endl;

	wifstream testCases;
	testCases.open("testCases", ios::in);
	if (!testCases)
	{
		cerr << "Unable to open the test file" << endl;
		return -1;
	}

	wstring fuzzyChar[] = {L"s,sh",L"z,zh",L"c,ch",L"l,n",L"f,h",L"l,r",L"an,ang",L"en,eng",L"in,ing",L"ian,iang",L"uan,uang",L"q,j"};
	int fuzzyNum = 12;
	vector<wstring> output; //最终结果
	vector<float> similarity; //相似度
	int num=3; //输出结果的数量
	wchar_t srcTemp[BUFFERSIZE];
	wstring src;
	wofstream outStream;
	outStream.open("testResult3", ios::out);
	while (!testCases.eof())
	{
		testCases.getline(srcTemp, BUFFERSIZE);
		src = srcTemp;

		if(-1 == postProcess.adjustedByPinYin(src, fuzzyChar, fuzzyNum))
		{
			outStream<<src<<L":   "<<endl;
			continue;
		}


		// 输出最终结果
		postProcess.getFinalResult(output, similarity, num);

		outStream<<src<<L":   ";
		for(int i=0; i<num; i++)
		{
			outStream<<output[i]<<" ";
		}
		outStream<<endl;
	}
	outStream.close();
	testCases.close();

	struct timeval end;
	gettimeofday( &end, NULL );

	timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
	wcout << L"time cost totaly(ms): " << timer / 1000.0 << endl;

	return 0;
}
#endif

#ifdef TESTONE
int main()
{
	locale lc("zh_CN.UTF-8");
	locale::global(lc);
	//wcout.imbue(locale("zh_CN.UTF-8"));

	struct timeval start;
	gettimeofday( &start, NULL );

	string channelListName = "channelList20161118.txt";
	//string channelListName = "testChannel";
	string ditcName = "chinese2syl_sphnix_utf8_addletter.dict";
	//wstring src(L"中央电视台啼与频道");
	//wstring src(L"我 要看 重庆ho回龙观电视台国际频道");
	wstring src(L"cctv5");
	//wstring src(L"我 要 看cctvk5");
	//wstring src(L"我 要 看体育平道");

	wcout << src << endl;

//	wstring wordTableVec[LISTNUM];
//	int listNum = 0;
//	readWordTable(tableName, wordTableVec, listNum);
//	output = kyMatchByOne(src6, wordTableVec, listNum);

	CPostProcess2 postProcess(channelListName, ditcName);

	if(-1 == postProcess.initial())
	{
		wcerr << "Initialize failed" << endl;
		return -1;
	}

	struct timeval initial;
	gettimeofday( &initial, NULL );
	unsigned long timer = 1000000 * (initial.tv_sec-start.tv_sec)+ initial.tv_usec-start.tv_usec;
	wcout << L"initial time(ms): " << timer / 1000.0 << endl;

	//wstring fuzzyChar[] = {L"f,h"};
	wstring fuzzyChar[] = {L"s,sh",L"z,zh",L"c,ch",L"l,n",L"f,h",L"l,r",L"an,ang",L"en,eng",L"in,ing",L"ian,iang",L"uan,uang",L"q,j"};
	int fuzzyNum = 12; //12;
	vector<wstring> output; //最终结果
	vector<float> similarity; //相似度
	int num = 3; //输出结果的数量
	if(-1 == postProcess.adjustedByPinYin(src, fuzzyChar, fuzzyNum))
	{
		wcerr << "Adjusted By Pin Yin Failed"<<endl;
		return -1;
	}

	// 输出最终结果
	if(-1 ==postProcess.getFinalResult(output, similarity, num))
	{
		wcerr << "Get final result failed"<<endl;
		return -1;
	}

	struct timeval end;
	gettimeofday( &end, NULL );

	timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
	wcout << L"time cost totaly(ms): " << timer / 1000.0 << endl;

	//wcout.imbue(locale("chs"));
	for(int i=0; i<num; i++)
	{
		wcout << output[i] << endl;
		wcout <<"similarity: "<<similarity[i]<<endl;
	}

	return 0;
}
#endif



