#pragma once

class CSpider : public CSprite
{
public:
	enum STATE { NONE, IDLE, PATROL, CHASE, ATTACK, RETURNTONEST, DIE };

	float PATROL_SPEED = 40;
	float CHASE_SPEED = 60;
	CVector wallPos;
	CVector nestPos;
	string enemyType;


private:
	STATE state;
	CVector enemyPosition;
	float health;
	

public:
	CSpider(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time);
	CSpider(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time);
	~CSpider(void);

	void OnAttacked();
	void OnUpdate(Uint32 time, Uint32 deltaTime);
	void ChangeState(STATE newState);
	STATE GetState() { return state; }
	float GetHealth() { return health; }
	void SetEnemyPosition(CVector v) { enemyPosition = v; }
	void Attacked();
};

typedef std::_gfc_std_ext<std::list<CSpider*>> CSpiderList;
