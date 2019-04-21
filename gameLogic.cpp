#include "gameLogic.h"
#include <map>
#include <math.h>
//扑克数据 
_uint8 const CardArray[21] = {
	0x22,						// 2  红
	0x23,						// 3  丁丁
	0x04, 0x24,					// 4  长二, 和牌	
	0x05, 0x25,					// 5  拐子, 幺四 
	0x06, 0x26, 0x36,			// 6  二红, 长三, 猫猫 
	0x07, 0x27, 0x37, 			// 7  三四, 二五, 高脚
	0x08, 0x18, 0x28,			// 8  平八,	三五, 人牌
	0x09, 0x29,					// 9  黑九, 红九	
	0x0a, 0x2a,					// 10 梅子, 四六
	0x0b, 						// 11 斧头
	0x2c,						// 12 红
};

//扑克转换
_uint8 GameLogic::SwitchToCardIndex(_uint8 cbCardData[], _uint8 bCardCount, _uint8 cbCardIndex[MAX_INDEX])
{
	memset(cbCardIndex, 0, sizeof(_uint8)*MAX_INDEX);
	for (_uint8 i = 0; i < bCardCount; ++i){
		++cbCardIndex[Card::ValueToIndex(cbCardData[i])];
	}
	return bCardCount;
}

void GameLogic::GetFixedWeaveItemMaoAndTuo(CWeaveItemArray &fixedWeaveItemArr, int &mao_count, int &tuo_count)
{
	for(int i = 0; i < (int)fixedWeaveItemArr.size(); ++i) {
		tagWeaveItem &weave_item = fixedWeaveItemArr[i];
		if(weave_item.cbWeaveKind == ACK_CHI) {
			mao_count += 1;
			for(int i = 0; i < weave_item.cbCardList.size(); ++i) {
				if(IsRedCard(weave_item.cbCardList[i])) {
					tuo_count += 1;
				}
			}
		}else if(weave_item.cbWeaveKind == ACK_PENG) {
			mao_count += 1;
			if(IsRedCard(weave_item.cbCenterCard)) {
				tuo_count += 8;
			}else {
				tuo_count += 4;
			}
		}else if(weave_item.cbWeaveKind == ACK_TU) {
			mao_count += 1;
			if(IsRedCard(weave_item.cbCenterCard)) {
				tuo_count += 12;
			}else {
				tuo_count += 6;
			}
		}else if(weave_item.cbCardList.size() == 5) {
			if(IsRedCard(weave_item.cbCenterCard)) {
				tuo_count += 16;
			}else {
				tuo_count += 8;
			}
		}
	}
}


void GameLogic::ExtraceCard(_uint8 handCards[MAX_INDEX], CWeaveItemArray &fixedWeaveItemArr, ANALYSE_CARDS &analyse_cards) {
	fixedWeaveItemArr.clear();
	for(int i = 0; i < MAX_NUM_INDEX; ++i) {
		memset(analyse_cards[i], 0, sizeof(analyse_cards[i]));
	}

	for(_uint8 i = 0; i < MAX_INDEX - 1; ++i) {  //踢出花牌的判断
		if(handCards[i] == 0) continue;
		_uint8 card = Card::IndexToValue(i);
		if(handCards[i] >= 3) {
			_uint8 cbWeaveKind = ACK_PENG;
			if(handCards[i] == 4 || handCards[i] == 5){
				cbWeaveKind = ACK_TU;
			}
			
			tagWeaveItem item;
			item.SetWeave(cbWeaveKind, card, handCards[i]);
			fixedWeaveItemArr.push_back(item);
			continue;
		}

		_uint8 face = Card::IndexToFace(i);
		_uint8 suit = Card::IndexToSuit(i);
		if(handCards[i] == 2) {
			analyse_cards[int(face) - 2][suit] = 2;
		}else if(handCards[i] == 1) {
			analyse_cards[int(face) - 2][suit] = 1;
		}
	}
}

int GameLogic::GetAnalyseItemCount(ANALYSE_ITEM small_cards) {
	int small_cnt = 0;
	for(int i = 0; i < 5; ++i) {
		small_cnt += small_cards[i];
	}

	return small_cnt;
}

void GameLogic::GetAnalyseWeave(ANALYSE_ITEM small_cards, ANALYSE_ITEM big_cards, tagAnalyseWeave &aw, tagAnalyseItem &item) {
	int small_cnt = GetAnalyseItemCount(small_cards);
	int big_cnt = GetAnalyseItemCount(big_cards);
	int small_face = item.index;
	int big_face = 14 - small_face; 

	if(small_cnt == 0 && big_cnt == 0) 
	{
		item.weave_vec.push_back(aw);
		return;
	}
	const _uint8 *pItem = NULL;
	int face = 0;
	if(small_cnt == 0) {
		pItem = big_cards;
		face = big_face;	
	}else if(big_cnt == 0) {
		pItem = small_cards;
		face = small_face;
	}

	if(pItem != NULL) {
		AnalyseWeaveItem seven_awItem;
		for(int i = 4; i >= 0; --i) {
			if(pItem[i] == 0) continue;
			if(pItem[i] == 2) {
				AnalyseWeaveItem awItem;
				awItem.push(CardValue(i, face));
				awItem.push(CardValue(i, face));
				aw.awItem_vec.push_back(awItem);
			}else if(pItem[i] == 1) {
				if(face != 7) {
					aw.remain_cards.push_back(CardValue(i, face));
				}else {
					seven_awItem.push(CardValue(i, face));
					if(seven_awItem.size() == 3) {
						aw.awItem_vec.push_back(seven_awItem);
						seven_awItem.clear();
					}
				}
			}
		}

		if(seven_awItem.size() > 0) {
			if(seven_awItem.size() == 2) {
				aw.awItem_vec.push_back(seven_awItem);
				item.weave_vec.push_back(aw);
			}else {
				aw.remain_cards.push_back(seven_awItem[0]);
				item.weave_vec.push_back(aw);

				aw.remain_cards.clear();
				for(int i = 0; i < (int)aw.awItem_vec.size(); ++i) {
					if(aw.awItem_vec[i].size() == 3) continue;
					if(Card::ValueToFace(aw.awItem_vec[i][0]) != 7) continue;
					aw.awItem_vec[i].push(seven_awItem[0]);
					break;
				}
				item.weave_vec.push_back(aw);
			}
		}else {
			item.weave_vec.push_back(aw);
		}		
		return;
	}

	for(int i = 4; i >= 0; --i) {
		if(small_cards[i] == 0) continue;
		for(int j = 4; j >= 0; --j) {
			if(big_cards[j] == 0) continue;
			if(small_cards[i] == 1 && big_cards[j] == 1) {
				tagAnalyseWeave aw1 = aw;
				AnalyseWeaveItem awItem;
				awItem.push(CardValue(i, small_face));
				awItem.push(CardValue(j, big_face));
				aw1.awItem_vec.push_back(awItem);
				ANALYSE_ITEM tmp_small_cards;
				ANALYSE_ITEM tmp_big_cards;
				memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
				memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
				tmp_small_cards[i]--;
				tmp_big_cards[j]--;
				GetAnalyseWeave(tmp_small_cards, tmp_big_cards, aw1, item);
			}else if(small_cards[i] == 2 && big_cards[j] == 2) {
				tagAnalyseWeave aw1 = aw;
				AnalyseWeaveItem awItem;
				awItem.push(CardValue(i, small_face));
				awItem.push(CardValue(i, small_face));
				aw1.awItem_vec.push_back(awItem);
				ANALYSE_ITEM tmp_small_cards;
				memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
				tmp_small_cards[i] -= 2;
				GetAnalyseWeave(tmp_small_cards, big_cards, aw1, item);

				tagAnalyseWeave aw2 = aw;
				awItem.clear();
				awItem.push(CardValue(j, big_face));
				awItem.push(CardValue(j, big_face));
				aw2.awItem_vec.push_back(awItem);
				ANALYSE_ITEM tmp_big_cards;
				memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
				tmp_big_cards[j] -= 2;
				GetAnalyseWeave(small_cards, tmp_big_cards, aw2, item);

				tagAnalyseWeave aw3 = aw;
				awItem.clear();
				awItem.push(CardValue(i, small_face));
				awItem.push(CardValue(j, big_face));
				aw3.awItem_vec.push_back(awItem);
				memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
				memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
				tmp_small_cards[i]--;
				tmp_big_cards[j]--;
				GetAnalyseWeave(tmp_small_cards, tmp_big_cards, aw3, item);

				tagAnalyseWeave aw4 = aw;
				awItem.clear();
				awItem.push(CardValue(i, small_face));
				awItem.push(CardValue(i, small_face));
				awItem.push(CardValue(j, big_face));
				aw4.awItem_vec.push_back(awItem);
				memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
				memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
				tmp_small_cards[i] -= 2;
				tmp_big_cards[j]--;
				GetAnalyseWeave(tmp_small_cards, tmp_big_cards, aw4, item);

				tagAnalyseWeave aw5 = aw;
				awItem.clear();
				awItem.push(CardValue(i, small_face));
				awItem.push(CardValue(j, big_face));
				awItem.push(CardValue(j, big_face));
				aw5.awItem_vec.push_back(awItem);
				memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
				memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
				tmp_small_cards[i]--;
				tmp_big_cards[j] -= 2;
				GetAnalyseWeave(tmp_small_cards, tmp_big_cards, aw5, item);
			}else {
				tagAnalyseWeave aw1 = aw;
				AnalyseWeaveItem awItem;
				awItem.push(CardValue(i, small_face));
				awItem.push(CardValue(j, big_face));
				aw1.awItem_vec.push_back(awItem);
				ANALYSE_ITEM tmp_small_cards;
				ANALYSE_ITEM tmp_big_cards;
				memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
				memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
				tmp_small_cards[i]--;
				tmp_big_cards[j]--;
				GetAnalyseWeave(tmp_small_cards, tmp_big_cards, aw1, item);

				if(small_cards[i] == 2) {
					tagAnalyseWeave aw1 = aw;
					AnalyseWeaveItem awItem;
					awItem.push(CardValue(i, small_face));
					awItem.push(CardValue(i, small_face));
					aw1.awItem_vec.push_back(awItem);
					memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
					tmp_small_cards[i] -= 2;
					GetAnalyseWeave(tmp_small_cards, big_cards, aw1, item);

					tagAnalyseWeave aw4 = aw;
					awItem.clear();
					awItem.push(CardValue(i, small_face));
					awItem.push(CardValue(i, small_face));
					awItem.push(CardValue(j, big_face));
					aw4.awItem_vec.push_back(awItem);
					memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
					memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
					tmp_small_cards[i] -= 2;
					tmp_big_cards[j]--;
					GetAnalyseWeave(tmp_small_cards, tmp_big_cards, aw4, item);
				}else if(big_cards[j] == 2) {
					tagAnalyseWeave aw2 = aw;
					awItem.clear();
					awItem.push(CardValue(j, big_face));
					awItem.push(CardValue(j, big_face));
					aw2.awItem_vec.push_back(awItem);
					memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
					tmp_big_cards[j] -= 2;
					GetAnalyseWeave(small_cards, tmp_big_cards, aw2, item);

					tagAnalyseWeave aw5 = aw;
					awItem.clear();
					awItem.push(CardValue(i, small_face));
					awItem.push(CardValue(j, big_face));
					awItem.push(CardValue(j, big_face));
					aw5.awItem_vec.push_back(awItem);
					memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
					memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
					tmp_small_cards[i]--;
					tmp_big_cards[j] -= 2;
					GetAnalyseWeave(tmp_small_cards, tmp_big_cards, aw5, item);
				}
			}
		}
	}
}

void GameLogic::GetGroupCard(ANALYSE_ITEM &small_cards, ANALYSE_ITEM &big_cards, tagAnalyseItem &item) {
	int small_cnt = GetAnalyseItemCount(small_cards);
	int big_cnt = GetAnalyseItemCount(big_cards);
	int small_face = item.index;
	int big_face = 14 - small_face; 

	if(small_cnt == 0 && big_cnt == 0) return;
	const _uint8 *pItem = NULL;
	int face = 0;
	if(small_cnt == 0) {
		pItem = big_cards;
		face = big_face;	
	}else if(big_cnt == 0) {
		pItem = small_cards;
		face = small_face;
	}

	if(pItem != NULL) {
		tagAnalyseWeave aw;
		AnalyseWeaveItem seven_awItem;
		for(int i = 4; i >= 0; --i) {
			if(pItem[i] == 0) continue;
			if(pItem[i] == 2) {
				AnalyseWeaveItem awItem;
				awItem.push(CardValue(i, face));
				awItem.push(CardValue(i, face));
				aw.awItem_vec.push_back(awItem);
			}else if(pItem[i] == 1) {
				if(face != 7) {
					aw.remain_cards.push_back(CardValue(i, face));
				}else {
					seven_awItem.push(CardValue(i, face));
					if(seven_awItem.size() == 3) {
						aw.awItem_vec.push_back(seven_awItem);
						seven_awItem.clear();
					}
				}
				
			}
		}

		if(seven_awItem.size() > 0) {
			if(seven_awItem.size() == 2) {
				aw.awItem_vec.push_back(seven_awItem);
			}else {
				aw.remain_cards.push_back(seven_awItem[0]);
			}
		}

		item.weave_vec.push_back(aw);		
		return;
	}

	for(int i = 4; i >= 0; --i) {
		if(small_cards[i] == 0) continue;
		for(int j = 4; j >= 0; --j) {
			if(big_cards[j] == 0) continue;
			if(small_cards[i] == 1 && big_cards[j] == 1) {
				tagAnalyseWeave aw;
				AnalyseWeaveItem awItem;
				awItem.push(CardValue(i, small_face));
				awItem.push(CardValue(j, big_face));
				aw.awItem_vec.push_back(awItem);
				ANALYSE_ITEM tmp_small_cards;
				ANALYSE_ITEM tmp_big_cards;
				memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
				memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
				tmp_small_cards[i]--;
				tmp_big_cards[j]--;
				GetAnalyseWeave(tmp_small_cards, tmp_big_cards, aw, item);
			}else if(small_cards[i] == 2 && big_cards[j] == 2) {
				tagAnalyseWeave aw1;
				AnalyseWeaveItem awItem;
				awItem.push(CardValue(i, small_face));
				awItem.push(CardValue(i, small_face));
				aw1.awItem_vec.push_back(awItem);
				ANALYSE_ITEM tmp_small_cards;
				memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
				tmp_small_cards[i] -= 2;
				GetAnalyseWeave(tmp_small_cards, big_cards, aw1, item);

				tagAnalyseWeave aw2;
				awItem.clear();
				awItem.push(CardValue(j, big_face));
				awItem.push(CardValue(j, big_face));
				aw2.awItem_vec.push_back(awItem);
				ANALYSE_ITEM tmp_big_cards;
				memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
				tmp_big_cards[j] -= 2;
				GetAnalyseWeave(small_cards, tmp_big_cards, aw2, item);

				tagAnalyseWeave aw3;
				awItem.clear();
				awItem.push(CardValue(i, small_face));
				awItem.push(CardValue(j, big_face));
				aw3.awItem_vec.push_back(awItem);
				memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
				memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
				tmp_small_cards[i]--;
				tmp_big_cards[j]--;
				GetAnalyseWeave(tmp_small_cards, tmp_big_cards, aw3, item);

				tagAnalyseWeave aw4;
				awItem.clear();
				awItem.push(CardValue(i, small_face));
				awItem.push(CardValue(i, small_face));
				awItem.push(CardValue(j, big_face));
				aw4.awItem_vec.push_back(awItem);
				memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
				memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
				tmp_small_cards[i] -= 2;
				tmp_big_cards[j]--;
				GetAnalyseWeave(tmp_small_cards, tmp_big_cards, aw4, item);

				tagAnalyseWeave aw5;
				awItem.clear();
				awItem.push(CardValue(i, small_face));
				awItem.push(CardValue(j, big_face));
				awItem.push(CardValue(j, big_face));
				aw5.awItem_vec.push_back(awItem);
				memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
				memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
				tmp_small_cards[i]--;
				tmp_big_cards[j] -= 2;
				GetAnalyseWeave(tmp_small_cards, tmp_big_cards, aw5, item);
			}else {
				tagAnalyseWeave aw;
				AnalyseWeaveItem awItem;
				awItem.push(CardValue(i, small_face));
				awItem.push(CardValue(j, big_face));
				aw.awItem_vec.push_back(awItem);
				ANALYSE_ITEM tmp_small_cards;
				ANALYSE_ITEM tmp_big_cards;
				memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
				memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
				tmp_small_cards[i]--;
				tmp_big_cards[j]--;
				GetAnalyseWeave(tmp_small_cards, tmp_big_cards, aw, item);

				if(small_cards[i] == 2) {
					tagAnalyseWeave aw1;
					AnalyseWeaveItem awItem;
					awItem.push(CardValue(i, small_face));
					awItem.push(CardValue(i, small_face));
					aw1.awItem_vec.push_back(awItem);
					memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
					tmp_small_cards[i] -= 2;
					GetAnalyseWeave(tmp_small_cards, big_cards, aw1, item);

					tagAnalyseWeave aw4;
					awItem.clear();
					awItem.push(CardValue(i, small_face));
					awItem.push(CardValue(i, small_face));
					awItem.push(CardValue(j, big_face));
					aw4.awItem_vec.push_back(awItem);
					memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
					memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
					tmp_small_cards[i] -= 2;
					tmp_big_cards[j]--;
					GetAnalyseWeave(tmp_small_cards, tmp_big_cards, aw4, item);
				}else if(big_cards[j] == 2) {
					tagAnalyseWeave aw2;
					awItem.clear();
					awItem.push(CardValue(j, big_face));
					awItem.push(CardValue(j, big_face));
					aw2.awItem_vec.push_back(awItem);
					memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
					tmp_big_cards[j] -= 2;
					GetAnalyseWeave(small_cards, tmp_big_cards, aw2, item);

					tagAnalyseWeave aw5;
					awItem.clear();
					awItem.push(CardValue(i, small_face));
					awItem.push(CardValue(j, big_face));
					awItem.push(CardValue(j, big_face));
					aw5.awItem_vec.push_back(awItem);
					memcpy(tmp_small_cards, small_cards, sizeof(tmp_small_cards));
					memcpy(tmp_big_cards, big_cards, sizeof(tmp_big_cards));
					tmp_small_cards[i]--;
					tmp_big_cards[j] -= 2;
					GetAnalyseWeave(tmp_small_cards, tmp_big_cards, aw5, item);
				}
			}
		}
	}
}
//获得组合牌型
void GameLogic::GetAllGroupCard(ANALYSE_CARDS &analyse_cards, std::deque<tagAnalyseItem> &anaArr) {
	anaArr.clear();
	for(int i = 0; i <= 5; ++i) {
		tagAnalyseItem item;
		item.index = i + 2;
		if(item.index != 7) {
			GetGroupCard(analyse_cards[i], analyse_cards[10 - i], item);
		}else if(item.index == 7) {
			ANALYSE_ITEM small_cards;
			memset(small_cards, 0, sizeof(small_cards));
			int j = 4;
			for(; j >= 0; --j) {
				if(analyse_cards[5][j] == 0) continue;
				break;
			}
			if(j < 5) {
				small_cards[j] = analyse_cards[5][j];
				ANALYSE_ITEM big_cards;
				memset(big_cards, 0, sizeof(big_cards));
				analyse_cards[5][j] = 0;
				memcpy(big_cards, analyse_cards[5], sizeof(analyse_cards[5]));
				GetGroupCard(small_cards, big_cards, item);
			}
		}
		
		anaArr.push_back(item);
	}
}

//获得卯数和坨数信息
void GameLogic::GetAllAnalyseItemMaoTuo(std::deque<tagAnalyseItem> &anaArr)
{
	for(int i = 0; i < 6; ++i) {
		anaMtSet[i].clear();
	}
	for(int i = 0; i < (int)anaArr.size(); ++i) {
		tagAnalyseItem &anaitem = anaArr[i];
		tagAnaMTItem mtItem;
		anaMtSet[anaitem.index - 2].insert(mtItem);		
		if(anaitem.weave_vec.size() == 0) continue;

		for(int j = 0; j < (int)anaitem.weave_vec.size(); ++j) {
			tagAnalyseWeave &weave = anaitem.weave_vec[j];
			if(weave.awItem_vec.size() == 0) continue;
			int tmp_max_tuo = 0;
			int tmp_max_mao = 0;
			for(int k = 0; k < (int)weave.awItem_vec.size(); ++k) {
				int tmp_mao = 0;
				int tmp_tuo = 0;
				AnalyseWeaveItem &item = weave.awItem_vec[k];
				GetAnalyseItemMaoAndTuo(item, tmp_mao, tmp_tuo);
				if(tmp_mao == 0 && tmp_tuo == 0) continue;
				mtItem.mao_cnt = tmp_mao;
				mtItem.tuo_cnt = tmp_tuo;
				anaMtSet[anaitem.index - 2].insert(mtItem);

				//累加某项的数据
				tmp_max_tuo += tmp_tuo;
				tmp_max_mao += tmp_mao;
				mtItem.mao_cnt = tmp_max_mao;
				mtItem.tuo_cnt = tmp_max_tuo;
				anaMtSet[anaitem.index - 2].insert(mtItem);
			}
		}
	}

	for(int i = 0; i < 6; ++i) {
		AnaMTSet &mtSet = anaMtSet[i];
		CIT_ANAMTSET cit = mtSet.begin();
		char str[500] = {0};
		int slen = 0;
		for(; cit != mtSet.end(); ++cit) {
			slen += snprintf(str + slen, 500 - slen, "(%d, %d)", cit->mao_cnt, cit->tuo_cnt);
		}
		std::cout << i + 2 << str << std::endl;
	}
}
//获得包子某项的坨数和卯数
void GameLogic::GetAnalyseItemMaoAndTuo(AnalyseWeaveItem &item, int &tmp_mao, int &tmp_tuo)
{
	if(item.size() == 2) {
		if(item[0] == item[1]) {
			if(IsRedCard(item[0])) {
				tmp_tuo += 8;
			}else {
				tmp_tuo += 4;
			}
		}else if(item[0] != item[1]) {
			int red_cnt = (IsRedCard(item[0]) ? 1 : 0);
			red_cnt += (IsRedCard(item[1]) ? 1 : 0);
			if(IsRedCard(item[0]) || IsRedCard(item[1])) {
				tmp_mao++;
				tmp_tuo += red_cnt + 1;
			}else {
				tmp_mao++;
				tmp_tuo += red_cnt;
			}
		}
	}else if(item.size() == 3) {
		tmp_mao++;
		for(int t = 0; t < item.size(); ++t) {
			if(IsRedCard(item[t])) {
				tmp_tuo++;
			}
		}
	}
}

//找一条路径
bool GameLogic::FindNoBaoZiOnePath(int index, int &mao, int &tuo, std::stringstream &oss)
{
	if(tuo >= 34 || tuo >= 33) {
		if(mao == 7) {
			return true;
		}
	}

	if(index >= 6) return false;

	AnaMTSet &mtSet = anaMtSet[index];
	std::stringstream oss_tmp;
	CIT_ANAMTSET cit = mtSet.begin();
	for(; cit != mtSet.end(); ++cit) {
		const tagAnaMTItem &item = *cit;
		mao += item.mao_cnt;
		tuo += item.tuo_cnt;
		oss_tmp.clear(); 	//只清空其状态
		oss_tmp.str("");	//清空其内容
		oss_tmp << "(" << index + 2 << ":" << item.mao_cnt << "," << item.tuo_cnt  << ")";
		bool ret = FindNoBaoZiOnePath(index + 1, mao, tuo, oss);
		if(ret) {
			oss << oss_tmp.str();
			return true;
		}else {
			mao -= item.mao_cnt;
			tuo -= item.tuo_cnt;
		}
	}

	return false;
}
//获得最好的路径
bool GameLogic::FindBestPath(int &mao, int &tuo)
{
	std::stringstream oss;
	bool ret = FindNoBaoZiOnePath(0, mao, tuo, oss);
	return ret;
}

bool GameLogic::AnalyseBaoZi(CWeaveItemArray &fixedWeaveItemArr, ANALYSE_CARDS &analyse_cards)
{
	int mao_count = 0;
	int tuo_count = 0;
	GetFixedWeaveItemMaoAndTuo(fixedWeaveItemArr,  mao_count, tuo_count);

	//获得剩余牌的组合
	std::deque<tagAnalyseItem> anaArr;
	GetAllGroupCard(analyse_cards, anaArr);

	//获得所有组合的卯数和坨数信息
	GetAllAnalyseItemMaoTuo(anaArr);

	bool ret = FindBestPath(mao_count, tuo_count);
	return ret;
}

bool GameLogic::AnalyseCard(_uint8 handCards[MAX_INDEX])
{
	CWeaveItemArray fixedWeaveItemArr;
	ANALYSE_CARDS analyse_cards;
	ExtraceCard(handCards, fixedWeaveItemArr, analyse_cards);

	bool ret = AnalyseBaoZi(fixedWeaveItemArr, analyse_cards);
	return ret;
}