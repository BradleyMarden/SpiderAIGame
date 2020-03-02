#include "stdafx.h"
#include "NestControll.h"
NestControll::NestControll(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	LoadImage(pFileBitmap, "nest");
	SetImage("nest");
	SetSize(100, 100);
}
NestControll::NestControll(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	LoadImage(pGraphics, "nest");
	SetImage("nest");
	SetSize(100, 100);
}
NestControll::~NestControll(void)
{
}

