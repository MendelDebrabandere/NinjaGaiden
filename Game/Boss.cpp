#include "pch.h"
#include "Boss.h"
#include "Texture.h"
#include "Avatar.h"
#include "Level.h"
#include "TextureManager.h"
#include "SoundManager.h"

Boss::Boss(SoundManager* SM, TextureManager* TM, const Point2f& spawnPos)
	: m_AttackCooldown{1.5f}
	, m_TimesAttacked{0}
	, m_InvincibilityCooldown{0.f}
	, m_BossExplosionTimer{ 0.f }
	, m_ExplosionTextureString{"resources/images/bossexplosion.png"}
	, m_ExplosionSoundString{"resources/sounds/soundeffects/bossdeath.wav"}
	, m_NrOfFrames{ 2 }
	, m_NrFramesPerSec{ 2 }
{
	m_pTM = TM;
	m_pSM = SM;

	m_TextureString = std::string("resources/images/Boss.png");
	TM->LoadTexture(m_TextureString);
	TM->LoadTexture(m_ExplosionTextureString);

	SM->LoadEffect(m_DeathSoundStr);
	SM->LoadEffect(m_ExplosionSoundString);

	m_SpawnPos = spawnPos;
	
	m_Shape = Rectf{ spawnPos.x, spawnPos.y, m_pTM->GetTexture(m_TextureString)->GetWidth() / 5, m_pTM->GetTexture(m_TextureString)->GetHeight() / 2};
	m_FacingRight = false;
	m_HP = 16;
}

Boss::~Boss()
{
	m_pTM->DeleteTexture(m_TextureString);
	m_pTM->DeleteTexture(m_ExplosionTextureString);
	m_pSM->DeleteEffect(m_ExplosionSoundString);
}

void Boss::Draw() const
{
	switch (m_CurrentState)
	{
	case m_ActionState::moving:
	{
		if (!m_IsDead)
		{
			Rectf dstRect{ m_Shape };
			Rectf srcRect;
			srcRect.height = m_Shape.height;
			srcRect.bottom = srcRect.height;
			srcRect.left = m_AnimFrame * m_Shape.width;

			if (m_AnimFrame == 1)
			{
				srcRect.width = 2 * m_Shape.width;
				dstRect.width *= 2;
			}
			else
			{
				srcRect.width = m_Shape.width;
			}

			if (!m_FacingRight)
			{
				glPushMatrix();
				glTranslatef(2 * m_Shape.left + m_Shape.width, 0, 0);
				glScalef(-1, 1, 1);
				m_pTM->GetTexture(m_TextureString)->Draw(dstRect, srcRect);
				glPopMatrix();
			}
			else
			{
				m_pTM->GetTexture(m_TextureString)->Draw(dstRect, srcRect);
			}
		}
		break;
	}
	case m_ActionState::attacking:
	{
		Rectf srcRect;
		srcRect.width = m_Shape.width;
		srcRect.height = m_Shape.height;
		srcRect.bottom = 2 * srcRect.height;

		if (m_AnimFrame == 2)
		{
			srcRect.left = 90;
		}
		else if (m_AnimFrame == 1)
		{
			srcRect.left = 30;
		}
		else
		{
			srcRect.left = 0;
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
		break;
	}
	case m_ActionState::dying:
	{
		Rectf srcRect;
		srcRect.height = m_Shape.height;
		srcRect.bottom = srcRect.height;
		srcRect.left = 0;
		srcRect.width = m_Shape.width;

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

		DrawBossExplosion();
		break;
	}
	}
}

void Boss::Update(float elapsedSec, const Level* pLevel, const Rectf& cameraWindow, Avatar* pAvatar, ProjectileManager* pPManager, bool timePaused)
{
	int lastFrameHP{ m_HP };
	switch (m_CurrentState)
	{
	case m_ActionState::moving:
	{
		if (!m_IsDead)
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

			if (m_InvincibilityCooldown <= 0)
			{
				HandleHitboxes(pAvatar, true);
			}
			else
			{
				HandleHitboxes(pAvatar, false);
			}

			CheckHP();
			if (m_CurrentState == m_ActionState::dying)
			{
				pAvatar->AddScore(5000);
				m_pSM->PlayEffect(m_ExplosionSoundString);
			}

			m_NrFramesPerSec = 2;
			m_NrOfFrames = 2;
			// Sprite animation
			m_AnimTime += elapsedSec;
			if (m_AnimTime >= 1.f / m_NrFramesPerSec)
			{
				m_AnimTime -= 1.f / m_NrFramesPerSec;
				++m_AnimFrame %= m_NrOfFrames;
			}

			m_AttackCooldown -= elapsedSec;
			if (m_AttackCooldown <= 0.f)
			{
				m_CurrentState = m_ActionState::attacking;
				m_AttackCooldown = 1.5f;
				m_AnimFrame = 0;
				m_AnimTime = 0.f;
			}
			m_IsDead = false;
		}
		break;
	}
	case m_ActionState::attacking:
	{
		if (!m_IsDead)
		{
			if (m_InvincibilityCooldown <= 0)
			{
				HandleHitboxes(pAvatar, true);
			}
			else
			{
				HandleHitboxes(pAvatar, false);
			}

			CheckHP();
			if (m_CurrentState == m_ActionState::dying)
			{
				pAvatar->AddScore(100);
				m_pSM->PlayEffect(m_ExplosionSoundString);
				ResetShape();
			}

			int lastFrame{ m_AnimFrame };

			m_NrFramesPerSec = 3;
			m_NrOfFrames = 3;
			// Sprite animation
			m_AnimTime += elapsedSec;
			if (m_AnimTime >= 1.f / m_NrFramesPerSec)
			{
				m_AnimTime -= 1.f / m_NrFramesPerSec;
				++m_AnimFrame %= m_NrOfFrames;
			}

			if (lastFrame != m_AnimFrame && lastFrame == 0)
			{
				if (!m_FacingRight) m_Shape.left -= m_Shape.width;
				m_Shape.width *= 2;
			}
			else if (lastFrame != m_AnimFrame && lastFrame == 1)
			{
				if (!m_FacingRight) m_Shape.left += m_Shape.width * 8.f/20;
				m_Shape.width *= 12.f / 20;
			}
			else if (lastFrame != m_AnimFrame && lastFrame == 2)
			{
				if (!m_FacingRight) m_Shape.left += m_Shape.width * 2.f/12;
				m_Shape.width *= 10.f / 12;
				m_TimesAttacked++;
			}

			if (m_TimesAttacked >= 2)
			{
				m_TimesAttacked = 0;
				m_CurrentState = m_ActionState::moving;
				m_AnimFrame = 0;
				m_AnimTime = 0.f;
				LookAtPlayer(pAvatar);
			}
			m_IsDead = false;
		}
		break;
	}
	case m_ActionState::dying:
	{
		UpdateBossExplosion(elapsedSec);
		break;
	}
	}

	if (m_InvincibilityCooldown >= 0)
	{
		m_InvincibilityCooldown -= elapsedSec;
	}

	if (lastFrameHP != m_HP)
	{
		m_InvincibilityCooldown = 0.291f;
	}	
}

void Boss::ResetShape()
{
	m_Shape.width = 30;
	if (!m_FacingRight)
	{
		m_Shape.left += m_Shape.width;
	}
}

void Boss::UpdateBossExplosion(float elapsedSec)
{
	int nrFramesPerSec{ 24 };
	int nrFrames{ 3 };

	m_AnimTime += elapsedSec;
	if (m_AnimTime >= 0.1f / nrFramesPerSec)
	{
		m_AnimTime -= 1.f / nrFramesPerSec;
		++m_AnimFrame %= nrFrames;
	}

	m_BossExplosionTimer += elapsedSec;
	if (m_BossExplosionTimer >= 3.f)
	{
		m_CurrentState = m_ActionState::moving;
		m_IsDead = true;
	}
}

void Boss::DrawBossExplosion() const
{
	if (m_AnimFrame == 2)
	{
		return;
	}
	Rectf dstRect{};
	dstRect.width = 32;
	dstRect.height = 32;
	dstRect.left = m_Shape.left + m_Shape.width / 2 - dstRect.width / 2;
	dstRect.bottom = m_Shape.bottom + m_Shape.height / 2 - dstRect.height / 2;

	Rectf srcRect;
	srcRect.width = 32;
	srcRect.height = 32;
	srcRect.left = m_AnimFrame * srcRect.width;
	srcRect.bottom = srcRect.height;

	m_pTM->GetTexture(m_ExplosionTextureString)->Draw(dstRect, srcRect);
}
