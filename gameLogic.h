#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#include <string>
#include "common.h"

typedef std::vector<tagWeaveItem> CWeaveItemArray;

//游戏逻辑类
class GameLogic
{
public:
	GameLogic(){};
	virtual ~GameLogic(){};
	AnaMTSet anaMtSet[6];
public:
	//扑克转换
	_uint8 SwitchToCardIndex(_uint8 cbCardData[], _uint8 bCardCount, _uint8 cbCardIndex[MAX_INDEX]);
	//计算红点数
	//获得固定的卯数和坨数,返回三张或四张的个数
	void GetFixedWeaveItemMaoAndTuo(CWeaveItemArray &fixedWeaveItemArr, int &mao_count, int &tuo_count);
	//******************获得所有可能组合**************************
	//提取某些牌
	void ExtraceCard(_uint8 handCards[MAX_INDEX], CWeaveItemArray &fixedWeaveItemArr, ANALYSE_CARDS &analyse_cards);
	//获得数量
	int GetAnalyseItemCount(ANALYSE_ITEM small_cards);
	//获得子项
	void GetAnalyseWeave(ANALYSE_ITEM small_cards, ANALYSE_ITEM big_cards, tagAnalyseWeave &aw, tagAnalyseItem &item);
	//获得组合项
	void GetGroupCard(ANALYSE_ITEM &small_cards, ANALYSE_ITEM &big_cards, tagAnalyseItem &item);
	//获得组合牌型
	void GetAllGroupCard(ANALYSE_CARDS &analyse_cards, std::deque<tagAnalyseItem> &anaArr);
	//获得卯数和坨数信息
	void GetAllAnalyseItemMaoTuo(std::deque<tagAnalyseItem> &anaArr);
	//获得包子某项的坨数和卯数
	void GetAnalyseItemMaoAndTuo(AnalyseWeaveItem &item, int &tmp_mao, int &tmp_tuo);
	//找一条路径
	bool FindNoBaoZiOnePath(int index, int &mao, int &tuo, std::stringstream &oss);
	//获得最好的路径
	bool FindBestPath(int &mao, int &tuo);

	//分析包子
	bool AnalyseBaoZi(CWeaveItemArray &fixedWeaveItemArr, ANALYSE_CARDS &analyse_cards);
	//分析牌
	bool AnalyseCard(_uint8 handCards[MAX_INDEX]);
};

#endif
