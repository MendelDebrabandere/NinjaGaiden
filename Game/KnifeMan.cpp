#include "pch.h"
#include "KnifeMan.h"
#include "Texture.h"
#include "Level.h"
#include "Avatar.h"
#include "ProjectileManager.h"
#include "TextureManager.h"
#include "SoundManager.h"

int KnifeMan::s_InstanceCounter{ 0 };

KnifeMan::KnifeMan(SoundManager* SM, TextureManager* TM, const Point2f& spawnPos)
	: m_NextAttackChrouch{false}
	, m_SecondsTillNextAttack{2.f}
{
	m_pTM = TM;
	m_pSM = SM;
	m_TextureString = "resources/images/knifeman.png";
	m_pTM->LoadTexture(m_TextureString);
	m_pSM->LoadEffect(m_DeathSoundStr);
	m_SpawnPos = spawnPos;
	m_Shape = Rectf{ -500, 0, m_pTM->GetTexture(m_TextureString)->GetWidth() / 6, m_pTM->GetTexture(m_TextureString)->GetHeight() };
	m_NrFramesPerSec = 3;
	m_NrOfFrames = 2;
	s_InstanceCounter++;
	m_pTM->LoadTexture("resources/images/deathexplosion.png");
	m_WalkSpeed = 30;
}

KnifeMan::~KnifeMan()
{
	s_InstanceCounter--;
	if (s_InstanceCounter == 0)
	{
		m_pTM->DeleteTexture(m_TextureString);
	}
}

void KnifeMan::Draw() const
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
	case m_ActionState::crouchAttacking:
	{
		Rectf srcRect;
		srcRect.width = m_Shape.width;
		srcRect.height = m_Shape.height;
		srcRect.left = (4 + m_AnimFrame) * srcRect.width;
		srcRect.bottom = srcRect.height;

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
	case m_ActionState::attacking:
	{
		Rectf srcRect;
		srcRect.width = m_Shape.width;
		srcRect.height = m_Shape.height;
		srcRect.left = (2 + m_AnimFrame) * srcRect.width;
		srcRect.bottom = srcRect.height;

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

void KnifeMan::Update(float elapsedSec, const Level* pLevel, const Rectf& cameraWindow, Avatar* pAvatar, ProjectileManager* pPManager, bool timePaused)
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
					pAvatar->AddScore(200);
				}

				// Sprite animation
				m_AnimTime += elapsedSec;
				if (m_AnimTime >= 1.f / m_NrFramesPerSec)
				{
					m_AnimTime -= 1.f / m_NrFramesPerSec;
					++m_AnimFrame %= m_NrOfFrames;
				}

				// Attack time update
				m_SecondsTillNextAttack -= elapsedSec;
				if (m_SecondsTillNextAttack <= 0)
				{
					Attack(pAvatar, pPManager);
				}

			}
			break;
		}
		case m_ActionState::dying:
		{
			UpdateExplosion(elapsedSec);
			break;
		}
		case m_ActionState::crouchAttacking:
		case m_ActionState::attacking:
		{
			m_SecondsTillNextAttack -= elapsedSec;
			if (m_SecondsTillNextAttack <= 0)
			{
				m_SecondsTillNextAttack = 2.5f;
				m_CurrentState = m_ActionState::moving;
			}

			// Sprite animation
			m_AnimTime += elapsedSec;
			if (m_AnimTime >= 1.f / m_NrFramesPerSec)
			{
				m_AnimTime -= 1.f / m_NrFramesPerSec;
				++m_AnimFrame %= m_NrOfFrames;
			}
			break;
		}
		}
	}
}

void KnifeMan::Attack(const Avatar* pAvatar, ProjectileManager* pPManager)
{
	m_AnimTime = 0.f;
	m_AnimFrame = 0;

	// Look at the player
	LookAtPlayer(pAvatar);

	if (m_NextAttackChrouch)
	{
		m_CurrentState = m_ActionState::crouchAttacking;
		pPManager->RequestKnife(Point2f{ m_Shape.left + m_Shape.width / 2, m_Shape.bottom + m_Shape.height / 3 }, m_FacingRight);
	}
	else
	{
		m_CurrentState = m_ActionState::attacking;
		pPManager->RequestKnife(Point2f{ m_Shape.left + m_Shape.width / 2, m_Shape.bottom + m_Shape.height / 2 }, m_FacingRight);
	}

	m_NextAttackChrouch = !m_NextAttackChrouch;
	m_SecondsTillNextAttack = 0.666f;


}

