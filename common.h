#ifndef _COMMON_H_
#define _COMMON_H_

#include <vector>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <time.h>
#include <utility>
#include <functional>
#include "card.h"
#include <deque>
#include <set>
#include <iostream>
#include <sstream>

#define MAX_NUM_INDEX 				12 	            //牌值最大索引 (不包括花牌)
#define MAX_INDEX					22				//最大索引

#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

#define ACK_CHI						0x08			//吃
#define ACK_PENG					0x10			//碰
#define ACK_SHA						0x20			//杀
#define ACK_TU						0x40			//吐火

#define CardNum(c) ((c) & MASK_VALUE)
#define CardType(c) (((c) & MASK_COLOR)>>4)
#define CardValue(t,n) ( (t<<4) | (n) )

#define IsRedCard(c)    ((CardType(c) >= 2) ? true : false)			//是否红牌
#define IsBlackCard(c)  ((CardType(c) < 2) ? true : false)

#define MAX_STYLE_LEN  80

template<class T, int SIZE = MAX_STYLE_LEN>
class PhzBuffer
{
private:
	T buffers[SIZE];
	int len;
public:
	PhzBuffer(): len(0)
	{

	}
	void push(const T& other)
	{
		if(len >= SIZE) 
			throw("out of range\n");
		buffers[len++] = other;
	}
	void push(const T *other, int size)
	{
		if (len + size >= sizeof(buffers) / sizeof(T))	
			throw("out of range!\n");
		for (int i = 0; i < size; ++i)		
			push(other[i]);
	}
	T& pop()
	{
		if(len == 0){
			return buffers[0];
			//throw("arr is null\n");
		}
		return buffers[--len];
	}
	int find(const T &other)
	{
		for (int i = 0; i < len; i++)
			if (buffers[i] == other){
				return i;
			}
		return len;
	}
	const T& operator[](int pos) const
	{
		if (pos >= len)		
			throw("out of range!\n");
		return buffers[pos];
	}	
	T& operator[](int pos)
	{
		if (pos >= len)
			throw("out of range!\n");
		return buffers[pos];
	}
	void operator=(const PhzBuffer<T, SIZE>& other)
	{
		len = other.len;
		for (int i = 0; i < len; ++i)
			buffers[i] = other[i];
	}
	bool operator==(const PhzBuffer<T, SIZE>& other)
	{
		if(len != other.len){
			return false;
		}
		return memcmp(buffers, other.buffers, len) == 0;
	}
	void operator+=(const PhzBuffer<T, SIZE>& other)
	{
		int iCount = MIN(int(SIZE - len), other.len);
		for (int i = 0; i < iCount; ++i)
			push(other[i]);
	}
	void insert(const T& other,_uint8 pos)
	{
		push(other);	
		_uint8 tmp = len - 1;
		if (pos > len || pos < 0)
		{
			return ;
		}
		for (int i = tmp; i >= 0; --i){
			if (i > pos){
				buffers[i] = buffers[i - 1];
			}
			if (i == pos)
			{
				buffers[i] = other;
				return ;
			}
		}
	}
	int size() const { return len; }
	bool empty() const{ return len == 0; }
	void clear() { len = 0; }
	int remove(const T& other)
	{
		if (empty()){
			return -1;
		}
		
		int tmp = len - 1;
		for (int i = 0; i < tmp; ++i){
			if (buffers[i] == other){
				memmove(&buffers[i], &buffers[i + 1], (tmp - i) * sizeof(T));
				len--;
				return i;
			}
		}
		if (buffers[tmp] == other){
			pop();
			return len;
		}
		return -1;
	}
	//递增的排序
	void sortByAscend()
	{
		for (int i = 0; i < len; i++){
			for (int j = i + 1; j < len; j++){
				if (buffers[i] > buffers[j]){
					T t = buffers[i];
					buffers[i] = buffers[j];
					buffers[j] = t;
				}
			}
		}		
	}
};

typedef PhzBuffer<_uint8, 6> 			CardsWeave;		//一组牌

struct tagWeaveItem
{
	_uint8							cbWeaveKind;						//组合类型
	_uint8							cbCenterCard;						//中心扑克
	CardsWeave 						cbCardList;
	tagWeaveItem()
	{
		cbWeaveKind = 0; cbCenterCard = 0;
	}
	void clear() {
		cbWeaveKind = 0; cbCenterCard = 0;
		cbCardList.clear();
	}
	void SetWeave(int type, _uint8 centerCard, _uint8 centerCardCnt,  _uint8 *cardChi = NULL, _uint8 cardChiCnt = 0)
	{
		cbCardList.clear();
		cbWeaveKind = type;
		cbCenterCard = centerCard;
		if(type != ACK_CHI){
			for(_uint8 i = 0; i < centerCardCnt && i < 5; ++i){
				cbCardList.push(centerCard);
			}
		}
		else{
			for(_uint8 i = 0; i < cardChiCnt; ++i){
				cbCardList.push(cardChi[i]);
			}
		}
	}
};
typedef PhzBuffer<tagWeaveItem, 11>  		WeaveItemArr;	//已知组合


typedef _uint8 ANALYSE_ITEM[5];
typedef ANALYSE_ITEM ANALYSE_CARDS[MAX_NUM_INDEX];
typedef PhzBuffer<_uint8, 3>  AnalyseWeaveItem;

struct tagAnalyseWeave {
	std::vector<AnalyseWeaveItem> awItem_vec;
	std::vector<_uint8> remain_cards;
	void operator=(const tagAnalyseWeave &anaWeave) {
		awItem_vec.clear();
		std::vector<AnalyseWeaveItem>::const_iterator it = anaWeave.awItem_vec.begin();
		for(; it != anaWeave.awItem_vec.end(); ++it) {
			awItem_vec.push_back(*it);
		}

		remain_cards.clear();
		std::vector<_uint8>::const_iterator iter = anaWeave.remain_cards.begin();
		for(; iter != anaWeave.remain_cards.end(); ++iter) {
			remain_cards.push_back(*iter);
		}
	}
	tagAnalyseWeave() {
		awItem_vec.clear();
		remain_cards.clear();
	}
};

struct tagAnalyseItem {
	int index;
	std::vector<tagAnalyseWeave> weave_vec;
	
	void operator=(const tagAnalyseItem &anaItem) {
		index = anaItem.index;
		weave_vec = anaItem.weave_vec;
	}
	tagAnalyseItem() {
		index = -1;
		weave_vec.clear();
	}
};

struct tagAnaMTItem {
	int mao_cnt;
	int tuo_cnt;
	tagAnaMTItem() {
		mao_cnt = 0;
		tuo_cnt = 0;
	}
	tagAnaMTItem(const tagAnaMTItem &other) {
		mao_cnt = other.mao_cnt;
		tuo_cnt = other.tuo_cnt;
	}
	tagAnaMTItem& operator=(const tagAnaMTItem &other) {
		mao_cnt = other.mao_cnt;
		tuo_cnt = other.tuo_cnt;
		return *this;
	}
	bool operator==(const tagAnaMTItem &other) const {
		return mao_cnt == other.mao_cnt && tuo_cnt == other.tuo_cnt;
	}
	bool operator>(const tagAnaMTItem &other) const {
		if(tuo_cnt > other.tuo_cnt) {
			return true;
		}else {
			if(mao_cnt > other.mao_cnt) {
				return true;
			}
		}

		return false;
	}
};

struct AnaMTCompare {
	bool operator() (const tagAnaMTItem& lhs, const tagAnaMTItem& rhs) const
  	{
		if(lhs > rhs) {
			return true;
		}

		return false;
	}
};

typedef std::set<tagAnaMTItem, AnaMTCompare>	AnaMTSet;
typedef std::set<tagAnaMTItem, AnaMTCompare>::const_iterator	CIT_ANAMTSET;

#endif