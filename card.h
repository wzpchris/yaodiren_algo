#ifndef _CARD_H_
#define _CARD_H_

#include <vector>
#include <algorithm>

//数据类型定义
typedef          char        _tint8;     //有符号 1 字节
typedef unsigned char        _uint8;     //无符号 1 字节
typedef short                _tint16;    //有符号 2 字节
typedef unsigned short       _uint16;    //无符号 2 字节
typedef unsigned int         _uint32;    //无符号 4 字节
typedef			 long long   _tint64;    //有符号 8 字节
typedef unsigned long long   _uint64;    //无符号 8 字节

class Card
{
public:
	Card();
	Card(_uint8 val);
	inline bool operator <  (const Card &c) const{ return (face < c.face); };
	inline bool operator >  (const Card &c) const { return (face > c.face); };
	inline bool operator == (const Card &c) const { return (face == c.face); };

	
	static _uint8 ValueToFace(_uint8 val);
	static _uint8 ValueToIndex(_uint8 val);
	static _uint8 IndexToFace(_uint8 index);
	static _uint8 IndexToValue(_uint8 index);
	static _uint8 IndexToSuit(_uint8 index);
	
	
	static int compare(const Card &a, const Card &b);
	static bool lesser_callback(const Card &a, const Card &b);
	static bool greater_callback(const Card &a, const Card &b);
	static void sort_by_ascending(std::vector<Card> &v);
	static void sort_by_descending(std::vector<Card> &v);

public:
	_uint8 face;
	_uint8 suit;
	_uint8 value;
	_uint8 index;
};

#endif /* _CARD_H_ */
