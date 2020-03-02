#include "stdafx.h"
#include "MinionSpider.h"


MinionSpider::MinionSpider(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	LoadAnimation(pFileBitmap, "walk", CSprite::Sheet(4, 2).Col(0).From(0).To(1));
	LoadAnimation(pFileBitmap, "attack", CSprite::Sheet(4, 2).Col(1).From(0).To(1));
	LoadAnimation(pFileBitmap, "idle", CSprite::Sheet(4, 2).Col(2).From(0).To(1));
	LoadAnimation(pFileBitmap, "death", CSprite::Sheet(4, 2).Col(3).From(0).To(0));


	health = 100;
	state = NONE;
	ChangeState(IDLE);
	wallPos.SetX(650);
	wallPos.SetY(200);
}

MinionSpider::MinionSpider(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	LoadAnimation(pGraphics, "walk", CSprite::Sheet(4, 2).Col(0).From(0).To(1));
	LoadAnimation(pGraphics, "attack", CSprite::Sheet(4, 2).Col(1).From(0).To(1));
	LoadAnimation(pGraphics, "idle", CSprite::Sheet(4, 2).Col(2).From(0).To(1));
	LoadAnimation(pGraphics, "death", CSprite::Sheet(4, 2).Col(3).From(0).To(0));
	SetAnimation("idle", 8);

	health = 100;
	state = NONE;
	ChangeState(IDLE);
	wallPos.SetX(650);
	wallPos.SetY(200);
}

MinionSpider::~MinionSpider(void)
{
}

void MinionSpider::OnAttacked()
{
	health -= 0.5f;
	if (health < 0) health = 0;
}

void MinionSpider::OnUpdate(Uint32 time, Uint32 deltaTime)
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
	case CHASE:
		health -= 0.05f;
		if (moveToPoint.GetY() > 200 && GetPos().GetY() >200) //if minion is above the wall
		{
			SetDirection(moveToPoint - GetPos());
		}
		if (moveToPoint.GetY() > 200 && GetPos().GetY() < 200) //if minion is below the wall
		{
			SetDirection(wallPos - GetPos());//ai pathfinds to the gap in the wall
		}
		if (moveToPoint.GetY() < 200)//if move to point is below the wall
		{
			SetDirection(wallPos - GetPos());//ai pathfinds to the gap in the wall
			if (GetPos().GetY() <= 200)//if ai is still below the wall
			{
				SetDirection(moveToPoint - GetPos());
			}
		}
		break;
	case ATTACK:
		health -= 0.05f;
		break;
	case DIE:
		break;
	}
	// Generic behaviour: bounce from the screen margins
	if (GetX() < 32 || GetX() > 1280 - 32) SetVelocity(Reflect(GetVelocity(), CVector(1, 0)));
	if (GetY() < 32 || GetY() > 768 - 32) SetVelocity(Reflect(GetVelocity(), CVector(0, 1)));
	// if in motion, rotate so that to follow the direction of the motion; otherwise reset rotation
	if (GetSpeed() > 0)
		SetRotation(GetDirection() - 90);
	else if (state != ATTACK)
		SetRotation(0);

	// Transitions
	float distanceToPoint = Distance(moveToPoint, GetPosition());
	switch (state)
	{
	case IDLE:
		if (health < 1)
		{
			ChangeState(DIE);
		}
		break;
	case CHASE:
		if (distanceToPoint < 70 )
		{
			/*if (rand()% 20 == 1)
			{
				ChangeState(IDLE);
			}*/
			ChangeState(IDLE);

		}
		if (health <1)
		{
			ChangeState(DIE);
		}
		break;
	case ATTACK:
		if ( enemyType != "spider" && health >= 30)
		{
			ChangeState(CHASE);
		}
		if (health < 1)
		{
			ChangeState(DIE);
		}
		break;
	case DIE:
		break;
	}
	CSprite::OnUpdate(time, deltaTime);
}


void MinionSpider::ChangeState(STATE newState)
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
	case CHASE:
		SetAnimation("walk");
		SetSpeed(50);
		break;
	case ATTACK:
		SetVelocity(0, 0);
		SetAnimation("attack", 4);
		break;
	case DIE:
		SetVelocity(0, 0);
		health = 0;
		SetAnimation("death");
		break;
	}
}

