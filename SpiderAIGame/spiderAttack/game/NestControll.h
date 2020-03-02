#pragma once
class NestControll:public CSprite
{
	
public:
private:

public:
NestControll(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time);
NestControll(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time);
~NestControll(void);




};
typedef std::_gfc_std_ext<std::list<NestControll*>> NestControllList;


