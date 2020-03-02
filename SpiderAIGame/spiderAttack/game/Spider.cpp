#include "stdafx.h"
#include "Spider.h"


CSpider::CSpider(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	LoadAnimation(pFileBitmap, "walk", CSprite::Sheet(4, 2).Col(0).From(0).To(1));
	LoadAnimation(pFileBitmap, "attack", CSprite::Sheet(4, 2).Col(1).From(0).To(1));
	LoadAnimation(pFileBitmap, "idle", CSprite::Sheet(4, 2).Col(2).From(0).To(1));
	LoadAnimation(pFileBitmap, "death", CSprite::Sheet(4, 2).Col(3).From(0).To(0));
	
	health = 100;
	state = NONE;
	ChangeState(PATROL);
	wallPos.SetX(650);
	wallPos.SetY(200);
}

CSpider::CSpider(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	LoadAnimation(pGraphics, "walk", CSprite::Sheet(4, 2).Col(0).From(0).To(1));
	LoadAnimation(pGraphics, "attack", CSprite::Sheet(4, 2).Col(1).From(0).To(1));
	LoadAnimation(pGraphics, "idle", CSprite::Sheet(4, 2).Col(2).From(0).To(1));
	LoadAnimation(pGraphics, "death", CSprite::Sheet(4, 2).Col(3).From(0).To(0));
	SetAnimation("idle", 8);

	health = 100;
	state = NONE;
	ChangeState(PATROL);
	wallPos.SetX(650);
	wallPos.SetY(200);
}

CSpider::~CSpider(void)
{
}

void CSpider::OnAttacked()
{
	health -= 0.5f;
	if (health < 0) health = 0;
}

void CSpider::OnUpdate(Uint32 time, Uint32 deltaTime)
{
	// State-dependent actions
	switch (state)
	{
	case IDLE:
		if (health < 100)
		{
			health += 0.1f;
		}
		break;
	case PATROL:
		if (rand() % 80 == 1)//picks random direction
		{
			SetDirection(GetDirection() + (float)(rand() % 45));
		}
		break;
	case CHASE:
		health -= 0.05f;
		if (enemyPosition.GetY() > 200) //if player is above the wall
		{
			SetDirection(enemyPosition - GetPos());
		}
		if (enemyPosition.GetY() <200)//if player is below the wall
		{
			SetDirection(wallPos - GetPos());//ai pathfinds to the gap in the wall
			if(GetPos().GetY() <=200)//if player is still below the wall
			{
				SetDirection(enemyPosition - GetPos());
			}
		}
		break;
	case ATTACK:
		health -= 0.05f;
		break;
	case RETURNTONEST:
		health -= 0.05f;
		break;
	case DIE:
		break;
	}


	// Generic behaviour: bounce from the screen margins
	if (GetX() < 32 || GetX() > 1280-32) SetVelocity(Reflect(GetVelocity(), CVector(1, 0)));
	if (GetY() < 32 || GetY() > 768-32) SetVelocity(Reflect(GetVelocity(), CVector(0, 1)));
	// if in motion, rotate so that to follow the direction of the motion; otherwise reset rotation
	if (GetSpeed() > 0)
		SetRotation(GetDirection() - 90);
	else if (state != ATTACK)
		SetRotation(0);

	// Transitions
	float enemyDistance = Distance(enemyPosition, GetPosition());
	switch (state)
	{
	case IDLE:
		if (health > 90)
		{
			ChangeState(PATROL);
		}
		else if (enemyDistance <= 300 && health >= 30 && enemyPosition.GetY() > 210)//if player is less than 300 pixles away, the enemy health is greater than 20 and the player is above the wall
		{
			ChangeState(CHASE);
		}
		else if (enemyDistance < 50 && health >=30)//if player is close enough attack
		{
			ChangeState(ATTACK);
		}
		else if (health<1)
		{
			ChangeState(DIE);
		}
		break;
	case PATROL:
		if (enemyDistance < 300 && enemyPosition.GetY() > 210)//if enemy is within chase range and above the wall
		{
			ChangeState(CHASE);
		}
		else if (health<30)//if health is low, retreat to nest
		{
			ChangeState(RETURNTONEST);
		}
		break;
	case CHASE:
		if (enemyDistance > 300)//if player is out of range
		{
			//return to nest if too far away
			if (Distance(nestPos, GetPosition()))
			{
				ChangeState(RETURNTONEST);
			}
			else
			{
				ChangeState(IDLE);
			}
		}
		else if (enemyDistance< 50 && health >= 30)//attack player
		{
			ChangeState(ATTACK);
		}
		else if (health < 20)
		{
			ChangeState(RETURNTONEST);//retreat
		}
		break;
	case ATTACK:
		if (enemyDistance > 30 && enemyType != "minion" && health >= 30)
		{
			ChangeState(CHASE);
		}
		if (health<20 && enemyType != "minion")
		{
			ChangeState(RETURNTONEST);
		}
		else if (health < 1)
		{
			ChangeState(DIE);
		}
		break;
	case RETURNTONEST:
		if (GetY() < 200)//if spider is below wall
		{
			SetDirection(wallPos - GetPos());//pathfind to wall point
		}
		else
		{
			SetDirection(nestPos - GetPos());//otherwise spider is above the wall
		}
		if (Distance(nestPos, GetPosition()) < 50)//if close enough to nest
		{
			ChangeState(IDLE);
		}
		else if (health<1)
		{
			ChangeState(DIE);
		}
		break;
	case DIE:
		break;
	}
	CSprite::OnUpdate(time, deltaTime);
}

void CSpider::ChangeState(STATE newState)
{
	if (newState == state)
		return;		// No state change
	state = newState;
	switch (state)
	{
	case IDLE:
		SetVelocity(0, 0); 
		SetAnimation("idle", 4);
		break;
	case PATROL:
		SetAnimation("walk");
		SetSpeed(25);
		break;
	case CHASE:
		SetAnimation("walk");
		SetSpeed(50);
		break;
	case ATTACK:
		SetVelocity(0, 0);
		SetAnimation("attack",4);
		break;
	case RETURNTONEST:
		SetAnimation("walk");
		break;
	case DIE:
		SetVelocity(0, 0);
		health = 0;
		SetAnimation("death");
		break;
	}
}
