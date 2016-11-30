#include "postProcess.h"
#include <iostream>
#include <time.h>
#include <windows.h>

using namespace std;

int main()
{
	string tableName = "2015allTVChannels.txt";
	//wstring src(L"�� Ҫ ��C����CTAV5");
	wstring src(L"�� Ҫ ������ƽ��");

	//wstring dst1(L"gl,������㲥��̨�������ֹ㲥");
	//string dst2("gl,������㲥��̨�������ֹ㲥");

	//int n1 = dst1.find(',');
	//int n2 = dst2.find(',');

	//wstring output(L"UNMATCHED!");

	time_t t1 = clock();
	time_t winT1 = GetTickCount();
	wstring output = (L"NULL");
	output = kySearch(src, tableName);
	time_t winT2 = GetTickCount();
	time_t t2 = clock();
	
	wcout.imbue(locale("chs"));
	wcout << output << endl;
	cout << "time cost(ms): " << t2-t1 << endl;
	cout << "windows time cost(ms): " << winT2 - winT1 << endl;

	system("pause");

	return 0;
}