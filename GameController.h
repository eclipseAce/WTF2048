#ifndef GAMECONTROLLER_H_INCLUDED
#define GAMECONTROLLER_H_INCLUDED

#include <list>
#include "Tile.h"

class GameController {
public:
	enum {
		SWIPE_LEFT = 0,
		SWIPE_UP,
		SWIPE_RIGHT,
		SWIPE_DOWN
	};
private:
	int _nXorder;
	int _nYorder;
	Tile ***_Cell;
	RECT **_Grid;

	HWND _hWnd;
	HDC _hDC;
	HBITMAP _hBitmap;
	HBITMAP _hOldBitmap;
	BITMAPINFO _BitmapInfo;
	BYTE *_pBkgndCache;

	std::list<Tile*> _TransformingTile;

	int _nBest;
	int _nWorst;
	int _nScore;
	RECT _rectBest;
	RECT _rectWorst;
	RECT _rectScore;
	int _nFlowScoreOffset;
	int _nFlowScoreValue;

	const RECT &RectAt(std::pair<int,int> &coord);
	void SetCellTile(std::pair<int,int> &coord,Tile *pTile);
	Tile *GetCellTile(std::pair<int,int> &coord);
	void AddRandomTile();
	void MoveTile(std::pair<int,int> &coordFrom,std::pair<int,int> &coordTo);
	void MergeTile(std::pair<int,int> &coord,std::pair<int,int> &coordFrom);
	void PaintText(LPRECT lpRect,LPCTSTR lpszText,LPCTSTR lpszFaceName,int nSize,int nWeight,COLORREF crText);
	void PaintSolidRoundRect(LPRECT lpRect,COLORREF crColor,int nRadius);
	void PaintSolidRect(LPRECT lpRect,COLORREF crColor);
	void PaintTile(Tile *pTile);

public:
	void Initial(HWND hWnd);
	void Destroy();
	void OutputUI(HDC hDC);
	void StartGame();
	void NextFrame();
	void Swipe(int direction);
	BOOL IsGameOver();
};

#endif // GAMECONTROLLER_H_INCLUDED
