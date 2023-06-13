#include "pch.h"
#include "Enemy.h"
#include "utils.h"
#include "Avatar.h"
#include "Texture.h"
#include "TextureManager.h"
#include "SoundManager.h"

int Enemy::s_InstanceCounter{ 0 };

Enemy::Enemy()
	: m_IsInFrame{false}
	, m_IsDead{false}
	, m_HP{1}
	, m_CurrentState{m_ActionState::moving}
	, m_FacingRight{false}
	, m_AnimTime{0.f}
	, m_AnimFrame{}
	, m_SpawnPos{Point2f{0,0}}
	, m_Velocity{-30,0}
	, m_pTM{nullptr}
	, m_pSM{nullptr}
	, m_WalkSpeed{}
	, m_TextureString{}
	, m_NrFramesPerSec{}
	, m_NrOfFrames{}
	, m_ExplosionTextureStr{"resources/images/deathexplosion.png"}
	, m_DeathSoundStr{"resources/sounds/soundeffects/EnemyDeath.wav"}
{
	s_InstanceCounter++;
}

Enemy::~Enemy()
{
	s_InstanceCounter--;
	if (!s_InstanceCounter)
	{
		m_pSM->DeleteEffect(m_DeathSoundStr);
	}
}

void Enemy::HandleHitboxes(Avatar* pAvatar, bool canTakeDamage)
{
	Rectf avatarShape{ pAvatar->GetShape() };

	Rectf hitBox{ m_Shape };
	hitBox.left += 8;
	hitBox.width -= 13;
	hitBox.height -= 8;
	if (m_CurrentState == m_ActionState::crouchAttacking)
	{
		hitBox.height -= 3;
	}

	if (canTakeDamage)
	{
		if (pAvatar->IsAttacking())
		{
			Rectf swordHitBox;
			if (pAvatar->GetFacingRight())
			{
				swordHitBox = Rectf{ avatarShape.left + avatarShape.width, avatarShape.bottom + avatarShape.width / 2, avatarShape.width, avatarShape.height / 2 };
			}
			else
			{
				swordHitBox = Rectf{ avatarShape.left - avatarShape.width, avatarShape.bottom, avatarShape.width, avatarShape.height };
			}
			if (utils::IsOverlapping(swordHitBox, hitBox))
			{
				m_HP--;
			}
		}
		else if (pAvatar->IsCrouchAttacking())
		{
			Rectf swordHitBox;
			if (pAvatar->GetFacingRight())
			{
				swordHitBox = Rectf{ avatarShape.left + avatarShape.width, avatarShape.bottom, avatarShape.width, avatarShape.height * 2 / 3 };
			}
			else
			{
				swordHitBox = Rectf{ avatarShape.left - avatarShape.width, avatarShape.bottom, avatarShape.width, avatarShape.height };
			}
			if (utils::IsOverlapping(swordHitBox, hitBox))
			{
				m_HP--;
			}
		}
		if (pAvatar->IsjumpSlashing())
		{
			Rectf jumpSlashBox{ avatarShape.left - 5, avatarShape.bottom - 5, avatarShape.width + 10, avatarShape.height + 10 };
			if (utils::IsOverlapping(hitBox, jumpSlashBox))
			{
				m_HP--;
			}
		}
		if (pAvatar->IsFireWheelOn())
		{
			Rectf fireWheelRect{ avatarShape.left - 8,
						avatarShape.bottom - 4,
						avatarShape.width + 16,
						avatarShape.height + 8 };
			if (utils::IsOverlapping(fireWheelRect, m_Shape))
			{
				m_HP--;
			}
		}
	}

	if (!pAvatar->IsjumpSlashing())
	{
		if (utils::IsOverlapping(hitBox, avatarShape))
		{
			pAvatar->TakeDamage(m_Shape.left + m_Shape.width / 2 >= avatarShape.left + avatarShape.width / 2);
		}
	}
}

void Enemy::CheckHP()
{
	if (m_HP <= 0)
	{
		m_pSM->PlayEffect(m_DeathSoundStr);
		m_HP = 1;
		m_IsDead = true;
		m_IsInFrame = false;
		m_CurrentState = m_ActionState::dying;
		m_AnimTime = 0.f;
		m_AnimFrame = 0;
	}
}

void Enemy::CheckSpawning(const Rectf& cameraWindow, const Point2f& spawnPos, const Avatar* pAvatar)
{

	if (m_Shape.left >= cameraWindow.left && m_Shape.left + m_Shape.width <= cameraWindow.left + cameraWindow.width)
	{
		if (pAvatar->GetFacingRight())
		{
			if (m_Shape.left > pAvatar->GetShape().left)
			{
				m_IsInFrame = true;
			}
		}
		else
		{
			if (m_Shape.left < pAvatar->GetShape().left)
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


				if (m_Shape.left > pAvatar->GetShape().left)
				{
					m_FacingRight = false;
				}


				if (m_Shape.left < pAvatar->GetShape().left)
				{
					m_FacingRight = true;
				}
			}
		}
	}
}

void Enemy::DrawExplosion() const
{
	Rectf dstRect{};
	dstRect.width = 50;
	dstRect.height = 50;
	dstRect.left = m_Shape.left + m_Shape.width / 2 - dstRect.width / 2;
	dstRect.bottom = m_Shape.bottom + m_Shape.height / 2 - dstRect.height / 2;

	Rectf srcRect;
	srcRect.width = 50;
	srcRect.height = 50;
	srcRect.left = m_AnimFrame * srcRect.width;
	srcRect.bottom = srcRect.height;

	m_pTM->GetTexture(m_ExplosionTextureStr)->Draw(dstRect, srcRect);
}

void Enemy::UpdateExplosion(float elapsedSec)
{
	int nrFramesPerSec{ 25 };

	m_AnimTime += elapsedSec;
	if (m_AnimTime >= 0.1f / nrFramesPerSec)
	{
		m_AnimTime -= 1.f / nrFramesPerSec;
		++m_AnimFrame;
	}

	if (m_AnimFrame >= 5)
	{
		m_Shape.left = -500;
		m_AnimTime = 0.f;
		m_AnimFrame = 0;
		m_CurrentState = m_ActionState::moving;
	}
}

void Enemy::LookAtPlayer(const Avatar* pAvatar)
{
	if (pAvatar->GetShape().left < m_Shape.left)
	{
		m_FacingRight = false;
	}
	else
	{
		m_FacingRight = true;
	}
}

Rectf Enemy::GetShape() const
{
	return m_Shape;
}

void Enemy::TakeDamage()
{
	m_HP--;
}

int Enemy::GetHP() const
{
	if (m_CurrentState == m_ActionState::dying || m_IsDead)
	{
		return 0;
	}
	else
	{
		return m_HP;
	}
}