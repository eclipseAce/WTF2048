#ifndef TILE_H
#define TILE_H

#include <windows.h>
#include <deque>
#include <cmath>

class Tile {
private:
	int _nLevel;
	Tile *_pMergeTile;
	RECT _Rect;
	std::deque<RECT> _Transform;

public:
	Tile(int nLevel);
	~Tile();
	void SetLevel(int nLevel);
	int GetLevel() const;
	void SetMergeTile(Tile *pTile);
	Tile *GetMergeTile() const;
	BOOL HasNextFrame() const;
	void NextFrame();
	void SkipFrame();
	void GetFinalFrame(RECT &rect) const;
	void GetCurrentFrame(RECT &rect) const;
	void TransformSpwan(const RECT &rect,int nFrame);
	void TransformOffset(int dx,int dy,int nFrame);
	void TransformBlink(int dx,int dy,int nFrame);
};

#endif // TILE_H
