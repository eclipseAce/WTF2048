#include <assert.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <time.h>
#include "GameController.h"
#include "GameAttributes.h"

void GameController::PaintTile(Tile *pTile) {
	RECT rect;
	pTile->GetCurrentFrame(rect);
	int nTextLen = _tcslen(TILE_TEXTS[pTile->GetLevel()]);
	int nFontSize = (rect.right - rect.left) * 90 / 100 / (nTextLen > 1 ? nTextLen : 2);
	COLORREF crColor = pTile->GetLevel() < 2 ? RGB(119,110,101) : RGB(255,255,255);
	PaintSolidRoundRect(&rect,TILE_COLORS[pTile->GetLevel()],8);
	PaintText(&rect,TILE_TEXTS[pTile->GetLevel()],_T("Arial"),nFontSize,FW_BOLD,crColor);
}
void GameController::PaintText(LPRECT lpRect,LPCTSTR lpszText,LPCTSTR lpszFaceName,int nSize,int nWeight,COLORREF crText) {
	// Create LOGFONT structure
	LOGFONT lf;
	::ZeroMemory(&lf,sizeof(LOGFONT));
	_tcscpy(lf.lfFaceName,lpszFaceName);
	lf.lfHeight = -nSize;
	lf.lfWeight = nWeight;

	// Draw text
	HFONT hOldFont = (HFONT)::SelectObject(_hDC,::CreateFontIndirect(&lf));
	::SetTextColor(_hDC,crText);
	::DrawText(_hDC,lpszText,_tcslen(lpszText),lpRect,DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	::DeleteObject(::SelectObject(_hDC,hOldFont));
}
void GameController::PaintSolidRoundRect(LPRECT lpRect,COLORREF crColor,int nRadius) {
	::BeginPath(_hDC);
	::RoundRect(_hDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom,nRadius,nRadius);
	::EndPath(_hDC);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(_hDC,::CreateSolidBrush(crColor));
	::FillPath(_hDC);
	::DeleteObject(::SelectObject(_hDC,hOldBrush));
}
void GameController::PaintSolidRect(LPRECT lpRect,COLORREF crColor) {
	HBRUSH hBrush = ::CreateSolidBrush(crColor);
	::FillRect(_hDC,lpRect,hBrush);
	::DeleteObject(hBrush);
}

void GameController::Initial(HWND hWnd) {
	// Assert for debug
	assert(this->_hWnd == NULL);

	_hWnd = hWnd;
	_nXorder = X_ORDER;
	_nYorder = Y_ORDER;

	// Set initial score
	_nBest = 0;
	_nWorst = 0x7FFFFFFF;
	_nScore = 0;
	_nFlowScoreOffset = 20;

	// Get size of client area
	RECT rectViewport;
	::GetClientRect(_hWnd,&rectViewport);
	::OffsetRect(&rectViewport,rectViewport.left,rectViewport.top);
	int cxViewport = rectViewport.right;
	int cyViewport = rectViewport.bottom;

	// Create memory bitmap
	HDC hWndDC = ::GetDC(_hWnd);
	_hDC = ::CreateCompatibleDC(hWndDC);
	_hBitmap = ::CreateCompatibleBitmap(hWndDC,cxViewport,cyViewport);
	_hOldBitmap = (HBITMAP)::SelectObject(_hDC,_hBitmap);
	::ReleaseDC(_hWnd,hWndDC);

	// Set for drawing text
	::SetBkMode(_hDC,TRANSPARENT);

	// Erase bitmap
	PaintSolidRect(&rectViewport,RGB(255,255,255));

	// Calculate and draw grid rects
	int nCellSize = (cxViewport - 10) / _nXorder - 10;
	RECT rectGrid;
	::SetRect(&rectGrid,0,cyViewport - (nCellSize + 10) * _nYorder - 10,cxViewport,cyViewport);
	PaintSolidRoundRect(&rectGrid,BKGND_COLOR,10);
	_Grid = new RECT*[_nXorder];
	_Cell = new Tile**[_nXorder];
	for (int i = 0,x = 10; i < _nXorder; i++,x += (nCellSize + 10)) {
		_Grid[i] = new RECT[_nYorder];
		_Cell[i] = new Tile*[_nYorder];
		for (int j = 0,y = rectGrid.top + 10; j < _nYorder; j++,y += (nCellSize + 10)) {
			::SetRect(&_Grid[i][j],x,y,x + nCellSize,y + nCellSize);
			_Cell[i][j] = NULL;
			PaintSolidRoundRect(&_Grid[i][j],CELL_COLOR,8);
		}
	}
	// Draw title text
	RECT rectText;
	::SetRect(&rectText,0,0,cxViewport,80);
	PaintText(&rectText,_T("WTF2048"),_T("Arial"),60,FW_BOLD,RGB(119,110,101));

	::SetRect(&rectText,0,80,cxViewport,100);
	PaintText(&rectText,_T("Arrow keys to swipe tiles, ESC for a new game."),_T("Arial"),12,FW_BOLD,RGB(119,110,101));

	// Calculate and draw score bar
	int cxScoreBar = 100;
	int cyScoreBar = 50;
	int nScoreBarLeftBound = (cxViewport - ((cxScoreBar + 10) * 3 - 10)) / 2;
	int nScoreBarUpBound = rectGrid.top - 10 - cyScoreBar;
	::SetRect(&_rectWorst,nScoreBarLeftBound,nScoreBarUpBound,nScoreBarLeftBound + cxScoreBar,nScoreBarUpBound + cyScoreBar);
	::SetRect(&_rectBest,nScoreBarLeftBound + cxScoreBar + 10,nScoreBarUpBound,nScoreBarLeftBound + cxScoreBar * 2 + 10,nScoreBarUpBound + cyScoreBar);
	::SetRect(&_rectScore,nScoreBarLeftBound + cxScoreBar * 2 + 20,nScoreBarUpBound,nScoreBarLeftBound + cxScoreBar * 3 + 20,nScoreBarUpBound + cyScoreBar);
	PaintSolidRoundRect(&_rectWorst,BKGND_COLOR,8);
	PaintSolidRoundRect(&_rectBest,BKGND_COLOR,8);
	PaintSolidRoundRect(&_rectScore,BKGND_COLOR,8);
	_rectWorst.bottom -= cyScoreBar / 2;
	_rectBest.bottom -= cyScoreBar / 2;
	_rectScore.bottom -= cyScoreBar / 2;
	PaintText(&_rectWorst,_T("WORST"),_T("Arial"),12,FW_BOLD,RGB(212,200,189));
	PaintText(&_rectBest,_T("BEST"),_T("Arial"),12,FW_BOLD,RGB(212,200,189));
	PaintText(&_rectScore,_T("SCORE"),_T("Arial"),12,FW_BOLD,RGB(212,200,189));
	::OffsetRect(&_rectWorst,0,cyScoreBar / 2);
	::OffsetRect(&_rectBest,0,cyScoreBar / 2);
	::OffsetRect(&_rectScore,0,cyScoreBar / 2);

	// Cache static background bitmap
	::ZeroMemory(&_BitmapInfo,sizeof(BITMAPINFO));
	_BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	_BitmapInfo.bmiHeader.biWidth = cxViewport;
	_BitmapInfo.bmiHeader.biHeight = cyViewport;
	_BitmapInfo.bmiHeader.biBitCount = 24;
	_BitmapInfo.bmiHeader.biPlanes = 1;
	_BitmapInfo.bmiHeader.biCompression = BI_RGB;
	::GetDIBits(_hDC,_hBitmap,0,cyViewport,NULL,&_BitmapInfo,DIB_RGB_COLORS);
	_pBkgndCache = new BYTE[_BitmapInfo.bmiHeader.biSizeImage];
	::GetDIBits(_hDC,_hBitmap,0,cyViewport,_pBkgndCache,&_BitmapInfo,DIB_RGB_COLORS);
}
void GameController::Destroy() {
	// Assert for debug
	assert(this->_hWnd != NULL);

	_hWnd = NULL;
	::SelectObject(_hDC,_hOldBitmap);
	::DeleteObject(_hBitmap);
	::DeleteDC(_hDC);
	delete[] _pBkgndCache;
	for (int i = 0; i < _nXorder; i++) {
		delete[] _Grid[i];
		delete[] _Cell[i];
	}
	delete[] _Grid;
	delete[] _Cell;
}
void GameController::OutputUI(HDC hDC) {
	// Assert for debug
	assert(this->_hWnd != NULL);

	// Restore static background bitmap
	::SetDIBits(_hDC,_hBitmap,0,_BitmapInfo.bmiHeader.biHeight,_pBkgndCache,&_BitmapInfo,DIB_RGB_COLORS);

	// Draw tiles
	for (int i = 0; i < _nXorder; i++) {
		for (int j = 0; j < _nYorder; j++) {
			Tile *pTile = _Cell[i][j];
			if (pTile != NULL) {
				Tile *pMergeTile = pTile->GetMergeTile();
				if (pMergeTile != NULL) {
					PaintTile(pMergeTile);
				}
				PaintTile(pTile);
			}
		}
	}

	// Draw score
	TCHAR szNumber[16];
	if (_nWorst == 0x7FFFFFFF) {
		_tcscpy(szNumber,_T("¡Þ"));
	} else {
		::_stprintf(szNumber,_T("%d"),_nWorst);
	}
	PaintText(&_rectWorst,szNumber,_T("Arial"),18,FW_BOLD,RGB(255,255,255));

	::_stprintf(szNumber,_T("%d"),_nBest);
	PaintText(&_rectBest,szNumber,_T("Arial"),18,FW_BOLD,RGB(255,255,255));

	::_stprintf(szNumber,_T("%d"),_nScore);
	PaintText(&_rectScore,szNumber,_T("Arial"),18,FW_BOLD,RGB(255,255,255));

	// Draw flow score
	if (_nFlowScoreOffset < 20) {
		RECT rectFlow;
		::CopyRect(&rectFlow,&_rectScore);
		::OffsetRect(&rectFlow,0,-2 * _nFlowScoreOffset);
		::_stprintf(szNumber,_T("+%d"),_nFlowScoreValue);
		PaintText(&rectFlow,szNumber,_T("Arial"),20,FW_BOLD,RGB(119,110,101));
	}

	// Copy bitmap to target DC
	::BitBlt(hDC,0,0,_BitmapInfo.bmiHeader.biWidth,_BitmapInfo.bmiHeader.biHeight,_hDC,0,0,SRCCOPY);
}
void GameController::AddRandomTile() {
	std::vector< std::pair<int,int> > avalible;
	for (int i = 0; i < _nXorder; i++) {
		for (int j = 0; j < _nYorder; j++) {
			if (_Cell[i][j] == NULL) {
				avalible.push_back(std::make_pair(i,j));
			}
		}
	}
	if (!avalible.empty()) {
		srand((DWORD)time(NULL));
		std::random_shuffle(avalible.begin(),avalible.end());
		std::pair<int,int> coord = avalible.at(0);
		Tile *pTile = new Tile(rand() % 100 >= 90 ? 1 : 0);
		SetCellTile(coord,pTile);
		pTile->TransformSpwan(RectAt(coord),10);
		_TransformingTile.push_back(pTile);
	}
}
const RECT &GameController::RectAt(std::pair<int,int> &coord) {
	return _Grid[coord.first][coord.second];
}
void GameController::SetCellTile(std::pair<int,int> &coord,Tile *pTile) {
	_Cell[coord.first][coord.second] = pTile;
}
Tile *GameController::GetCellTile(std::pair<int,int> &coord) {
	return _Cell[coord.first][coord.second];
}
void GameController::MoveTile(std::pair<int,int> &coordFrom,std::pair<int,int> &coordTo) {
	Tile *pTile = _Cell[coordFrom.first][coordFrom.second];
	_Cell[coordTo.first][coordTo.second] = pTile;
	_Cell[coordFrom.first][coordFrom.second] = NULL;
	int dx = RectAt(coordTo).left - RectAt(coordFrom).left;
	int dy = RectAt(coordTo).top - RectAt(coordFrom).top;
	if (!pTile->HasNextFrame()) {
		_TransformingTile.push_back(pTile);
	}
	pTile->TransformOffset(dx,dy,10);
}
void GameController::MergeTile(std::pair<int,int> &coord,std::pair<int,int> &coordFrom) {
	Tile *pTile = GetCellTile(coord);
	Tile *pMergeTile = GetCellTile(coordFrom);
	SetCellTile(coordFrom,NULL);
	pTile->SetMergeTile(pMergeTile);
	pTile->SetLevel(pTile->GetLevel() + 1);
	if (!pTile->HasNextFrame()) {
		_TransformingTile.push_back(pTile);
	} else {
		pTile->SkipFrame();
	}
	pTile->TransformBlink(10,10,10);
	int dx = RectAt(coord).left - RectAt(coordFrom).left;
	int dy = RectAt(coord).top - RectAt(coordFrom).top;
	pMergeTile->TransformOffset(dx,dy,10);
}
void GameController::Swipe(int direction) {
	while (!_TransformingTile.empty()) {
		Tile *pTile = _TransformingTile.front();
		pTile->SetMergeTile(NULL);
		pTile->SkipFrame();
		_TransformingTile.pop_front();
	}
	std::vector< std::vector< std::pair<int,int> > > sequence;
	switch (direction) {
	case GameController::SWIPE_LEFT:
		for (int i = 0; i < _nYorder; i++) {
			std::vector< std::pair<int,int> > line;
			for (int j = 0; j < _nXorder; j++) {
				line.push_back(std::make_pair(j,i));
			}
			sequence.push_back(line);
		}
		break;
	case GameController::SWIPE_UP:
		for (int i = 0; i < _nXorder; i++) {
			std::vector< std::pair<int,int> > line;
			for (int j = 0; j < _nYorder; j++) {
				line.push_back(std::make_pair(i,j));
			}
			sequence.push_back(line);
		}
		break;
	case GameController::SWIPE_RIGHT:
		for (int i = 0; i < _nYorder; i++) {
			std::vector< std::pair<int,int> > line;
			for (int j = _nXorder - 1; j >= 0; j--) {
				line.push_back(std::make_pair(j,i));
			}
			sequence.push_back(line);
		}
		break;
	case GameController::SWIPE_DOWN:
		for (int i = 0; i < _nXorder; i++) {
			std::vector< std::pair<int,int> > line;
			for (int j = _nYorder - 1; j >= 0; j--) {
				line.push_back(std::make_pair(i,j));
			}
			sequence.push_back(line);
		}
		break;
	}
	BOOL moved = FALSE;
	for (size_t i = 0; i < sequence.size(); i++) {
		std::vector< std::pair<int,int> > &line = sequence.at(i);
		for (size_t j = 0; j < line.size() - 1; j++) {
			size_t k = j + 1;
			while (k !=  line.size() - 1 && GetCellTile(line.at(k)) == NULL) k++;
			std::pair<int,int> coord = line.at(j);
			std::pair<int,int> coordNext = line.at(k);
			if (GetCellTile(coordNext) == NULL) continue;
			if (GetCellTile(coord) == NULL) {
				moved = TRUE;
				MoveTile(coordNext,coord);
				j--;
			} else if (GetCellTile(coord)->GetLevel() == GetCellTile(coordNext)->GetLevel()) {
				moved = TRUE;
				MergeTile(coord,coordNext);
				_nScore += TILE_SCORES[GetCellTile(coord)->GetLevel()];
				_nFlowScoreOffset = 0;
				_nFlowScoreValue = TILE_SCORES[GetCellTile(coord)->GetLevel()];
				if (_nBest < _nScore) {
					_nBest = _nScore;
				}
			}
		}
	}
	if (moved) {
		AddRandomTile();
	}
	::SetTimer(_hWnd,1,ANIM_TIMER_INTERVAL,NULL);
}

void GameController::StartGame() {
	_nScore = 0;
	while (!_TransformingTile.empty()) {
		_TransformingTile.pop_front();
	}
	for (int i = 0; i < _nXorder; i++) {
		for (int j = 0; j < _nYorder; j++) {
			std::pair<int,int> cord = std::make_pair(i,j);
			if (NULL != GetCellTile(cord)) {
				delete GetCellTile(cord);
				SetCellTile(cord,NULL);
			}
		}
	}
	AddRandomTile();
	AddRandomTile();
	
	::SetTimer(_hWnd,1,ANIM_TIMER_INTERVAL,NULL);
}
void GameController::NextFrame() {
	for (int i = _TransformingTile.size(); i != 0; i--) {
		Tile *pTile = _TransformingTile.front();
		_TransformingTile.pop_front();
		pTile->NextFrame();
		if (pTile->HasNextFrame()) {
			Tile *pMergeTile = pTile->GetMergeTile();
			if (pMergeTile != NULL) {
				if (!pMergeTile->HasNextFrame()) {
					pTile->SetMergeTile(NULL);
				} else {
					pMergeTile->NextFrame();
				}
			}
			_TransformingTile.push_back(pTile);
		} else {
			pTile->SetMergeTile(NULL);
		}
	}
	if (_nFlowScoreOffset < 20) {
		_nFlowScoreOffset++;
	}
	::RedrawWindow(_hWnd,NULL,NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	if (_TransformingTile.empty() && _nFlowScoreOffset >= 20) {
        ::KillTimer(_hWnd,1);
	}
}

BOOL GameController::IsGameOver() {
	const int STEP[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};
	for (int i = 0; i < _nXorder; i++) {
		for (int j = 0; j < _nYorder; j++) {
			std::pair<int,int> cord1 = std::make_pair(i,j);
			Tile *pTile1 = GetCellTile(cord1);
			if (pTile1 == NULL) return FALSE;
			for (int k = 0; k < 4; k++) {
				int nx = i + STEP[k][0];
				int ny = j + STEP[k][1];
				if (nx < 0 || nx >= _nXorder || ny < 0 || ny >= _nYorder) continue;
				std::pair<int,int> cord1 = std::make_pair(nx,ny);
				Tile *pTile2 = GetCellTile(cord1);
				if (pTile2 == NULL) return FALSE;
				if (pTile1->GetLevel() == pTile2->GetLevel()) return FALSE;
			}
		}
	}
	if (_nScore < _nWorst) {
		_nWorst = _nScore;
	}
	return TRUE;
}
