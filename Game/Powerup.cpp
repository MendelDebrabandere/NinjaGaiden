#include "pch.h"
#include "Powerup.h"
#include "Texture.h"
#include "Level.h"
#include "Avatar.h"
#include "Utils.h"
#include "TextureManager.h"
#include "SoundManager.h"

int Powerup::s_InstanceCounter{ 0 };

Powerup::Powerup(SoundManager* SM, TextureManager* TM, Point2f spawnPos, int powerup)
	: m_Power{m_Powerup(powerup)}
	, m_DroppedTimer{5.f}
	, m_Velocity{0,0}
	, m_CurrentState{m_State::lantern}
	, m_AnimFrame{0}
	, m_AnimTime{0.f}
	, m_IsInFrame{false}
	, m_TextureStr{ "resources/images/powerups.png" }
	, m_PickupSoundStr{"resources/sounds/soundeffects/PowerupPickup.wav"}
	, m_pTM{TM}
	, m_pSM{SM}
{	
	if (!s_InstanceCounter)
	{
		m_pTM->LoadTexture(m_TextureStr);
		m_pSM->LoadEffect(m_PickupSoundStr);
	}
	s_InstanceCounter++;

	m_Shape = Rectf{ spawnPos.x,
		spawnPos.y,
		m_pTM->GetTexture(m_TextureStr)->GetWidth() / 12,
		m_pTM->GetTexture(m_TextureStr)->GetHeight() };
}

Powerup::~Powerup()
{
	s_InstanceCounter--;
	if (!s_InstanceCounter)
	{
		//m_pTM->DeleteTexture(m_TextureStr);
		// Doesn't get deleted, stays the entire game in memory because the HUD needs it as well

		m_pSM->DeleteEffect(m_PickupSoundStr);
	}
}

void Powerup::Draw() const
{
	switch (m_CurrentState)
	{
	case m_State::lantern:
	{
		if (m_IsInFrame)
		{
			Rectf srcRect{ m_Shape.width * m_AnimFrame, m_Shape.height, m_Shape.width, m_Shape.height };
			m_pTM->GetTexture(m_TextureStr)->Draw(m_Shape, srcRect);
		}
		break;
	}
	case m_State::dropped:
	{
		Rectf srcRect{ 0, m_Shape.height, m_Shape.width, m_Shape.height };

		srcRect.left = (1 + int(m_Power)) * m_Shape.width;
		m_pTM->GetTexture(m_TextureStr)->Draw(m_Shape, srcRect);
		break;
	}
	}
}

bool Powerup::Update(float elapsedSec, const Rectf& cameraWindow, const Level* pLevel, Avatar* pAvatar)
{
	switch (m_CurrentState)
	{
	case m_State::lantern:
	{
		CheckSpawning(cameraWindow);
		if (m_IsInFrame)
		{
			LanternCollisionCheck(pAvatar);

			m_AnimTime += elapsedSec;
			if (m_AnimTime >= 1.f / m_NrFramesPerSec)
			{
				m_AnimTime -= 1.f / m_NrFramesPerSec;
				++m_AnimFrame %= m_NrOfFrames;
			}
		}
		break;
	}
	case m_State::dropped:
	{
		float fallAccel = -800.f;
		m_Velocity.y += fallAccel * elapsedSec;
		m_Shape.left += m_Velocity.x * elapsedSec;
		m_Shape.bottom += m_Velocity.y * elapsedSec;

		pLevel->HandleVerticalCollision(m_Shape, m_Velocity);

		if (DroppedCollisionCheck(pAvatar))
		{
			return true;
		}

		m_DroppedTimer -= elapsedSec;
		if (m_DroppedTimer <= 0.f)
		{
			m_CurrentState = m_State::gone;
		}
		break;
	}
	}
	return false;
}

void Powerup::LanternCollisionCheck(const Avatar* pAvatar)
{
	Rectf avatarShape{ pAvatar->GetShape() };

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
		if (utils::IsOverlapping(swordHitBox, m_Shape))
		{
			m_CurrentState = m_State::dropped;
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
		if (utils::IsOverlapping(swordHitBox, m_Shape))
		{
			m_CurrentState = m_State::dropped;
		}
	}
	else if (pAvatar->IsjumpSlashing())
	{
		if (utils::IsOverlapping(m_Shape, avatarShape))
		{
			m_CurrentState = m_State::dropped;
		}
	}
}

bool Powerup::DroppedCollisionCheck(Avatar* pAvatar)
{
	Rectf avatarShape{ pAvatar->GetShape() };
	avatarShape.left += 8;
	avatarShape.bottom += 8;
	avatarShape.width -= 16;
	avatarShape.height -= 16;

	Rectf hitBox{ m_Shape.left + 6, m_Shape.bottom + 6, m_Shape.width - 12, m_Shape.height - 12 };

	if (utils::IsOverlapping(hitBox, avatarShape))
	{
		m_CurrentState = m_State::gone;
		m_pSM->PlayEffect(m_PickupSoundStr);
		// Give Powerup
		if (int(m_Power) == 10)
		{
			return true;
		}
		else
		{
			pAvatar->PowerupPickedUp(int(m_Power));
		}
	}
	return false;
}

const Texture* Powerup::GetTexture() const
{
	return m_pTM->GetTexture(m_TextureStr);
}

void Powerup::CheckSpawning(const Rectf& cameraWindow)
{
	if (m_Shape.left >= cameraWindow.left && m_Shape.left + m_Shape.width <= cameraWindow.left + cameraWindow.width)
	{
		m_IsInFrame = true;
	}
	else
	{
		m_IsInFrame = false;
	}
}

Rectf Powerup::GetLanternShape() const
{
	if (m_CurrentState == m_State::lantern)
	{
		if (m_IsInFrame)
		{
			return m_Shape;
		}
	}
	return Rectf{ 0,0,0,0 };
}

void Powerup::Drop()
{
	m_CurrentState = m_State::dropped;
}
