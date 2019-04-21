#include "card.h"

//牌值表
static _uint8 IndexToFaceArray[22][2] = {
	{2, 0x22},
	{3, 0x23},
	{4, 0x04}, {4, 0x24},
	{5, 0x05}, {5, 0x25},
	{6, 0x06}, {6, 0x26}, {6, 0x36},
	{7, 0x07}, {7, 0x27}, {7, 0x37},
	{8, 0x08}, {8, 0x18}, {8, 0x28},
	{9, 0x09}, {9, 0x29},
	{10, 0x0a}, {10, 0x2a},
	{11, 0x0b}, 
	{12, 0x2c},
	{14, 0x2e}
};


Card::Card() :
face(0),
suit(0),
value(0),
index(0)
{
}


Card::Card(_uint8 val) {
	value = val;
	face = value & 0xF; 
	suit = value >> 4;
	int tmp = -1;
	switch (face) {
	case 2: tmp = 0; 	break;
	case 3: tmp = 1;	break;
	case 4: {
		if (0 == suit) {
			tmp = 2;
		}
		else if (2 == suit) {
			tmp = 3;
		}
	}break;
	case 5: {
		if (0 == suit) {
			tmp = 4;
		}
		else if (2 == suit) {
			tmp = 5;
		}
	}break;
	case 6: {
		if (0 == suit) {
			tmp = 6;
		}
		else if (2 == suit) {
			tmp = 7;
		}
		else if (3 == suit) {
			tmp = 8;
		}
	}break;
	case 7: {
		if (0 == suit) {
			tmp = 9;
		}
		else if (2 == suit) {
			tmp = 10;
		}
		else if (3 == suit) {
			tmp = 11;
		}
	}break;
	case 8: {
		if (0 == suit) {
			tmp = 12;
		}
		else if (1 == suit) {
			tmp = 13;
		}
		else if (2 == suit) {
			tmp = 14;
		}
	}break;
	case 9: {
		if (0 == suit) {
			tmp = 15;
		}
		else if (2 == suit) {
			tmp = 16;
		}
	}break;
	case 10: {
		if (0 == suit) {
			tmp = 17;
		}
		else if (2 == suit) {
			tmp = 18;
		}
	}break;
	case 11: tmp = 19; break;
	case 12: tmp = 20; break;
	case 14: tmp = 21; break; //花牌
	default:break;
	}
	if (-1 == tmp) {
		suit = 5;
	}
	else {
		index = (_uint8)tmp;
	}
}


_uint8 Card::ValueToFace(_uint8 val) {
	return val & 0xF;
}

_uint8 Card::ValueToIndex(_uint8 val) {
	Card c(val);
	return c.index;
}

_uint8 Card::IndexToFace(_uint8 index) {
	if (index < 22) {
		return IndexToFaceArray[index][0]; 
	}
	return 0;
}

_uint8 Card::IndexToValue(_uint8 index) {
	if (index < 22) {
		return IndexToFaceArray[index][1]; 
	}
	return 0;
}

_uint8 Card::IndexToSuit(_uint8 index) {
	if (index < 22) {
		return (IndexToFaceArray[index][1]) >> 4; 
	}
	return 0;
}

int Card::compare(const Card &a, const Card &b) {
	if (a.face > b.face) {
		return 1;
	}
	else if (a.face < b.face) {
		return -1;
	}
	else if (a.face == b.face) {
		if (a.suit > b.suit) {
			return 1;
		}
		else if (a.suit < b.suit) {
			return -1;
		}	
	}
	return 0;
}

bool Card::lesser_callback(const Card &a, const Card &b) {
	if (Card::compare(a, b) == -1)
		return true;
	else
		return false;
}

bool Card::greater_callback(const Card &a, const Card &b) {
	if (Card::compare(a, b) == 1)
		return true;
	else
		return false;
}

void Card::sort_by_ascending(std::vector<Card> &v) {
	sort(v.begin(), v.end(), Card::lesser_callback);
}

void Card::sort_by_descending(std::vector<Card> &v) {
	sort(v.begin(), v.end(), Card::greater_callback);
}