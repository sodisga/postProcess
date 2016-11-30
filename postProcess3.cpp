#include "postProcess3.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string.h>

#define MATCHSCORE 8  //匹配上一个字的得分
#define SENTINEL (-1)
#define EDIT_COST (1)


CPostProcess2::CPostProcess2(string channelListName, string dictName)
{
	m_channelListName = channelListName;
	m_dictName = dictName;

	wstring allPYStr[PHONENUMBER] = {L"a", L"ai", L"an", L"ang", L"ao", L"as", L"b", L"c", L"ch", L"d", L"e", L"ei", L"en", L"eng",
				L"er", L"es", L"f", L"g", L"h", L"i", L"ia", L"ian", L"iang", L"iao", L"ib", L"ie", L"if", L"in", L"ing", L"iong",
				L"iu", L"j", L"k", L"l", L"m", L"n", L"o", L"ong", L"os", L"ou", L"p", L"q", L"r", L"s", L"sh", L"sil", L"t", L"u",
				L"ua", L"uai", L"uan", L"uang", L"ui", L"un", L"uo", L"ux", L"uxan", L"uxe", L"uxn", L"uxs", L"w", L"x", L"y", L"z", L"zh"};

	m_bFuzzy = false;
	m_fuzzyPair1 = NULL;
	m_fuzzyPair2 = NULL;

	for(int i=0;i<PHONENUMBER;i++)
	{
		m_pinYinMap[allPYStr[i]] = i;
	}

	m_dstOriArray = new wstring[LISTNUM];

	m_dstArray = new wstring[LISTNUM];

	m_dstPinYin = new wstring* [LISTNUM];
	for(int i=0; i<LISTNUM; i++)
	{
		m_dstPinYin[i] = new wstring[BUFFERSIZE];
	}

	m_dstInitials = new wstring* [LISTNUM];
	for(int i=0; i<LISTNUM; i++)
	{
		m_dstInitials[i] = new wstring[BUFFERSIZE];
	}

	m_dstVowels = new wstring* [LISTNUM];
	for(int i=0; i<LISTNUM; i++)
	{
		m_dstVowels[i] = new wstring[BUFFERSIZE];
	}

	m_dstLineNO = new int[LISTNUM];

	m_dstColumns = new int[LISTNUM];

	m_dstPYFlag = new int* [LISTNUM];
	for(int i=0; i<LISTNUM; i++)
	{
		m_dstPYFlag[i] = new int[PHONENUMBER];
		memset(m_dstPYFlag[i], 0, PHONENUMBER*sizeof(int));
	}

	m_srcPYFlag = new int[PHONENUMBER];
	memset(m_srcPYFlag, 0, PHONENUMBER*sizeof(int));

	m_srcLength = 0;

	m_srcInt = new int[2*BUFFERSIZE];

	m_dstInt = new int* [LISTNUM];
	for(int i=0; i<LISTNUM; i++)
	{
		m_dstInt[i] = new int[2*BUFFERSIZE];
	}

	m_dstIntPY = new int* [LISTNUM];
	for(int i=0; i<LISTNUM; i++)
	{
		m_dstIntPY[i] = new int[2*BUFFERSIZE];
	}
}

CPostProcess2::~CPostProcess2()
{
	if(m_fuzzyPair1 != NULL)
	{
		delete[] m_fuzzyPair1;
	}
	if(m_fuzzyPair1 != NULL)
	{
		delete[] m_fuzzyPair2;
	}

	delete[] m_dstOriArray;
	delete[] m_dstArray;

	delete[] *m_dstPinYin;
	delete[] m_dstPinYin;

	delete[] *m_dstInitials;
	delete[] m_dstInitials;

	delete[] *m_dstVowels;
	delete[] m_dstVowels;

	delete[] m_dstLineNO;

	delete[] m_dstColumns;

	delete[] *m_dstPYFlag;
	delete[] m_dstPYFlag;

	delete[] m_srcPYFlag;

	delete[] m_srcInt;

	delete[] *m_dstInt;
	delete[] m_dstInt;

	delete[] *m_dstIntPY;
	delete[] m_dstIntPY;
}

int CPostProcess2::readDict()
{
	wifstream wordDict;
	wordDict.open(m_dictName.c_str(), ios::in);

	wchar_t wordTemp[BUFFERSIZE];
	wstring key, value;
	wstring strWordDict;

	if (!wordDict)
	{
		wcerr << "Unable to open the dict" << endl;
		return -1;
	}

	int pos;
	while (!wordDict.eof())
	{
		wordDict.getline(wordTemp, BUFFERSIZE);
		strWordDict = wordTemp;
		pos = strWordDict.find_first_of(' ');
		if(-1 != pos)
		{
			key = strWordDict.substr(0,pos); //取出汉字
			value = strWordDict.substr(pos+1, strWordDict.find_first_of('\n')-pos-1); //取出拼音

			m_dictMap[key] = value;
		}
	}
	wordDict.close();

	return 0;
}

int CPostProcess2::readChannelList()
{
	wifstream wordTable;
	wordTable.open(m_channelListName.c_str(), ios::in);
	//wordTable.imbue(locale("zh_CN.UTF-8"));
	wchar_t wordTemp[BUFFERSIZE];
	wstring strTemp;

	if (!wordTable)
	{
		wcerr << "Unable to open word table" << endl;
		return -1;
	}

	int i = 0;
	int	n0, n1, n2 ;
	int realLineNO = 0;
	while (!wordTable.eof())
	{
		//wordTable.read(wordTemp,BUFFERSIZE);
		wordTable.getline(wordTemp, BUFFERSIZE);
		strTemp = wordTemp;

		n2 = strTemp.find('\r');
		if(-1 != n2)
		{
			strTemp = strTemp.substr(0,n2);
		}

		n0 = 0;
		n1 = -1;
		do //考虑到有别名的情况
		{
			n1 = strTemp.find_first_of(',', n0);
			m_dstArray[i] = strTemp.substr(n0, n1-n0);
			n0 = n1 + 1;
			m_dstLineNO[i] = realLineNO;
			i ++;
		}
		while(n1 != -1);

		m_dstOriArray[realLineNO] = strTemp;
		realLineNO ++;

	}
	wordTable.close();
	m_dstLines = i - 1;

	return 0;
}

void CPostProcess2::readFuzzyChar(wstring* fuzzyChar, int num)
{
	if(m_fuzzyPair1 != NULL)
	{
		delete[] m_fuzzyPair1;
		m_fuzzyPair1 = NULL;
	}
	if(m_fuzzyPair2 != NULL)
	{
		delete[] m_fuzzyPair2;
		m_fuzzyPair2 = NULL;
	}

	if(0 == num)
	{
		m_bFuzzy = false;
		return;
	}
	else
	{
		m_fuzzyNum = num;
		m_fuzzyPair1 = new int[num];
		m_fuzzyPair2 = new int[num];
		m_bFuzzy = true;
	}

	wstring temp1;
	int n1;
	for(int i=0; i<num; i++)
	{
		temp1 = fuzzyChar[i];
		n1 = temp1.find(',');

		m_fuzzyPair1[i] = m_pinYinMap[temp1.substr(0,n1)];
		m_fuzzyPair2[i] = m_pinYinMap[temp1.substr(n1+1,-1)];
	}
}

int CPostProcess2::channelListToPinYin()
{
	wstring pinYin, ShengMu, YunMu;
	int n1;

	int i,j;
	int smInt, ymInt; //声母和韵母的int形式
	int pyNum;
	for(i=0; i<m_dstLines; i++)
	{
		pyNum = 0;
		for(j=0; j<m_dstArray[i].size(); j++)
		{
			if(m_dstArray[i][j] != ' ')
			{
				if(-1 == hanZiToPinYin(m_dstArray[i].substr(j,1), pinYin)) //没有查到当前的这个字
				{
					continue;
				}

				n1 = pinYin.find_first_of(' ');

				ShengMu = pinYin.substr(0, n1);
				YunMu = pinYin.substr(n1 + 1, -1);
				m_dstInitials[i][pyNum] = ShengMu;
				m_dstVowels[i][pyNum] = YunMu;

				// 将声韵母转成int
				smInt = m_pinYinMap[ShengMu];
				ymInt = m_pinYinMap[YunMu];
				m_dstInt[i][2*pyNum] =  smInt;
				m_dstInt[i][2*pyNum + 1] = ymInt;
				//m_dstIntPY[i][j] = 100*m_dstInt[i][2*j] + m_dstInt[i][2*j+1];

				// 将拼音指示表相应的位置置1
				m_dstPYFlag[i][smInt] = 1;
				m_dstPYFlag[i][ymInt] = 1;

				pyNum ++;
			}
		}
		m_dstColumns[i] = pyNum;
	}

	return 0;
}

int CPostProcess2::srcToPinYin(wstring juZi, vector<wstring> &pinYin, vector<wstring> &initials, vector<wstring> &vowels)
{
	wstring tempPinyin, ShengMu, YunMu;
	int n1;
	int n2;

	// 去除句子末尾可能出现的<unk>字符
	n2 = juZi.find('<');
	if(-1 != n2)
	{
		juZi = juZi.substr(0,n2);
	}

	int pinYinNum = 0;
	for(size_t i=0;i<juZi.size();i++)
	{
		if(' ' != juZi[i])
		{
			if(-1 == hanZiToPinYin(juZi.substr(i,1), tempPinyin)) //没有查到当前的这个字
			{
				continue;
			}

			// wcout<<Pinyin<<endl;

			n1 = tempPinyin.find_first_of(' ');
			ShengMu = tempPinyin.substr(0,n1);
			YunMu = tempPinyin.substr(n1+1,tempPinyin.find_first_of('\n'));

			initials.push_back(ShengMu);
			vowels.push_back(YunMu);

			//将声韵母转成int形式
			m_srcInt[2*pinYinNum] = m_pinYinMap[ShengMu];
			m_srcInt[2*pinYinNum + 1] = m_pinYinMap[YunMu];

			// 将拼音指示表相应的位置置1
			m_srcPYFlag[m_srcInt[2*pinYinNum]] = 1;
			m_srcPYFlag[m_srcInt[2*pinYinNum + 1]] = 1;

			pinYinNum ++;
		}
	}

	if(0 == pinYinNum)
	{
		wcerr<<"No words can be translated to pin yin!"<<endl;
		return -1;
	}
	else
	{
		m_srcLength = pinYinNum;
	}

	return 0;
}

int CPostProcess2::hanZiToPinYin(wstring Hanzi, wstring &Pinyin)
{
//	if((unsigned)*Hanzi.c_str() <= 0x80) //英文字母
//	{
//		Pinyin = Hanzi + L' ' + Hanzi; //不翻译
//		//wcout<<L"ying wen found"<<endl;
//
//		return 0;
//	}

	if(Hanzi.size() == 0)
	{
		wcerr << L"No words input!"<< endl;
		return -2;
	}

	if (m_dictMap.find(Hanzi) != m_dictMap.end())
	{
		Pinyin = m_dictMap[Hanzi];
	}
	else
	{
		//wcerr << L"Can not find this Chinese word in the dictionary: "<< Hanzi << endl;
		return -1;
	}

	return 0;
}

int CPostProcess2::fuzzyFilter(float threshold)
{
	m_linesKeeped.clear();

	int matchedNum;		 //每行匹配上的数量
	float matchRatio;
	int sumSrc, sumDst;	//src和每行dst中分别中包含的声韵母数
	for(int i=0; i<m_dstLines; i++)
	{
		sumSrc = 0;
		sumDst = 0;
		matchedNum = 0;
		for(int j=0; j<PHONENUMBER; j++)
		{
			sumSrc += m_srcPYFlag[j];
			sumDst += m_dstPYFlag[i][j];
			matchedNum += m_dstPYFlag[i][j] & m_srcPYFlag[j];
		}

		matchRatio = float(matchedNum) / min(sumSrc, sumDst);
		if(matchRatio > threshold)
		{
			m_linesKeeped.push_back(i); //保存行号
		}
	}

	if(0 == m_linesKeeped.size())
	{
		wcerr<<"Fuzzy match got nothing left!"<<endl;
		return -1;
	}

	return 0;
}

void CPostProcess2::fuzzyPinYin(int &smInt, int &ymInt)
{
	for(int k=0; k<m_fuzzyNum; k++)
	{
		if(smInt == m_fuzzyPair2[k]) //声母含模糊音
		{
			if(((smInt == sh) || (smInt == zh) || (smInt == ch)) && (ymInt == ib))
			{
				ymInt = ifs; //翘舌音的ib，对应平舌的if(ifs)
			}
			if(((smInt == s) || (smInt == z) || (smInt == c)) && (ymInt == ifs))
			{
				ymInt = ib; //翘舌音的ib，对应平舌的if(ifs)
			}

			smInt = m_fuzzyPair1[k];
		}
		if(ymInt == m_fuzzyPair2[k]) //韵母含模糊音
		{
			ymInt = m_fuzzyPair1[k];
		}
	}

}

void CPostProcess2::matchByPYMatch(int *srcInt, int *dstInt, int srcNum, int dstNum, float &score, int &matchedNum)
{
	vector<int> lastMatchPosDst; //存储以前各次匹配在dst中的位置
	vector<int> lastMatchPosSrc; //存储以前各次匹配在src中的位置
	vector<int> serialVec1; //dst中各个位置的连续匹配次数
	for(int len1=0; len1<dstNum; len1++) // 次数置1
	{
		serialVec1.push_back(1);
	}
	vector<int> serialVecTemp; //临时存储连续匹配次数的vector

	int serialLen = 1;

	// 匹配
	int k;
	bool bMatched;
	bool bSerial;
	for(size_t j=0; j<srcNum; j++) //对src的每个字
	{
		bMatched = false;
		bSerial = false;

		serialVecTemp.clear();

		for(k=0; k<dstNum; k++) //对节目单中每行的每个字
		{
			//if(srcInitials[j] == m_dstInitials[lineNO][k]) //声母匹配上
			if(srcInt[2*j] == dstInt[2*k]) //声母匹配上
			{
				//if(srcVowels[j] == m_dstVowels[lineNO][k]) //韵母匹配上
				if(srcInt[2*j+1] == dstInt[2*k+1]) //韵母匹配上
				{
					if(lastMatchPosDst.end() != find(lastMatchPosDst.begin(), lastMatchPosDst.end(), k-1)) //如果dst中上一个字被匹配上
					{
						for(int l=0; l<lastMatchPosDst.size(); l++)
						{
							if((k-1 == lastMatchPosDst[l]) && (j-1 == lastMatchPosSrc[l])) //上一次dst中匹配的位置对应的src中匹配的位置正好也是上一个字
							{
								serialVec1[k] = serialVec1[k-1] + 1; //如果是连续匹配，则当前位置的连续值等于前一个位置的值加1
								serialVecTemp.push_back(serialVec1[k]);

								bSerial = true;
								break;
							}
						}

					}

					bMatched = true;

					lastMatchPosDst.push_back(k); //保存匹配的dst位置
					lastMatchPosSrc.push_back(j); //保存匹配的src位置
				}

				//score += 0; //声母匹配上但韵母没有匹配上,得0分
			}
//				else if(m_srcInt[2*j+1] == m_dstInt[lineNO][2*k+1]) //韵母匹配上但声母没有匹配上,得0分
//				{
//					score += 0;
//				}
		}

		if(!bSerial) //没有连续匹配则连续的次数置1
		{
			for(int len2=0; len2<dstNum; len2++)
			{
				serialVec1[len2] = 1;
			}
		}
		else
		{
			serialLen ++;
		}

		if(bMatched) //输入的字没有找到匹配
		{
			if(serialVecTemp.empty())
				serialVecTemp.push_back(1);

			vector<int>::iterator biggest = max_element(serialVecTemp.begin(), serialVecTemp.end());
			int scoreWei = *biggest;

			score = score + MATCHSCORE * scoreWei; //声母、韵母都匹配上,得8分,且连续匹配上额外加分
			matchedNum ++;
		}

		if(serialLen == min(srcNum, dstNum)) //连续匹配的次数已经等于src或dst的长度，则跳到下一行
			break;
	}

	score = score / min(srcNum, dstNum); //距离归一化
}

int CPostProcess2::sortPYScoreVec(vector<float> &scoreVec, int nBest)
{
	m_bestLines.clear();
	m_scoreBufVec.clear();
	m_scoreBufVec = scoreVec;
	sort(m_scoreBufVec.begin(), m_scoreBufVec.end(), greater<float>()); //按分数降序排列

	int vecPosition;
	vector<int> lastVecPosition;
	bool bRepeateDel = true; //防止重复的开关
	bool bRepeated;
	wstring strDstTemp;
	wstring strSrcTemp;
	int realLineNO;
	for(int i=0; i<nBest; i++)
	{
		int minLine = BUFFERSIZE;
		vecPosition = -1;
		if(m_scoreBufVec.size() <= i) //m_scoreBufVec中的元素可能会被删除，导致size减小
			break;

		for(int j=0; j<scoreVec.size(); j++)
		{
			bRepeated = false;
			int lenDist = abs(m_dstColumns[m_linesKeeped[j]] - m_srcLength);

			if((scoreVec[j] == m_scoreBufVec[i]) && (lastVecPosition.end() == find(lastVecPosition.begin(), lastVecPosition.end(), j)) && (lenDist < minLine))
			{
				if(bRepeateDel && (i>0)) //防止节目单中出现重复
				{
					realLineNO = m_dstLineNO[m_linesKeeped[j]];
					wstring temp = m_dstOriArray[realLineNO];
					wstring temp1 = temp.substr(0,temp.find_first_of(','));
					wstring temp2;
					for(int k=0; k<i; k++)
					{
						realLineNO = m_dstLineNO[m_bestLines[k]];
						temp = m_dstOriArray[realLineNO];
						temp2 = temp.substr(0,temp.find_first_of(','));
						if(temp1 == temp2)
						{
							bRepeated = true;
							break;
						}
					}
					if(bRepeated)
					{
						continue;
					}
				}

				vecPosition = j;
				//m_bestLines.push_back(m_linesKeeped[j]);	//行号
				minLine = lenDist;
			}
		}
		if(-1 == vecPosition) //因为此次和上次的字符串一样，而没有匹配上
		{
			vector<float>::iterator it = m_scoreBufVec.begin() + i;
			m_scoreBufVec.erase(it); //删除该行的分数

			i --; //重新进行此次循环
			continue;
		}

		m_bestLines.push_back(m_linesKeeped[vecPosition]);	//行号
		lastVecPosition.push_back(vecPosition);
	}

	return 0;
}

int CPostProcess2::sortByPYMatch()
{
	/*
	int lineNO; //行号
	vector<float> scoreVec;
	float score;
	int serialLen; //连续匹配的次数
	bool bSerial;
	vector<int> serialVec1; //dst中每个位置的连续次数
	vector<int> serialVecTemp; //临时存储连续次数的vector

	bool bMatched; //当前字匹配的标识符
	vector<int> lastMatchPosDst; //上一次dst中匹配上的位置
	vector<int> lastMatchPosSrc; //上一次src中匹配上的位置
	int *matchedNum = new int[m_linesKeeped.size()]; //每一行匹配上的字数
	*/

	m_similarity.clear();
	float score;
	int matchedNum; //每一行匹配上的字数
	vector<float> scoreVec;
	float similarity;
	for(int i=0; i<m_linesKeeped.size(); i++) //剩余节目单中的每一行
	{
		/*lineNO = m_linesKeeped[i];
		score = 0;
		//serialLen = 1;
		matchedNum[i] = 0;
		lastMatchPosDst.clear();
		lastMatchPosSrc.clear();
		serialVec1.clear();
		for(int len1=0; len1<m_dstColumns[lineNO]; len1++) // 次数置1
		{
			serialVec1.push_back(1);
		}
		serialLen = 1;

		// 匹配
		int k;
		for(size_t j=0; j<m_srcLength; j++) //对输入的每个字
		{
			bMatched = false;
			bSerial = false;

			serialVecTemp.clear();

			for(k=0; k<m_dstColumns[lineNO]; k++) //对节目单中每行的每个字
			{
				//if(srcInitials[j] == m_dstInitials[lineNO][k]) //声母匹配上
				if(m_srcInt[2*j] == m_dstInt[lineNO][2*k]) //声母匹配上
				{
					//if(srcVowels[j] == m_dstVowels[lineNO][k]) //韵母匹配上
					if(m_srcInt[2*j+1] == m_dstInt[lineNO][2*k+1]) //韵母匹配上
					{
						if(lastMatchPosDst.end() != find(lastMatchPosDst.begin(), lastMatchPosDst.end(), k-1)) //如果dst中上一个字被匹配上
						{
							for(int l=0; l<lastMatchPosDst.size(); l++)
							{
								if((k-1 == lastMatchPosDst[l]) && (j-1 == lastMatchPosSrc[l])) //上一次dst中匹配的位置对应的src中匹配的位置正好也是上一个字
								{
									serialVec1[k] = serialVec1[k-1] + 1; //如果是连续匹配，则当前位置的连续值等于前一个位置的值加1
									serialVecTemp.push_back(serialVec1[k]);

									bSerial = true;
									break;
								}
							}

						}

						bMatched = true;

						lastMatchPosDst.push_back(k); //保存匹配的dst位置
						lastMatchPosSrc.push_back(j); //保存匹配的src位置
					}

					//score += 0; //声母匹配上但韵母没有匹配上,得0分
				}
//				else if(m_srcInt[2*j+1] == m_dstInt[lineNO][2*k+1]) //韵母匹配上但声母没有匹配上,得0分
//				{
//					score += 0;
//				}
			}

			if(!bSerial) //没有连续匹配则连续的次数置1
			{
				for(int len2=0; len2<m_dstColumns[lineNO]; len2++)
				{
					serialVec1[len2] = 1;
				}
			}
			else
			{
				serialLen ++;
			}

			if(bMatched) //输入的字没有找到匹配
			{
				if(serialVecTemp.empty())
					serialVecTemp.push_back(1);

				vector<int>::iterator biggest = max_element(serialVecTemp.begin(), serialVecTemp.end());
				int scoreWei = *biggest;

				score = score + MATCHSCORE * scoreWei; //声母、韵母都匹配上,得8分,且连续匹配上额外加分
				matchedNum[i] ++;
			}

			if(serialLen == min(m_dstColumns[lineNO], m_srcLength)) //连续匹配的次数已经等于src或dst的长度，则跳到下一行
				break;
		}

		score = score / min(m_dstColumns[lineNO], m_srcLength); //距离归一化
		*/

		int lineNO = m_linesKeeped[i]; //对应m_dst中的行号（不含别名）
		score = 0;
		matchedNum = 0;

		matchByPYMatch(m_srcInt, m_dstInt[lineNO], m_srcLength, m_dstColumns[lineNO], score, matchedNum);

		scoreVec.push_back(score);

		similarity = float(matchedNum) / m_srcLength;
		//m_similarity.push_back(similarity);
		m_similarity[lineNO] = similarity;
	}

	/*
	vector<float> scoreBuffer(scoreVec);
	sort(scoreBuffer.begin(), scoreBuffer.end(), greater<float>()); //按分数降序排列

	if (scoreBuffer[0] == 0) //一个字都没由匹配上
	{
		wcerr<< "Can not match by PinYin" <<endl;

		return -1;
	}

	// 输出多个候选结果
	m_similarity.clear();
	float similarity;
	int vecPosition;
	vector<int> lastVecPosition;
	int minLine[RESULTNUM];
	bool bRepeateDel = true; //防止重复的开关
	bool bRepeated;
	wstring strDstTemp;
	wstring strSrcTemp;
	int resultTemp = m_linesKeeped.size() < RESULTBUFNUM ? m_linesKeeped.size() : RESULTBUFNUM;
	int realLineNO;
	for(int i=0; i<resultTemp; i++)
	{
		minLine[i] = BUFFERSIZE;
		vecPosition = -1;

		for(int j=0; j<scoreVec.size(); j++)
		{
			bRepeated = false;
			int lenDist = abs(m_dstColumns[m_linesKeeped[j]] - m_srcLength);
			if((scoreVec[j] == scoreBuffer[i]) && (lastVecPosition.end() == find(lastVecPosition.begin(), lastVecPosition.end(), j)) && (lenDist < minLine[i]))
			{
				if(bRepeateDel && (i>0)) //防止节目单中出现重复
				{
					realLineNO = m_dstLineNO[m_linesKeeped[j]];
					wstring temp1 = m_dstOriArray[realLineNO];
					wstring temp2;
					for(int k=0; k<i; k++)
					{
						realLineNO = m_dstLineNO[m_bestLines[k]];
						temp2 = m_dstOriArray[realLineNO];
						if(temp1 == temp2)
						{
							bRepeated = true;
							break;
						}
					}
					if(bRepeated)
					{
						continue;
					}
				}

				vecPosition = j;
				m_bestLines[i] = m_linesKeeped[j];	//行号
				minLine[i] = lenDist;
			}
		}
		if(-1 == vecPosition) //因为此次和上次的字符串一样，而没有匹配上
		{
			vector<float>::iterator it = scoreBuffer.begin()+i;
			scoreBuffer.erase(it); //删除该行的分数
			i --; //重新进行此次循环
			continue;
		}

		lastVecPosition.push_back(vecPosition);
		similarity = float(matchedNum[vecPosition]) / m_srcLength;
		m_similarity.push_back(similarity);
	}
	*/

	// 对得分进行由大到小进行排序，并保存前RESULTBUFNUM行至m_bestLines中，排序后的分数保存至m_scoreBufVec
	int resultTemp = scoreVec.size() < RESULTBUFNUM ? scoreVec.size() : RESULTBUFNUM;
	sortPYScoreVec(scoreVec, resultTemp);

	return 0;
}

int CPostProcess2::sortByFuzzyChar()
{
	if(!m_bFuzzy) //没有设置模糊音
		return 0;

	// 对m_bestLines中的dst进行模糊
	int **dstTemp = new int* [m_bestLines.size()];
	for(size_t i=0; i<m_bestLines.size(); i++) //对于bestLines中的每一行
	{
		int lineNO = m_bestLines[i];
		dstTemp[i] = new int[2*m_dstColumns[lineNO]];

		for(int j=0; j<m_dstColumns[lineNO]; j++) //对每个词条（不含别名的）中的每个字
		{
			dstTemp[i][2*j] = m_dstInt[lineNO][2*j];
			dstTemp[i][2*j+1] = m_dstInt[lineNO][2*j+1];
			fuzzyPinYin(dstTemp[i][2*j], dstTemp[i][2*j+1]);
		}
	}

	// 对src进行模糊
	for(int i=0; i<m_srcLength; i++)
	{
		fuzzyPinYin(m_srcInt[2*i], m_srcInt[2*i+1]);
	}

	// 模糊以后的RESULTBUFNUM行重新进行匹配
	float score;
	vector<float> scoreVec;
	for(size_t i=0; i<m_bestLines.size(); i++) //对于bestLines中的每一行
	{
		int lineNO = m_bestLines[i];
		int shorter = min(m_srcLength, m_dstColumns[lineNO]);
		score = 0;
		int matchedNum = 0;

//		if(m_scoreBufVec[i] >= MATCHSCORE*shorter) //完全匹配上
//		{
//			scoreVec.push_back(m_scoreBufVec[lineNO]);
//			continue; //??
//		}
//		else
//		{
			matchByPYMatch(m_srcInt, dstTemp[i], m_srcLength, m_dstColumns[lineNO], score, matchedNum);
			scoreVec.push_back((m_scoreBufVec[i] + score) / 2.0); //原得分与模糊后的得分加权
//		}
	}

	int resultTemp = scoreVec.size() < RESULTNUM ? scoreVec.size() : RESULTNUM;
	sortFuzzyScoreVec(scoreVec, resultTemp);

	delete[] *dstTemp;
	delete[] dstTemp;

	return 0;
}

int CPostProcess2::sortFuzzyScoreVec(vector<float> &scoreVec, int nBest)
{
	//vector<float> scoreBuffer(scoreVec);
	vector<int> bestLinesTemp = m_bestLines;
	m_bestLines.clear();
	m_scoreBufVec.clear();
	m_scoreBufVec = scoreVec;
	sort(m_scoreBufVec.begin(), m_scoreBufVec.end(), greater<float>()); //按分数降序排列

	int vecPosition;
	vector<int> lastVecPosition;
	bool bRepeateDel = false; //防止重复的开关
	bool bRepeated;
	wstring strDstTemp;
	wstring strSrcTemp;
	int realLineNO;

	for(int i=0; i<nBest; i++)
	{
		int minLine = BUFFERSIZE;
		vecPosition = -1;

		for(int j=0; j<scoreVec.size(); j++)
		{
			bRepeated = false;
			int lenDist = abs(m_dstColumns[bestLinesTemp[j]] - m_srcLength);

			if((scoreVec[j] == m_scoreBufVec[i]) && (lastVecPosition.end() == find(lastVecPosition.begin(), lastVecPosition.end(), j)) && (lenDist < minLine))
			{
				if(bRepeateDel && (i>0)) //防止节目单中出现重复
				{
					realLineNO = m_dstLineNO[bestLinesTemp[j]];
					wstring temp = m_dstOriArray[realLineNO];
					wstring temp1 = temp.substr(0,temp.find_first_of(','));
					wstring temp2;
					for(int k=0; k<i; k++)
					{
						realLineNO = m_dstLineNO[m_bestLines[k]];
						temp = m_dstOriArray[realLineNO];
						temp2 = temp.substr(0,temp.find_first_of(','));
						if(temp1 == temp2)
						{
							bRepeated = true;
							break;
						}
					}
					if(bRepeated)
					{
						continue;
					}
				}

				vecPosition = j;
				//m_bestLines.push_back(m_linesKeeped[j]);	//行号
				minLine = lenDist;
			}
		}
		if(-1 == vecPosition) //因为此次和上次的字符串一样，而没有匹配上
		{
			vector<float>::iterator it = m_scoreBufVec.begin() + i;
			m_scoreBufVec.erase(it); //删除该行的分数
			i --; //重新进行此次循环
			continue;
		}

		m_bestLines.push_back(bestLinesTemp[vecPosition]);	//行号
		lastVecPosition.push_back(vecPosition);
	}

	return 0;
}

int CPostProcess2::matchByMinEdit(vector<wstring> &srcInitials, vector<wstring> &srcVowels, float &similarity)
{
	int *srcInt = new int[2*m_srcLength];
	int *srcIntPY = new int[m_srcLength];
	for(int i=0; i<2*m_srcLength; i += 2)
	{
		srcInt[i] =  m_pinYinMap[srcInitials[i/2]];
		srcInt[i+1] = m_pinYinMap[srcVowels[i/2]];
		srcIntPY[i] = srcInt[i]*100 + srcInt[i+1];
	}

	vector<float> dist;
	float disTemp;
	int lineNO;
	for(size_t i=0; i<m_linesKeeped.size(); i++)
	{
		lineNO = m_linesKeeped[i];

		// 声韵母分开计算最小编辑距离
		//disTemp = float(editDistanceDP(srcInt, m_dstInt[lineNO], 2*m_srcLength, 2*m_dstColumns[lineNO])) / min(m_dstColumns[lineNO], m_srcLength);
		// 声韵母一起计算最小编辑距离
		disTemp = float(editDistanceDP(srcIntPY, m_dstIntPY[lineNO], m_srcLength, m_dstColumns[lineNO])) / min(m_dstColumns[lineNO], m_srcLength);
		dist.push_back(disTemp);

	}

	vector<float>::iterator smallest = min_element(dist.begin(), dist.end()); //寻找匹配成功次数最多的行（如果有N个一样的，返回第一个）
	int bestLine = distance(dist.begin(), smallest);

	//similarity =

	delete []srcInt;
	delete []srcIntPY;

	return m_linesKeeped[bestLine];
}

// Returns Minimum among a, b, c
int Minimum(int a, int b, int c)
{
    return min(min(a, b), c);
}

// Strings of size m and n are passed.
// Construct the Table for X[0...m, m+1], Y[0...n, n+1]
int CPostProcess2::editDistanceDP(int *X, int *Y, int Xn, int Yn)
{
    // Cost of alignment
    int cost = 0;
    int leftCell, topCell, cornerCell;

    int m = Xn + 1;
    int n = Yn + 1;

    // T[m][n]
    int *T = (int *)malloc(m * n * sizeof(int));

    // Initialize table
    for(int i = 0; i < m; i++)
        for(int j = 0; j < n; j++)
            *(T + i * n + j) = SENTINEL;

    // Set up base cases
    // T[i][0] = i
    for(int i = 0; i < m; i++)
        *(T + i * n) = i;

    // T[0][j] = j
    for(int j = 0; j < n; j++)
        *(T + j) = j;

    float insWei = 1;	  //插入的权重
    float delWei = 1;	  //删除的权重
    float replaceWei = 2; //替换的权重
    // Build the T in top-down fashion
    for(int i = 1; i < m; i++)
    {
        for(int j = 1; j < n; j++)
        {
            // T[i][j-1]
        	leftCell = *(T + i*n + j-1);
            leftCell += delWei * EDIT_COST; // deletion

            // T[i-1][j]
            topCell = *(T + (i-1)*n + j);
            topCell += insWei * EDIT_COST; // insertion

            // Top-left (corner) cell
            // T[i-1][j-1]
            cornerCell = *(T + (i-1)*n + (j-1) );

            // edit[(i-1), (j-1)] = 0 if X[i] == Y[j], 1 otherwise
            cornerCell += replaceWei*(X[i-1] != Y[j-1]); // may be replace

            // Minimum cost of current cell
            // Fill in the next cell T[i][j]
            *(T + (i)*n + (j)) = Minimum(leftCell, topCell, cornerCell);
        }
    }

    // 结果存储在 T[m][n]
    cost = *(T + m*n - 1);
    free(T);
    return cost;
}

// 递归方法实现
/*int EditDistanceRecursion( char *X, char *Y, int m, int n )
{
    // 基本情况
    if( m == 0 && n == 0 )
        return 0;

    if( m == 0 )
        return n;

    if( n == 0 )
        return m;

    // Recurse
    int left = EditDistanceRecursion(X, Y, m-1, n) + 1;
    int right = EditDistanceRecursion(X, Y, m, n-1) + 1;
    int corner = EditDistanceRecursion(X, Y, m-1, n-1) + (X[m-1] != Y[n-1]);

    return Minimum(left, right, corner);
}*/

int CPostProcess2::minEditDist(wstring &str1, wstring &str2)
{
    int max1 = str1.size();
    int max2 = str2.size();

    int **ptr = new int*[max1 + 1];
    for(int i = 0; i < max1 + 1 ;i++)
    {
        ptr[i] = new int[max2 + 1];
    }

    for(int i = 0 ;i < max1 + 1 ;i++)
    {
        ptr[i][0] = i;
    }

    for(int i = 0 ;i < max2 + 1;i++)
    {
        ptr[0][i] = i;
    }

    for(int i = 1 ;i < max1 + 1 ;i++)
    {
        for(int j = 1 ;j< max2 + 1; j++)
        {
            int d;
            int temp = min(ptr[i-1][j] + 1, ptr[i][j-1] + 1);
            if(str1[i-1] == str2[j-1])
            {
                d = 0 ;
            }
            else
            {
                d = 1 ;
            }
            ptr[i][j] = min(temp, ptr[i-1][j-1] + d);
        }
    }

    int dis = ptr[max1][max2];

    for(int i = 0; i < max1 + 1; i++)
    {
        delete[] ptr[i];
        ptr[i] = NULL;
    }

    delete[] ptr;
    ptr = NULL;

    return dis;
}

void CPostProcess2::getStandardKeyword()
{
	m_finalResult.clear();
	m_finalSimilarity.clear();

	int resultTemp = m_bestLines.size() < RESULTNUM ? m_bestLines.size() : RESULTNUM;
	int n1;
	int realLineNO;
	for(int i=0; i<resultTemp; i++)
	{
		realLineNO = m_dstLineNO[m_bestLines[i]];
		n1 = m_dstOriArray[realLineNO].find_first_of(',');

		if (-1 == n1)
		{
			m_finalResult.push_back(m_dstOriArray[realLineNO]);
		}
		else
		{
			m_finalResult.push_back(m_dstOriArray[realLineNO].substr(0, n1));
		}

		m_finalSimilarity.push_back(m_similarity[m_bestLines[i]]);
	}
}

int CPostProcess2::getFinalResult(vector<wstring> &finalResult, vector<float> &similarity,int num)
{
	int resultTemp = m_finalResult.size() < RESULTNUM ? m_finalResult.size() : RESULTNUM;
	if(num > resultTemp)
	{
		wcerr<<"You can get "<<resultTemp<<" results at most!"<<endl;
		return -1;
	}

	finalResult.clear();
	similarity.clear();
	for(int i=0; i<num; i++)
	{
		finalResult.push_back(m_finalResult[i]);
		similarity.push_back(m_finalSimilarity[i]);
	}

	return 0;
}

int CPostProcess2::initial()
{
	//hash_map<wstring, wstring, str_hash, str_compare> mymap;
	if(-1 == readDict()) //读取字典至哈希表
	{
		return -1;
	}

	if(-1 == readChannelList()) //读取节目单列表
	{
		return -1;
	}

	channelListToPinYin();

	return 0;
}

int CPostProcess2::adjustedByPinYin(wstring &src, wstring* fuzzyChar, int fuzzyNum, int choice)
{
	if(0 == src.size())
	{
		wcerr<<"No words input"<<endl;
		return -1;
	}

	readFuzzyChar(fuzzyChar, fuzzyNum);

	vector<wstring> pinYin;
	vector<wstring> srcInitials; //输入句子的声母
	vector<wstring> srcVowels;
	if(-1 == srcToPinYin(src, pinYin, srcInitials, srcVowels))
	{
		wcerr<< L"Failed while translate input sentence to pin yin"<<endl;
		return -1; //转换后没由拼音输出
	}

	float thres = 0.4;
	if(-1 == fuzzyFilter(thres))
	{
		wcerr<<L"Fuzzy match failed"<<endl;
		return -1; //模糊匹配失败
	}

	if(1 == choice) //基于声韵母的拼音匹配函数
	{
		 if(-1 == sortByPYMatch())
		 {
			 wcerr<<L"Pin Yin match failed"<<endl;
			 return -1;
		 }
	}

	sortByFuzzyChar();

	//一个字都没由匹配上
	if (m_scoreBufVec[0] == 0)
	{
		return -1;
	}

	// 输入小于3个字的处理
	if(m_srcLength <= 3)
	{
		if(m_srcLength < m_dstColumns[m_bestLines[0]])
		{
			if(m_scoreBufVec[0]*m_srcLength < MATCHSCORE*(1+m_srcLength)*m_srcLength/2) //没有完全、连续匹配
			{
				return -1;
			}
		}
		else
		{
			if(m_scoreBufVec[0]*m_dstColumns[m_bestLines[0]] < MATCHSCORE*m_dstColumns[m_bestLines[0]]) //节目单中相应行较短，且没有完全匹配
			{
				return -1;
			}
		}
	}

	getStandardKeyword();

	return 0;
}
