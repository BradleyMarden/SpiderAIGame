#include "stdafx.h"
#include "SilkSpawner.h"


SilkSpawner::SilkSpawner(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	LoadImage(pFileBitmap, "web");
	SetImage("web");
	SetSize(50, 50);

}
SilkSpawner::SilkSpawner(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	LoadImage(pGraphics, "web");
	SetImage("web");
	SetSize(50, 50);

}
SilkSpawner::~SilkSpawner(void)
{
}
