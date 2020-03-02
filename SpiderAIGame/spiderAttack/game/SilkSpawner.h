#pragma once
class SilkSpawner : public CSprite
{
public:



	


private:
	


public:
	SilkSpawner(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time);
	SilkSpawner(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time);
	~SilkSpawner(void);

	

	

};

typedef std::_gfc_std_ext<std::list<SilkSpawner*>> SilkSpawnerList;
