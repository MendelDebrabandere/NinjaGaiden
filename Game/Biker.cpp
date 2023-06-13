#include "pch.h"
#include "Biker.h"
#include "Texture.h"
#include "Level.h"
#include "Avatar.h"
#include "TextureManager.h"
#include "SoundManager.h"

int Biker::s_InstanceCounter{ 0 };

Biker::Biker(SoundManager* SM, TextureManager* TM, const Point2f& spawnPos)
{
	m_pTM = TM;
	m_pSM = SM;
	m_TextureString = "resources/images/biker.png";
	m_pTM->LoadTexture(m_TextureString);
	m_pSM->LoadEffect(m_DeathSoundStr);
	m_SpawnPos = spawnPos;
	m_Shape = Rectf{ -500, 0, m_pTM->GetTexture(m_TextureString)->GetWidth() / 3, m_pTM->GetTexture(m_TextureString)->GetHeight()};
	m_NrFramesPerSec = 3;
	m_NrOfFrames = 4;
	s_InstanceCounter++;
	m_pTM->LoadTexture("resources/images/deathexplosion.png");
	m_WalkSpeed = 30.f;
}

Biker::~Biker()
{
	s_InstanceCounter--;
	if (s_InstanceCounter == 0)
	{
		m_pTM->DeleteTexture(m_TextureString);
	}
}

void Biker::Draw() const
{
	switch (m_CurrentState)
	{
	case m_ActionState::moving:
	{
		if (m_IsInFrame && !m_IsDead)
		{
			Rectf srcRect;
			srcRect.width = m_Shape.width;
			srcRect.height = m_Shape.height;
			srcRect.bottom = srcRect.height;

			if (m_AnimFrame == 3)
			{
				srcRect.left = 2 * srcRect.width;
			}
			else
			{
				srcRect.left = (m_AnimFrame % 2) * srcRect.width;
			}


			if (!m_FacingRight)
			{
				glPushMatrix();
				glTranslatef(2 * m_Shape.left + m_Shape.width, 0, 0);
				glScalef(-1, 1, 1);
				m_pTM->GetTexture(m_TextureString)->Draw(m_Shape, srcRect);
				glPopMatrix();
			}
			else
			{
				m_pTM->GetTexture(m_TextureString)->Draw(m_Shape, srcRect);
			}
		}
		break;
	}
	case m_ActionState::dying:
	{
		DrawExplosion();
		break;
	}
	}
}

void Biker::Update(float elapsedSec, const Level* pLevel, const Rectf& cameraWindow, Avatar* pAvatar, ProjectileManager* pPManager, bool timePaused)
{
	if (timePaused)
	{
		switch (m_CurrentState)
		{
		case m_ActionState::moving:
		case m_ActionState::attacking:
		case m_ActionState::crouchAttacking:
		{
			HandleHitboxes(pAvatar, true);
			CheckHP();
			if (m_CurrentState == m_ActionState::dying)
			{
				pAvatar->AddScore(100);
			}
			break;
		}
		case m_ActionState::dying:
		{
			UpdateExplosion(elapsedSec);
			break;
		}
		}
	}

	else
	{
		// Check if you should draw and update the enemy
		CheckSpawning(cameraWindow, m_SpawnPos, pAvatar);


		switch (m_CurrentState)
		{
		case m_ActionState::moving:
		{
			if (m_IsInFrame && !m_IsDead)
			{
				// Enemy updates (movement)
				m_Velocity.x = m_WalkSpeed;
				if (!m_FacingRight)
				{
					if (m_Velocity.x > 0)
					{
						m_Velocity.x = -m_WalkSpeed;
					}
				}
				m_Shape.left += m_Velocity.x * elapsedSec;

				pLevel->HandleHorizontalCollision(m_Shape, m_Velocity);

				switch (pLevel->IsFullEnemyOnPlatform(m_Shape))
				{
				case 1:
					m_FacingRight = true;
					break;
				case 2:
					m_FacingRight = false;
					break;
				}

				// Hit detection
				HandleHitboxes(pAvatar, true);

				CheckHP();
				if (m_CurrentState == m_ActionState::dying)
				{
					pAvatar->AddScore(100);
				}

				// Sprite animation
				m_AnimTime += elapsedSec;
				if (m_AnimTime >= 1.f / m_NrFramesPerSec)
				{
					m_AnimTime -= 1.f / m_NrFramesPerSec;
					++m_AnimFrame %= m_NrOfFrames;
				}
			}
			break;
		}
		case m_ActionState::dying:
		{
			UpdateExplosion(elapsedSec);
			break;
		}
		}
	}
}
