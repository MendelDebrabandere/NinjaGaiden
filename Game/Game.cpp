#include "pch.h"
#include "Game.h"
#include "Level.h"
#include "Avatar.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "ProjectileManager.h"
#include "HUD.h"
#include "PowerupManager.h"
#include "TextureManager.h"
#include "utils.h"
#include <iostream>
#include "Texture.h"
#include "SoundManager.h"

Game::Game( const Window& window ) 
	:m_Window{ window }
{
	Initialize( );
}

Game::~Game( )
{
	Cleanup( );
}

void Game::Initialize( )
{
	std::cout << "Press I for info\n\n";

	glScalef(3, 3, 1);

	m_pTextureManager = new TextureManager();
	m_pSoundManager = new SoundManager();
	m_pLevel = new Level(m_pSoundManager, m_pTextureManager, false);
	m_pAvatar = new Avatar(m_pSoundManager, m_pTextureManager);
	m_pCamera = new Camera(m_Window.width/3);
	m_pCamera->SetLevelBoundaries(Rectf{ 0,0,m_pLevel->GetLevelWidth(),208 });
	m_pEnemyManager = new EnemyManager(m_pSoundManager, m_pTextureManager, false);
	m_pProjectileManager = new ProjectileManager(m_pTextureManager);
	m_pHUD = new HUD(m_pTextureManager, false);
	m_pPowerupManager = new PowerupManager(m_pSoundManager, m_pTextureManager, false);

	m_Jumped = false;
	m_Attacked = false;
	m_Timer = 150.f;
	m_TimePaused = false;
	m_TimePausedTimer = 5.f;
	m_TimePausedTimerInt = 4;

	m_Respawining = false;
	m_RespawnTimer = 3.f;
	m_GameOver = false;
	m_GameOverTimer = 3.f;

	m_GameOverTextureStr = "GAME OVER";
	m_pTextureManager->LoadTexture(m_GameOverTextureStr, "resources/fonts/font.ttf", 8, Color4f{ 1,1,1,1 });

	m_GameOverSoundStr = "resources/sounds/soundeffects/gameover.mp3";
	m_pSoundManager->LoadEffect(m_GameOverSoundStr);

	m_TimePausedSoundStr = "resources/sounds/soundeffects/timepaused.wav";
	m_pSoundManager->LoadEffect(m_TimePausedSoundStr);

	m_IsInBossRoom = false;
}

void Game::Cleanup( )
{
	delete m_pLevel;
	delete m_pAvatar;
	delete m_pCamera;
	delete m_pEnemyManager;
	delete m_pProjectileManager;
	delete m_pHUD;
	delete m_pPowerupManager;
	delete m_pTextureManager;
	delete m_pSoundManager;
}

void Game::Update( float elapsedSec )
{
	if (!m_GameOver)
	{
		if (!m_Respawining)
		{
			if (m_TimePaused)
			{
				m_TimePausedTimer -= elapsedSec;

				if (m_TimePausedTimer <= m_TimePausedTimerInt)
				{
					m_TimePausedTimerInt--;
					m_pSoundManager->PlayEffect(m_TimePausedSoundStr);
				}

				if (m_TimePausedTimer <= 0)
				{
					m_TimePausedTimer = 5.f;
					m_TimePausedTimerInt = 4;
					m_TimePaused = false;
				}
			}

			Rectf cameraWindow{ m_pCamera->GetWindow(m_pAvatar->GetShape()) };
			m_Timer -= elapsedSec;
			if (m_Timer <= 0)
			{
				m_pAvatar->Die();
				m_Timer = 150.f;
			}

			// Avatar update
			m_pAvatar->Update(elapsedSec, m_pLevel);
			if (m_pAvatar->CheckIfDead())
			{
				if (m_pAvatar->GetLives() == 0)
				{
					m_GameOver = true;
				}
				m_Respawining = true;

				m_pSoundManager->PauseStream();
				m_pSoundManager->PlayEffect(m_GameOverSoundStr);
			}
			
			// Powerup updates
			if (m_pPowerupManager->Update(elapsedSec, cameraWindow, m_pLevel, m_pAvatar)) m_TimePaused = true;

			// Enemy updates
			if (m_TimePaused)
			{
				m_pEnemyManager->Update(elapsedSec, m_pLevel, cameraWindow, m_pAvatar, m_pProjectileManager, true);
			}
			else
			{
				m_pEnemyManager->Update(elapsedSec, m_pLevel, cameraWindow, m_pAvatar, m_pProjectileManager, false);
			}

			// Projectile Updates
			m_pProjectileManager->Update(elapsedSec, cameraWindow, m_pEnemyManager->GetEnemies(), m_pAvatar, m_pPowerupManager->GetPowerups());

			// HUD update
			m_pHUD->Update(m_Timer, m_pAvatar->GetScore(), m_pAvatar->GetHP(), m_pAvatar->GetLives(), m_pEnemyManager->GetHP(), m_pAvatar->GetEnergy(), m_pAvatar->GetPowerup());
	

 			CheckAdvanceLevel();
		}
		else
		{
			m_RespawnTimer -= elapsedSec;
			if (m_RespawnTimer <= 0.f)
			{
				GameReset();
			}
		}

	}
	else
	{
		m_GameOverTimer -= elapsedSec;
	}
}

void Game::Draw( ) const
{
	ClearBackground();
	if (!m_GameOver || m_GameOverTimer >= 0.f)
	{
		ClearBackground();
		glPushMatrix();
		m_pCamera->Transform(m_pAvatar->GetShape());

		m_pLevel->DrawBackground();
		if (m_GameOver)
		{
			DrawColorOverlays();
		}

		m_pPowerupManager->Draw();
		m_pEnemyManager->Draw();
		m_pAvatar->Draw();
		m_pProjectileManager->Draw();



		glPopMatrix();

		m_pHUD->Draw(m_pTextureManager->GetTexture("resources/images/powerups.png"));
	}
	else
	{
		Texture* gameOverTexture = m_pTextureManager->GetTexture(m_GameOverTextureStr);
		Rectf drawRect{ 256 / 2 - gameOverTexture->GetWidth() / 2, 208 / 2 - gameOverTexture->GetHeight() / 2, gameOverTexture->GetWidth() , gameOverTexture->GetHeight()};
		gameOverTexture->Draw(drawRect);
	}
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	switch ( e.keysym.sym )
	{
	case SDLK_x:
	{
		if (!m_Jumped)
		{
			m_pAvatar->Jump();
			m_Jumped = true;
		}
		break;
	}
	case SDLK_z:
	{
		if (!m_Attacked)
		{
			const Uint8* pStates = SDL_GetKeyboardState(nullptr);
			if (pStates[SDL_SCANCODE_UP])
			{
				m_pAvatar->UseItem(m_pProjectileManager);
			}
			else
			{
				m_pAvatar->Attack();
			}
			m_Attacked = true;
			break;
		}
	}
	case SDLK_RETURN:
	{
		if (m_GameOver && m_GameOverTimer <= 0.f)
		{
			FullGameReset();
		}
		break;
	}
	case SDLK_i :
	{

		std::cout << "Arrows  to move\n";
		std::cout << "Z to attack\n";
		std::cout << "X to jump\n";
		std::cout << "Up arrow and attack to use item\n";
		std::cout << "Slash lanterns for items to drop\n\n";

		break;
	}
	case SDLK_p:
	{
		m_pAvatar->SetPos(Point2f{ 2900,0 });
		break;
	}
	}
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	switch ( e.keysym.sym )
	{
	case SDLK_x:
	{
		m_Jumped = false;
		break;
	}
	case SDLK_z:
	{
		m_Attacked = false;
		break;
	}
	}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::DrawColorOverlays() const
{
	Color4f color;
	switch (int(m_GameOverTimer * 6) % 3)
	{
	case 0:
		color = Color4f{ 0.8f,0,0,0.4f };
		break;
	case 1:
		color = Color4f{ 0,0.8f,0,0.4f };
		break;
	case 2:
		color = Color4f{ 0,0,0.8f,0.4f };
		break;
	}

	utils::SetColor(color);
	Rectf window{ m_pCamera->GetWindow(m_pAvatar->GetShape()) };
	window.height = 208 * 3;
	utils::FillRect(window);
}

void Game::GameReset()
{
	m_RespawnTimer = 3.f;
	m_Timer = 150.f;

	delete m_pLevel;
	delete m_pPowerupManager;
	delete m_pEnemyManager;
	delete m_pHUD;

	m_pLevel = new Level(m_pSoundManager, m_pTextureManager, false);
	m_pPowerupManager = new PowerupManager(m_pSoundManager, m_pTextureManager, false);
	m_pEnemyManager = new EnemyManager(m_pSoundManager, m_pTextureManager, false);
	m_pHUD = new HUD(m_pTextureManager, false);


	m_pAvatar->Reset();
	m_pCamera->SetLevelBoundaries(Rectf{ 0,0,m_pLevel->GetLevelWidth(),0 });

	m_Respawining = false;
}

void Game::FullGameReset()
{
	m_RespawnTimer = 3.f;
	m_GameOverTimer = 3.f;
	m_Timer = 150.f;

	delete m_pLevel;
	delete m_pPowerupManager;
	delete m_pEnemyManager;
	delete m_pHUD;

	m_pLevel = new Level(m_pSoundManager, m_pTextureManager, false);
	m_pPowerupManager = new PowerupManager(m_pSoundManager, m_pTextureManager, false);
	m_pEnemyManager = new EnemyManager(m_pSoundManager, m_pTextureManager, false);
	m_pHUD = new HUD(m_pTextureManager, false);

	m_pAvatar->FullReset();
	m_pCamera->SetLevelBoundaries(Rectf{ 0,0,m_pLevel->GetLevelWidth(),0 });

	m_Respawining = false;
	m_GameOver = false;
}

void Game::AdvanceLevel()
{
	delete m_pLevel;
	delete m_pEnemyManager;
	delete m_pPowerupManager;
	delete m_pHUD;

	m_pLevel = new Level(m_pSoundManager, m_pTextureManager, true);
	m_pEnemyManager = new EnemyManager(m_pSoundManager, m_pTextureManager, true);
	m_pPowerupManager = new PowerupManager(m_pSoundManager, m_pTextureManager, true);
	m_pHUD = new HUD(m_pTextureManager, true);

	m_pAvatar->SetPos(Point2f{ 20,20 });
	m_pCamera->SetLevelBoundaries(Rectf{ 0,0,m_pLevel->GetLevelWidth(),0 });

	m_Timer = 150.f;
}

void Game::CheckAdvanceLevel()
{
	if (!m_pLevel->GetIsBossRoom())
	{
		if (m_pAvatar->GetShape().left >= 3030)
		{
			AdvanceLevel();
		}
	}
}
