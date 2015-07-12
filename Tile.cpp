#include "Tile.h"

Tile::Tile(int nLevel) {
	_nLevel = nLevel;
	_pMergeTile = NULL;
}
Tile::~Tile() {
	if (_pMergeTile != NULL) {
		delete[] _pMergeTile;
	}
}
void Tile::SetLevel(int nLevel) {
	_nLevel = nLevel;
}
int Tile::GetLevel() const {
	return _nLevel;
}
void Tile::SetMergeTile(Tile *pTile) {
	if (_pMergeTile != NULL) {
		delete _pMergeTile;
	}
	_pMergeTile = pTile;
}
Tile *Tile::GetMergeTile() const {
	return _pMergeTile;
}

BOOL Tile::HasNextFrame() const {
	return (!_Transform.empty());
}
void Tile::NextFrame() {
	if (HasNextFrame()) {
		::CopyRect(&_Rect,&_Transform.front());
		_Transform.pop_front();
	}
}
void Tile::SkipFrame() {
	if (HasNextFrame()) {
		::CopyRect(&_Rect,&_Transform.back());
		_Transform.clear();
	}
}
void Tile::GetFinalFrame(RECT &rect) const {
	if (HasNextFrame()) {
		::CopyRect(&rect,&_Transform.back());
	} else {
		::CopyRect(&rect,&_Rect);
	}
}
void Tile::GetCurrentFrame(RECT &rect) const {
	::CopyRect(&rect,&_Rect);
}

void Tile::TransformSpwan(const RECT &rect,int nFrame) {
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	int dx = (int)std::floor((float)width / 2 / nFrame);
	int dy = (int)std::floor((float)height / 2 / nFrame);

	RECT rectT;
	::CopyRect(&rectT,&rect);
	::InflateRect(&rectT,-width / 2,-height / 2);
	while (--nFrame != 1) {
		::InflateRect(&rectT,dx,dy);
		_Transform.push_back(rectT);
	}
	_Transform.push_back(rect);
}
void Tile::TransformOffset(int dx,int dy,int nFrame) {
	RECT rectT,rectF;
	GetFinalFrame(rectT);
	::CopyRect(&rectF,&rectT);
	::OffsetRect(&rectF,dx,dy);

	int gx = (int)(dx > 0 ? std::floor((float)dx / nFrame) : std::ceil((float)dx / nFrame));
	int gy = (int)(dy > 0 ? std::floor((float)dy / nFrame) : std::ceil((float)dy / nFrame));

	while (--nFrame != 1) {
		::OffsetRect(&rectT,gx,gy);
		_Transform.push_back(rectT);
	}
	_Transform.push_back(rectF);
}
void Tile::TransformBlink(int dx,int dy,int nFrame) {
	RECT rectT,rectF;
	GetFinalFrame(rectT);
	::CopyRect(&rectF,&rectT);

	int gx = (int)(dx > 0 ? std::floor((float)dx / nFrame) : std::ceil((float)dx / nFrame));
	int gy = (int)(dy > 0 ? std::floor((float)dy / nFrame) : std::ceil((float)dy / nFrame));

	int i = nFrame;
	while (--i != nFrame / 2) {
		::InflateRect(&rectT,gx,gy);
		_Transform.push_back(rectT);
	}
	while (--i != 1) {
		::InflateRect(&rectT,-gx,-gy);
		_Transform.push_back(rectT);
	}
	_Transform.push_back(rectF);
}
