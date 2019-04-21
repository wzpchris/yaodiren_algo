#include <iostream>
#include <sstream>
#include <sys/time.h>
#include <map>

#include "common.h"
#include "gameLogic.h"

void print_card_index(_uint8 cbCardIndex[MAX_INDEX]) {
    char str[300] = {0};
    int rlen = 0;
    rlen += snprintf(str + rlen, 300 - rlen, "[");
    for(int i = 0; i < MAX_INDEX; ++i) {
        if(cbCardIndex[i] == 0) continue;
        rlen += snprintf(str + rlen, 300 - rlen, "(0x%02x, %d), ", Card::IndexToValue(i), cbCardIndex[i]);
    }
    rlen += snprintf(str + rlen, 300 - rlen, "]");

    std::cout << str << std::endl;
}

int main(int argc, char *argv[])
{
    GameLogic m_GameLogic;
    
    // _uint8 cbCard[] = {
    //     0x22,
	//     0x23, 
	//     0x04, 0x04, 0x24, 0x24,
	//     // 0x05, 0x25,
	//     0x06, 0x26, 0x26, 0x36,
	//     0x08, /* 0x18, */ 0x28,
    //     0x07, 0x07, 0x37,
	//     0x09, 
	//     0x0a, 0x2a, 
	//     0x0b, 
	//     0x2c, 
    // };

    // _uint8 cbCard[] = {
    //     0x22,
	//     0x23, 0x23,
	//     0x05, 0x05, 0x25, 0x25,
	//     0x06, 0x26, 0x26, 
	//     0x08, 0x08, 0x18, 0x28, 0x28,
    //     0x37,
	//     0x29, 
	//     0x0a, 0x0a, 
	//     0x0b,  
    // };

    _uint8 cbCard[] = {
	    // 0x23, 0x23,
        // 0x04, 0x04,
	    // 0x05, 0x25,
	    0x06, 0x06, 0x26, 0x26,
	    0x08, 0x08, 0x18, 0x28,
        // 0x37, 
	    // 0x2a,
	    // 0x0b, 0x0b, 0x0b, 0x0b,
    };
    // _uint8 cbCard[] = {
    //     // 34,36,36,5,37,6,38,7,39,55,24,40,9,9,10,10,42,42,11,44,
    //     34,34,35,4,5,37,6,54,7,39,55,55,8,40,9,41,42,42,11,44,
    // };
    
    _uint8 cbCardIndex[MAX_INDEX] = {0};
    m_GameLogic.SwitchToCardIndex(cbCard, sizeof(cbCard) / sizeof(cbCard[0]), cbCardIndex);

    print_card_index(cbCardIndex);

    CWeaveItemArray fixedWeaveItemArr;
	ANALYSE_CARDS analyse_cards;
	m_GameLogic.ExtraceCard(cbCardIndex, fixedWeaveItemArr, analyse_cards);

    std::cout << "fixedWeaveItemArr :" << std::endl;
    for(int t = 0; t < (int)fixedWeaveItemArr.size(); ++t)
    {
        tagWeaveItem &item = fixedWeaveItemArr[t];
        char str[300] = {0};
        int rlen = 0;
        rlen += snprintf(str + rlen, 300 - rlen, "[");
        for(int i = 0; i < item.cbCardList.size(); ++i) {
            rlen += snprintf(str + rlen, 300 - rlen, "0x%02x, ", item.cbCardList[i]);
        }
        rlen += snprintf(str + rlen, 300 - rlen, "]");

        std::cout << "fixedWeaveItemArr weave item card: t = " << t << ", cards = " << str << std::endl;
    }

    for(int i = 0; i < MAX_NUM_INDEX - 1; ++i) {
        ANALYSE_ITEM &anaItem = analyse_cards[i];
        char str[300] = {0};
        int rlen = 0;
        rlen += snprintf(str + rlen, 300 - rlen, "%02d [", i + 2);
        for(int j = 0; j < 5; ++j) {
            if(anaItem[j] == 0) continue;
            _uint8 card = CardValue(j, i + 2);
            rlen += snprintf(str + rlen, 300 - rlen, "(0x%02x, %d)", card, anaItem[j]);
        }
        rlen += snprintf(str + rlen, 300 - rlen, "]");
        std::cout << "cards = " << str << std::endl;
    }

    struct timeval begin, end;
	gettimeofday(&begin, NULL); 

    std::deque<tagAnalyseItem> anaArr;
	m_GameLogic.GetAllGroupCard(analyse_cards, anaArr);

    gettimeofday(&end, NULL);
	int usetime = 1000000 * (end.tv_sec - begin.tv_sec) + end.tv_usec - begin.tv_usec;
    std::cout << "elapse time : " << usetime << " us" << std::endl;

    std::cout << "==============================" << std::endl;

    for(int t = 0; t < (int)anaArr.size(); ++t) {
        tagAnalyseItem &anaitem = anaArr[t];
        std::cout << "index = " << anaitem.index << ", t = " << t << std::endl;
        std::cout << "weave Arr: " << std::endl;
        for(int i = 0; i < (int)anaitem.weave_vec.size(); ++i) {
            tagAnalyseWeave &weave = anaitem.weave_vec[i];
            char str[600] = {0};
            int rlen = 0;
            for(int j = 0; j < (int)weave.awItem_vec.size(); ++j) {
                AnalyseWeaveItem &item = weave.awItem_vec[j];
                rlen += snprintf(str + rlen, 600 - rlen, "(");
                for(int k = 0; k < item.size(); ++k)
                {
                     rlen += snprintf(str + rlen, 600 - rlen, "0x%02x,", item[k]);
                }
                rlen += snprintf(str + rlen, 600 - rlen, "),");               
            }
            std::cout << "i = " << i << ", cards = " << str;
            
            char remain_str[600] = {0};
            int remain_len = 0;
            for(int i = 0; i < (int)weave.remain_cards.size(); ++i) {
                remain_len += snprintf(remain_str + remain_len, 600 - remain_len, "0x%02x, ", weave.remain_cards[i]);
            }
            std::cout <<"\t\tremain_cards: cards = [" << remain_str << "]" << std::endl; 
        }
    }

	return 0;
}