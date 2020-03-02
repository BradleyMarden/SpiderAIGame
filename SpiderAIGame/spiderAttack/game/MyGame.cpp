#include "stdafx.h"
#include "MyGame.h"

#pragma warning(disable:4244)

CMyGame::CMyGame(void) : 
	m_player(640, 384, "boy.png", 0),
	m_background("tile_5.png"),
	lWall(300, 200, 600, 10, CColor::DarkYellow(), CColor::Black(), 0),
	rWall(1000, 200, 600, 10, CColor::DarkYellow(), CColor::Black(), 0),
	playerNest(200,100,100,100,"PlayerNest.png", 0)

{
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	if (!IsGameMode() || m_spiders.size() == 0) return;

	Uint32 t = GetTime();

	// Update AI Agents
	bool bAttack = IsKeyDown(SDLK_LCTRL) || IsKeyDown(SDLK_RCTRL);
	if (IsKeyDown(SDLK_w) || IsKeyDown(SDLK_UP)) m_player.Input(CPlayer::UP, bAttack);
	else if (IsKeyDown(SDLK_s) || IsKeyDown(SDLK_DOWN)) m_player.Input(CPlayer::DOWN, bAttack);
	else if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT)) m_player.Input(CPlayer::LEFT, bAttack);
	else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT)) m_player.Input(CPlayer::RIGHT, bAttack);
	else m_player.Input(CPlayer::NO_DIR, bAttack);
	for (SilkSpawner* silk : silks)
	{
		silk->Update(t);
	}
	for (MinionSpider* minion : minionSpiders)
		minion->Update(t);
	float health = 0;
	int iter = 0;

	for (CSpider *pSpider : m_spiders)
	{
		pSpider->Update(t);
		health += pSpider->GetHealth();
		pSpider->SetEnemyPosition(m_player.GetPosition());
		//tells the minions where each spider is
		for(MinionSpider* minion : minionSpiders)
		{
			minion->enemyPositions[iter] = pSpider->GetPos();
			iter++;
		}

		if (Distance(pSpider->GetPosition(), m_player.GetPosition()) <= 64)//spider will attack player if within range
		{
			if (pSpider->GetState() == CSpider::ATTACK)
			{
				m_player.OnAttacked();
				pSpider->enemyType = "player";

			}
			if (m_player.GetState() == CPlayer::ATTACK)
			{
				pSpider->OnAttacked();
				pSpider->enemyType = "minion";

			}
		}

	}
	for (MinionSpider* minion : minionSpiders)
	{
		for (CSpider* spider : m_spiders)
		{
			if (Distance(minion->GetPosition(), spider->GetPosition()) <= 64 && spider->GetState() != spider->DIE && minion->GetState() != minion->DIE)//minion and spider will attack each other if in range
			{
				minion->ChangeState(minion->ATTACK);
				spider->ChangeState(spider->ATTACK);
				if (minion->GetState() == MinionSpider::ATTACK)
				{
					spider->enemyType = "minion";//tells the spider that the enemy type is a minion and not a player

					spider->OnAttacked();
					if (spider->GetHealth() <1)
					{
						minion->ChangeState(minion->IDLE);
						spider->ChangeState(spider->DIE);
					}
				}

				if (spider->GetState() == CSpider::ATTACK)
				{
					minion->enemyType = "spider";

					minion->OnAttacked();
					if (minion->GetHealth() < 1)
					{

						spider->ChangeState(spider->IDLE);
						minion->ChangeState(minion->DIE);
						

					}
				}
			}
		}
	}
	if (rand() % 300 == 1)//silk spawner over time
	{
		if (silks.size() < 9)
		{
			silks.push_back(new SilkSpawner(rand() % (1000 + 1 - 100) + 100, rand() % (680 + 1 - 300) + 300, "Web.png", 0));//spawns silks
		}
		
	}

	for (SilkSpawner* silk : silks)//allows player to collect silk
	{
		if (Distance(silk->GetPosition(), m_player.GetPosition()) <= 35 && m_player.silkCollected < m_player.maxWeb)//if player within rnage collect silk
		{
			m_player.AddSilk();//increment the silk collected amount
			silk->SetPosition(rand() % (1000 + 1 - 100) + 100, rand() % (680 + 1 - 300) + 300);//repositions silk
		}
	}
	if (m_player.silkCollected == m_player.maxWeb)//check for displaying if the player has enough silk to spawn minion
	{
		m_player.nestAvaliable = true;
	}

	if (m_player.GetHealth() <= 0 || health <= 0)
		GameOver();

	m_player.Update(t);
	
	if (m_player.HitTest(&rWall))
	{
		m_player.SetVelocity(Reflect(m_player.GetVelocity(), CVector(0, 1)));//rebounds the player off of the wall in the middle of the scene
		
	}
	if (m_player.HitTest(&lWall))
	{
		m_player.SetVelocity(Reflect(m_player.GetVelocity(), CVector(0, 1)));

	}
	for (CSpider* pSpider : m_spiders)
	{
		if (pSpider->HitTest(&rWall))
		{
			pSpider->SetVelocity(Reflect(pSpider->GetVelocity(), CVector(0, 1)));//rebounds the enemy off of the wall in the middle of the scene

		}
		if (pSpider->HitTest(&lWall))
		{
			pSpider->SetVelocity(Reflect(pSpider->GetVelocity(), CVector(0, 1)));
		}
	}
	int add = 0;
	for (MinionSpider* minion : minionSpiders)
	{
		for (CSpider* spider : m_spiders)
		{
		
			//cout << "enemy poses x: " << minion->enemyPositions[add].GetX() << "y: " << minion->enemyPositions[add].GetY() << endl;
			add++;
		}
	}
	MinionSpiderControl();
}
void CMyGame::MinionSpiderControl()
{
	if (Distance(playerNest.GetPos(), m_player.GetPos()) <= 64)
	{
		if (m_player.silkCollected>3 &&  minionSpiders.size() <  m_player.maxMinions)//spawn minion
		{
			m_player.silkCollected -= 3;
			minionSpiders.push_back(new MinionSpider(350, 100, "Spider64Boss.png",0));
			m_player.nestAvaliable = false;
		}
	}
}


void DrawHealth(CGraphics* g, CVector pos, float w, float h, float health)
{
	if (health <= 0) return;
	g->DrawRect(CRectangle(pos.m_x - 1, pos.m_y - 1, w + 1, h + 1), CColor::Black());
	g->FillRect(CRectangle(pos.m_x, pos.m_y - 1, ceil(w * health / 100.f), h), health > 20 ? CColor::DarkGreen() : CColor::Red());
}

void CMyGame::OnDraw(CGraphics* g)
{
	CColor colorForSilksCollected;//number of silks collected ui color
	// Draw background
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 3; j++)
			g->Blit(CVector(256.f*i, 256.f *j), m_background);
	rWall.Draw(g);
	lWall.Draw(g);
	m_player.Draw(g);
	DrawHealth(g, m_player.GetPosition() + CVector(-32, 32), 20, 4, m_player.GetHealth());
	playerNest.Draw(g);
	for (SilkSpawner* silk : silks)
		silk->Draw(g);
	for (NestControll* nest : nests)
		nest->Draw(g);
	
	*g << "Silks Collected: ";
	if (m_player.silkCollected == 0)//default color for 0 silk
	{
		colorForSilksCollected = CColor::Red();
	}
	else if(m_player.silkCollected > 0 && m_player.silkCollected < 5)//changes color to yellow if more than 1 collected
	{
		colorForSilksCollected = CColor::Yellow();
	}
	else // (m_player.silkCollected >= 5) {}
	{
		colorForSilksCollected = CColor::Green();//changes color to gree if more than 5 collected
	}
	*g << font(20) << color(colorForSilksCollected) <<m_player.silkCollected;//displays silk collected text
	for (CSpider* pSpider : m_spiders)
	{
		pSpider->Draw(g);
		DrawHealth(g, pSpider->GetPosition() + CVector(-32, 32), 20, 4, pSpider->GetHealth());
	}
	for (MinionSpider* minion : minionSpiders)
	{
		minion->Draw(g);
		DrawHealth(g, minion->GetPosition() + CVector(-32, 32), 20, 4, minion->GetHealth());
	}
	if (m_player.nestAvaliable && minionSpiders.size() < 2)//enable minion avaliable text to player
	{
		*g << font(48) << color(CColor::LightGreen()) << vcenter << top << center <<  "Return to Nest, Minion Avaliable!" << endl;//tells player that they have enough silk to spawn minion
	}
	if (minionSpiders.size() ==2)
	{
		*g << font(48) << color(CColor::LightGreen()) << vcenter << top << center << "Max Minion Amount Reached!" << endl;//shows player that they cannot spawn any more minions
	}

	if (IsGameOver())
		if (m_player.GetHealth() <= 0)
			* g << font(48) << color(CColor::DarkRed()) << vcenter << center << "GAME OVER" << endl;
		else
			*g << font(48) << color(CColor::DarkBlue()) << vcenter << center << "YOU'RE A SPIDER KILLER!" << endl;

	*g << font(10) << bottom << right << color(CColor::White()) << "Created By Bradley Marden" << endl;//displays created by
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	StartGame();	// exits the menu mode and starts the game mode
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	m_spiders.delete_all();

	//spawn spiders
	m_spiders.push_back(new CSpider(640, 550, "Spider64Enemy.png", 0));
	m_spiders.push_back(new CSpider(900, 500, "Spider64Enemy.png", 0));
	m_spiders.push_back(new CSpider(1200, 576, "Spider64Enemy.png", 0));
	m_spiders.push_back(new CSpider(200, 450, "Spider64Enemy.png", 0));
	m_spiders.push_back(new CSpider(300, 600, "Spider64Enemy.png", 0));
	m_spiders.push_back(new CSpider(300, 650, "Spider64Enemy.png", 0));


	//spawn webs to start with
	//silks.push_back(new SilkSpawner(rand() % 1200, rand() % 740 + 400, "Web.png", 0));
	//silks.push_back(new SilkSpawner(rand() % 1200, rand() % 740 + 400, "Web.png", 0));

	//spawn nests
	nests.push_back(new NestControll(rand() % (1000 + 1 - 100)+100, rand() % (680 + 1 - 300) + 300, "nest.png", 0));
	nests.push_back(new NestControll(rand() % (1000 + 1 - 100) + 100, rand() % (680 + 1 - 300) + 300, "nest.png", 0));
	nests.push_back(new NestControll(rand() % (1000 + 1 - 100) + 100, rand() % (680 + 1 - 300) + 300, "nest.png", 0));

	int counter = 0;
	for (NestControll* nest : nests)
	{
		nestPostions[counter] = nest->GetPosition();
		counter++;
	}
	//spawn player
	m_player.SetPosition(1100, 100);
	m_player.ChangeState(CPlayer::IDLE, CPlayer::DOWN);
	m_player.SetHealth(100);

	//world setup
	gapInWallPos.SetX(650);
	gapInWallPos.SetY(200);
	for (CSpider* pSpider : m_spiders)//assigns a spider a random nest from array
	{
		pSpider->nestPos = nestPostions[rand()% 3];
		cout << "spider nest at x: " << pSpider->nestPos.GetX() << "y: " <<pSpider->nestPos.GetY() << endl;
	}
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
}

// called when the game is over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();

	if (sym == SDLK_1) (*m_spiders.begin())->ChangeState(CSpider::IDLE);
	if (sym == SDLK_2) (*m_spiders.begin())->ChangeState(CSpider::PATROL);
	if (sym == SDLK_3) (*m_spiders.begin())->ChangeState(CSpider::CHASE);
	if (sym == SDLK_4) (*m_spiders.begin())->ChangeState(CSpider::ATTACK);
	if (sym == SDLK_6) (*m_spiders.begin())->ChangeState(CSpider::DIE);
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}

/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	for (MinionSpider* minion : minionSpiders)//controls minion on click movement
	{
		if (minion->GetState() != minion->DIE)
		{
			//run move funtion to mouse point
			minion->moveToPoint.SetX(x);
			minion->moveToPoint.SetY(y);
			minion->ChangeState(minion->CHASE);
		}
	}
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
