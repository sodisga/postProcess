#ifndef POSTPROCESS3_H_
#define POSTPROCESS3_H_

#include <string>
#include <map>
#include <vector>
#include <sys/time.h>

#define PHONENUMBER 65		//声韵母总数
#define LISTNUM 50000		//词库中词表的行数
#define BUFFERSIZE 300		//词库中每行所包含的字符数（wchar_t型）
#define SEARCHFAIL (L"Sorry, can not search the keyword!")
#define RESULTBUFNUM 100		//第二步匹配完成后，保存的候选结果数量
#define RESULTNUM 3		    //返回的结果数目


using namespace std;

class CPostProcess2
{
public:
	CPostProcess2(string channelListName, string dictName);
	virtual ~CPostProcess2();

private:
	enum m_allPY{a, ai, an, ang, ao, as, b, c, ch, d, e, ei, en, eng, er, es, f, g, h, i, ia, ian, iang, iao, ib, ie,
		ifs, in, ing, iong, iu, j, k, l, m, n, o, ong, os, ou, p, q, r, s, sh, sil,	t, u, ua, uai, uan, uang, ui, un,
		uo, ux, uxan, uxe, uxn, uxs, w, x, y, z, zh}; //所有的声韵母，为了避免关键字的出现，将韵母if写成了ifs，请注意！
	//wstring m_allPYStr[PHONENUMBER];

	map<wstring, wstring> m_dictMap; //字典的哈希表
	map<wstring, int> m_pinYinMap;   //声韵母的哈希表

	string m_dictName;				 //字典的名字
	string m_channelListName;		 //节目表单的名字

	bool m_bFuzzy;					 //模糊音开关
	int m_fuzzyNum;					 //模糊音节对的数量
	int *m_fuzzyPair1;				 //模糊音节对中的第一个，如z
	int *m_fuzzyPair2;				 //模糊音节对中的第二个，如zh

	wstring *m_dstOriArray; 		 //节目单数组，包含别名的
	wstring *m_dstArray; 			 //节目单数组，不含别名的
	wstring **m_dstPinYin;			 //节目单拼音
	wstring **m_dstInitials; 		 //节目单声母
	wstring **m_dstVowels; 			 //节目单韵母
	int *m_dstLineNO;				 //存储节目单的行号
	int m_dstLines; 				 //节目单拼音的行数
	int *m_dstColumns; 				 //节目单拼音的每行的列数

	int m_srcLength;				 //输入句子的有效长度
	int *m_srcInt;					 //输入句子的int表示形式，声韵母在一起转成了int

	int **m_dstPYFlag; 			 	 //定长的节目单定长的声韵母指示表
	int *m_srcPYFlag;				 //定长的输入句子的定长的声韵母指示表

	int **m_dstInt;					 //节目单的int表示形式，声韵母转成了int
	int **m_dstIntPY;				 //节目单的int表示形式，声韵母在一起转成了int

	vector<int> m_linesKeeped;		 //经模糊匹配后剩下的行
	//int m_bestLinesBuf[RESULTBUFNUM];//经地二步匹配后重新排序的行
	vector<int> m_bestLines;	 	 //最终输出的行
	vector<float> m_scoreBufVec;	 //经过排序以后的分数列表
	vector<wstring> m_finalResult;	 //最终的结果
	map<int, float> m_similarity;	 //相似度
	vector<float> m_finalSimilarity;//最终的结果

public:
	/**
	* 初始化函数，	可得到：
	* （1）字典的哈希表；
	* （2）节目单数组和声韵母指示表；节目单拼音；节目单声母；节目单韵母；节目单拼音的行数；节目单拼音的每行的列数；
	* @return 返回0表示正常退出，返回-1表示异常退出
	**/
	int initial();

	/**
	* 匹配函数：将输入语句与节目单匹配
	* @param src 输入的语句
	* @param similarity 匹配的相似度
	* @param choice 选择匹配的方法：
	* choice = 1 表示通过声韵母的匹配来搜索
	* choice = 2 表示通过拼音的最小编辑距离来匹配
	* @return 返回0表示正常退出，返回-1表示异常退出
	**/
	int adjustedByPinYin(wstring &src, wstring* fuzzyChar, int fuzzyNum, int choice = 1);

	/**
	 * 获取最终的结果
	 * @param finalResult最终的匹配结果
	 * @param similarity 每个匹配结果对应的相似度
	 * @param num，获得结果的数量
	 * @return 返回0表示正常退出，返回-1表示异常退出
	 **/
	int getFinalResult(vector<wstring> &finalResult, vector<float> &similarity, int num=3);

private:
	/**
	* 读取字典至哈希表
	* @return 返回0表示正常退出，返回-1表示异常退出
	**/
	int readDict();

	/**
	* 初始化，读取节目单到wstring类型的数组中
	* @return 返回-1表示读取失败，返回0表示读取成功
	**/
	int readChannelList();

	/**
	 * 读取模糊音的设置
	 * @param fuzzyChar 模糊音对
	 * @param num 模糊音个数，0表示不模糊
	 **/
	void readFuzzyChar(wstring* fuzzyChar, int num);

	/**
	* 通过查表将节目单中的每个汉字分别转为声母和韵母，并得到声韵母指示表
	* @return 返回0表示正常退出，返回-1表示异常退出
	**/
	int channelListToPinYin();

	/**
	* 通过查表将句子中的每个汉字分别转为声母和韵母，并得到声韵母指示表
	* @param Juzi 输入的句子
	* @param initials 通过查表得到的声母
	* @param vowels 通过查表得到的韵母
	* @return 返回0表示正常退出，返回-1表示异常退出
	**/
	int srcToPinYin( wstring juZi, vector<wstring> &pinYin, vector<wstring> &initials, vector<wstring> &vowels);

	/**
	* 通过查表将汉字转为拼音
	* @param Hanzi 输入的汉字（一个字）
	* @param Pinyin 通过查表得到的拼音
	* @return 返回0表示正常退出，返回-1表示没有查找到该字，-2表示没有汉字输入
	**/
	int hanZiToPinYin(wstring Hanzi, wstring &Pinyin);

	/**
	 * 模糊匹配，通过m_dstPYFlag和m_srcPYFlag进行模糊匹配，保留匹配度超过阈值超的
	 * @param threshold 阈值，src与dst匹配的最小值
	 * @return 正常返回0，返回-1表示模糊匹配后全部被筛除
	 **/
	int fuzzyFilter(float threshold = 0.4);

	/**
	 * 根据模糊音设置，对dst进行模糊音处理
	 */
	//void fuzzDst();

	/**
	 * 利用拼音的int形式进行匹配
	 */
	void matchByPYMatch(int *srcInt, int *dstInt, int srcNum, int dstNum, float &score, int &matchedNum);

	/**
	 * 对拼音匹配后的ScoreVec按分数由高到低进行排序，并将分数最高的nBest行的行号
	 * (对应的节目表中的词条不重复)和相似度存到m_bestLines和m_similarity中去
	 */
	int sortPYScoreVec(vector<float> &scoreVec, int nBest);

	/**
	 * 对加模糊音后的拼音匹配后的ScoreVec按分数由高到低进行排序，并将分数最高的nBest行
	 * 的行号(对应的节目表中的词条不重复)和相似度存到m_bestLines和m_similarity中去
	 */
	int sortFuzzyScoreVec(vector<float> &scoreVec, int nBest);

	/**
	* 通过拼音匹配对候选的节目列表进行排序
	* @return 返回0表示正常匹配，返回-1表示匹配失败
	**/
	int sortByPYMatch();

	/**
	 * 对输入的拼音进行模糊
	 * @param srcInt为输入拼音的int形式数组，srcInt[0]为声母部分，srcInt[1]为韵母部分。
	 */
	void fuzzyPinYin(int &smInt, int &ymInt);

	/**
	 * 通过模糊音后的拼音匹配对候选的节目列表进行排序
	 */
	int sortByFuzzyChar();

	/**
	* 基于最小编辑距离的拼音匹配函数
	* 返回str1和str2之间的最小编辑距离
	**/
	int matchByMinEdit(vector<wstring> &srcInitials, vector<wstring> &srcVowels, float &similarity);

	/**
	 * 最小编辑距离
	 * 返回str1和str2之间的最小编辑距离
	 **/
	int minEditDist(wstring &str1, wstring &str2);

	int editDistanceDP(int *X, int *Y, int Xn, int Yn);

	/**
	* 由行号获取节目列表中m_bestLines行中的标准关键词
	**/
	void getStandardKeyword();

};

#endif /* POSTPROCESS3_H_ */
