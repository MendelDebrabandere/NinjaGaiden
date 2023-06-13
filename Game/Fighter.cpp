#include "pch.h"
#include "Fighter.h"
#include "Texture.h"
#include "Level.h"
#include "Avatar.h"
#include "TextureManager.h"
#include "SoundManager.h"

int Fighter::s_InstanceCounter{ 0 };

Fighter::Fighter(SoundManager* SM, TextureManager* TM, const Point2f& spawnPos)
	: m_AttackCooldown{0.37f}
{
	m_pTM = TM;
	m_pSM = SM;
	m_TextureString = "resources/images/fighter.png";
	m_pTM->LoadTexture(m_TextureString);
	m_pSM->LoadEffect(m_DeathSoundStr);
	m_SpawnPos = spawnPos;
	m_Shape = Rectf{ -500, 0, m_pTM->GetTexture(m_TextureString)->GetWidth() / 4, m_pTM->GetTexture(m_TextureString)->GetHeight() };
	m_NrFramesPerSec = 4;
	m_NrOfFrames = 2;
	s_InstanceCounter++;
	m_pTM->LoadTexture("resources/images/deathexplosion.png");
	m_WalkSpeed = 30.f;
}

Fighter::~Fighter()
{
	s_InstanceCounter--;
	if (s_InstanceCounter == 0)
	{
		m_pTM->DeleteTexture(m_TextureString);
	}
}

void Fighter::Draw() const
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
	case m_ActionState::attacking:
	{
		Rectf srcRect;
		srcRect.width = m_Shape.width;
		srcRect.height = m_Shape.height;
		srcRect.bottom = srcRect.height;
		srcRect.left = srcRect.width;

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

		break;
	}
	}
}

void Fighter::Update(float elapsedSec, const Level* pLevel, const Rectf& cameraWindow, Avatar* pAvatar, ProjectileManager* pPManager, bool timePaused)
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
		CheckSpawning(cameraWindow, m_SpawnPos, pAvatar);


		switch (m_CurrentState)
		{
		case m_ActionState::moving:
		{
			if (m_IsInFrame && !m_IsDead)
			{
				// DoFalling
				// FallAccel = -500
				m_Velocity.y += -500 * elapsedSec;

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
				m_Shape.bottom += m_Velocity.y * elapsedSec;

				pLevel->HandleHorizontalCollision(m_Shape, m_Velocity);

				if (pLevel->HandleVerticalCollision(m_Shape, m_Velocity))
				{
					m_Velocity.y = 50;
				}

				// Check attack
				if (m_AttackCooldown <= 0)
				{
					Rectf AvatarShape{ pAvatar->GetShape() };
					float distance{ AvatarShape.left + AvatarShape.width / 2 - (m_Shape.left + m_Shape.width / 2) };
					if ((distance <= 25) && (distance >= 0))
					{
						Attack(false);
					}
					else if ((distance >= -30) && (distance <= 0))
					{
						Attack(true);
					}

					// Look at the player
					LookAtPlayer(pAvatar);
				}
				else
				{
					m_AttackCooldown -= elapsedSec;
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
			m_Shape.width = m_pTM->GetTexture(m_TextureString)->GetWidth() / 4;
			break;
		}
		case m_ActionState::attacking:
		{
			// DoFalling
			// FallAccel = -800
			m_Velocity.y += -800 * elapsedSec;

			// Hit detection
			HandleHitboxes(pAvatar, true);

			CheckHP();
			if (m_CurrentState == m_ActionState::dying)
			{
				pAvatar->AddScore(200);
			}

			// Enemy updates (movement)
			float horizontalAttackSpeed{ 120 };
			m_Velocity.x = horizontalAttackSpeed;
			if (!m_FacingRight)
			{
				if (m_Velocity.x > 0)
				{
					m_Velocity.x = -horizontalAttackSpeed;
				}
			}

			m_Shape.left += m_Velocity.x * elapsedSec;
			m_Shape.bottom += m_Velocity.y * elapsedSec;

			pLevel->HandleHorizontalCollision(m_Shape, m_Velocity);

			if (pLevel->HandleVerticalCollision(m_Shape, m_Velocity))
			{
				m_CurrentState = m_ActionState::moving;
				m_Shape.width /= 2;
				m_Shape.left += m_Shape.width;
				LookAtPlayer(pAvatar);
			}

			break;
		}
		}
	}

}

void Fighter::Attack(bool rightDirection)
{
	m_AttackCooldown = 0.37f;
	m_Velocity.y = 100;
	m_CurrentState = m_ActionState::attacking;
	m_Shape.left -= m_Shape.width;
	m_Shape.width *= 2;
}

