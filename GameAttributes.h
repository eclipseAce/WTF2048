#ifndef GAMEATTRIBUTES_H_INCLUDED
#define GAMEATTRIBUTES_H_INCLUDED

#include <tchar.h>

// Definition of tile colors
const COLORREF TILE_COLORS[16] = {
	RGB(238,228,218),
	RGB(237,224,200),
	RGB(242,177,121),
	RGB(245,149,99),
	RGB(246,124,95),
	RGB(246,94,59),
	RGB(237,207,114),
	RGB(237,204,97),
	RGB(237,204,97),
	RGB(237,204,97),
	RGB(237,204,97),
	RGB(237,204,97),
	RGB(237,204,97),
	RGB(237,204,97),
	RGB(237,204,97),
	RGB(237,204,97)
};

// Definition of tile texts
const LPCTSTR TILE_TEXTS[16] = {
	_T("2"),
	_T("4"),
	_T("8"),
	_T("16"),
	_T("32"),
	_T("64"),
	_T("128"),
	_T("256"),
	_T("512"),
	_T("1024"),
	_T("2048"),
	_T("4096"),
	_T("8192"),
	_T("16384"),
	_T("32768")
};
/*
const LPCTSTR TILE_TEXTS[16] = {
	_T("H"),
	_T("He"),
	_T("Li"),
	_T("Be"),
	_T("B"),
	_T("C"),
	_T("N"),
	_T("O"),
	_T("F"),
	_T("Ne"),
	_T("Na"),
	_T("Mg"),
	_T("Al"),
	_T("Si"),
	_T("P")
};
*/

// Definition of tile scores
const int TILE_SCORES[16] = { 2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768 };

// Definition of background color
const COLORREF BKGND_COLOR = RGB(187,173,160);

// Definition of cell color
const COLORREF CELL_COLOR = RGB(204,192,179);

// Definition of cell count (X axis)
const int X_ORDER = 5;

// Definition of cell count (Y axis)
const int Y_ORDER = 5;

// Definition of animation timer interval, USER_TIMER_MINIMUM <= value <= USER_TIMER_MAXIMUM
const int ANIM_TIMER_INTERVAL = 10;

#endif // GAMEATTRIBUTES_H_INCLUDED
