#include "pch.h"
#include "Dog.h"
#include "Texture.h"
#include "Level.h"
#include "Avatar.h"
#include "TextureManager.h"
#include "SoundManager.h"

int Dog::s_InstanceCounter{ 0 };

Dog::Dog(SoundManager* SM, TextureManager* TM, const Point2f& spawnPos, bool spawnBehindYou)
	: m_Spawned{ false }
	, m_SpawnBehindYou{ spawnBehindYou }
{
	m_pTM = TM;
	m_pSM = SM;
	m_TextureString = "resources/images/dog.png";
	m_pTM->LoadTexture(m_TextureString);
	m_pSM->LoadEffect(m_DeathSoundStr);
	m_SpawnPos = spawnPos;
	m_Shape = Rectf{ -500, 0, m_pTM->GetTexture(m_TextureString)->GetWidth() / 2, m_pTM->GetTexture(m_TextureString)->GetHeight() };
	m_Velocity = Vector2f{ m_WalkSpeed, 0 };
	m_FacingRight = true;
	m_NrFramesPerSec = 5;
	m_NrOfFrames = 2;
	s_InstanceCounter++;
	m_pTM->LoadTexture("resources/images/deathexplosion.png");
	m_WalkSpeed = 105.f;
}

Dog::~Dog()
{
	s_InstanceCounter--;
	if (s_InstanceCounter == 0)
	{
		m_pTM->DeleteTexture(m_TextureString);
	}
}

void Dog::Draw() const
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

			srcRect.left = srcRect.width * m_AnimFrame;

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

void Dog::Update(float elapsedSec, const Level* pLevel, const Rectf& cameraWindow, Avatar* pAvatar, ProjectileManager* pPManager, bool timePaused)
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
				pAvatar->AddScore(200);
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
		if (m_SpawnBehindYou)
		{
			CheckDogSpawning(cameraWindow, m_SpawnPos, pAvatar);
		}
		else
		{
			CheckSpawning(cameraWindow, m_SpawnPos, pAvatar);
		}

		switch (m_CurrentState)
		{
		case m_ActionState::moving:
		{
			if (m_IsInFrame && !m_IsDead)
			{
				// DoFalling
				// FallAccel = -800
				m_Velocity.y += -800 * elapsedSec;

				// Enemy updates (movement)
				m_Velocity.x = m_WalkSpeed;
				if (!m_FacingRight)
				{
					if (m_Velocity.x > 0)
					{
						m_Velocity.x = -m_WalkSpeed;
					}
				}
				Vector2f velocityCopy{ m_Velocity };
				Rectf shapeCopy{ m_Shape };
				if (pLevel->BounceOfWall(shapeCopy, velocityCopy))
				{
					m_FacingRight = !m_FacingRight;
				}

				m_Shape.left += m_Velocity.x * elapsedSec;
				m_Shape.bottom += m_Velocity.y * elapsedSec;



				if (pLevel->HandleVerticalCollision(m_Shape, m_Velocity))
				{
					// Jump Speed
					m_Velocity.y = 130;
				}

				// Hit detection
				HandleHitboxes(pAvatar, true);

				CheckHP();
				if (m_CurrentState == m_ActionState::dying)
				{
					pAvatar->AddScore(200);
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


void Dog::CheckDogSpawning(const Rectf& cameraWindow, const Point2f& spawnPos, const Avatar* pAvatar)
{
	float spawnMargin = cameraWindow.width / 2 - 37;
	Rectf avatarRect{ pAvatar->GetShape() };
	bool avatarFacingRight{ pAvatar->GetFacingRight() };

	if (m_Shape.left >= cameraWindow.left && m_Shape.left + m_Shape.width <= cameraWindow.left + cameraWindow.width)
	{
		if (avatarFacingRight)
		{
			if (m_Shape.left + m_Shape.width < cameraWindow.left + cameraWindow.width / 2 - spawnMargin)
			{
				m_IsInFrame = true;
			}
		}
		else
		{
			if (m_Shape.left > cameraWindow.left + cameraWindow.width / 2 + spawnMargin)
			{
				m_IsInFrame = true;
			}
		}
	}


	else
	{
		m_IsDead = true;
		m_IsInFrame = false;
		m_Shape.left = -500;
	}

	if (!(spawnPos.x >= cameraWindow.left && spawnPos.x + m_Shape.width <= cameraWindow.left + cameraWindow.width))
	{
		if (!m_IsInFrame)
		{
			if (m_CurrentState != m_ActionState::dying)
			{
				m_IsDead = false;
				m_HP = 1;
				m_Shape.left = spawnPos.x;
				m_Shape.bottom = spawnPos.y;
			}
		}
	}




	if (m_IsInFrame && !m_IsDead)
	{
		if (!m_Spawned)
		{
			m_FacingRight = avatarFacingRight;
			m_Spawned = true;
		}
	}
	else
	{
		m_Spawned = false;
	}

}
