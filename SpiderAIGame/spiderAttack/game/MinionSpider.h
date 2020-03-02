#pragma once
class MinionSpider : public CSprite
{
	
	public:
		enum STATE { NONE, IDLE, CHASE, ATTACK, DIE };

		float CHASE_SPEED = 60;
		CVector wallPos;
		CVector nestPos;
		CVector enemyPositions[10];
		CVector moveToPoint;
		string enemyType;

	private:
		STATE state;
		CVector enemyPosition;
		float health;


	public:
		MinionSpider(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time);
		MinionSpider(Sint16 x, Sint16 y, CGraphics * pGraphics, Uint32 time);
		~MinionSpider(void);

		void OnAttacked();
		void OnUpdate(Uint32 time, Uint32 deltaTime);
		void ChangeState(STATE newState);
		STATE GetState() { return state; }
		float GetHealth() { return health; }
		void SetEnemyPosition(CVector v) { enemyPosition = v; }
		
		//void Attacked();
};

typedef std::_gfc_std_ext<std::list<MinionSpider*>> MinionSpiderList;


