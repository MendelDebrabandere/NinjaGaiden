#include "pch.h"
#include "ThrowingKnife.h"
#include "Texture.h"
#include "utils.h"
#include "Avatar.h"
#include "TextureManager.h"

int ThrowingKnife::s_InstanceCounter{};

ThrowingKnife::ThrowingKnife(TextureManager* TM, Point2f spawnPos)
	: m_Exploding{false}
	, m_AnimFrame{0}
	, m_AnimTime{0.f}
{
	m_pTM = TM;
	m_TextureStr = "resources/images/knife.png";
	m_ExpTextureStr = "resources/images/deathexplosion.png";
	m_pTM->LoadTexture(m_TextureStr);
	m_pTM->LoadTexture(m_ExpTextureStr);
	s_InstanceCounter++;

	Texture* texture{ m_pTM->GetTexture(m_TextureStr) };
	m_Shape = Rectf{ spawnPos.x - texture->GetWidth() / 2, spawnPos.y - texture->GetHeight() / 2,texture->GetWidth(), texture->GetHeight() };
}

ThrowingKnife::~ThrowingKnife()
{
	s_InstanceCounter--;
	if (s_InstanceCounter == 0)
	{
		m_pTM->DeleteTexture(m_TextureStr);
		m_pTM->DeleteTexture(m_ExpTextureStr);
	}
}

void ThrowingKnife::Draw() const
{
	if (!m_Exploding)
	{
		if (m_IsInFrame)
		{
			if (!m_FacingRight)
			{
				glPushMatrix();
				glTranslatef(2 * m_Shape.left + m_Shape.width, 0, 0);
				glScalef(-1, 1, 1);
				m_pTM->GetTexture(m_TextureStr)->Draw(m_Shape);
				glPopMatrix();
			}
			else
			{
				m_pTM->GetTexture(m_TextureStr)->Draw(m_Shape);
			}
		}
	}
	else
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

		m_pTM->GetTexture(m_ExpTextureStr)->Draw(dstRect, srcRect);
	}
}

void ThrowingKnife::Update(float elapsedSec, const Rectf& cameraWindow, std::vector<Enemy*>& pEnemyPtrs, Avatar* pAvatar)
{
	if (!m_Exploding)
	{
		CheckIfIsInFrame(cameraWindow);

		if (m_IsInFrame)
		{
			if (m_FacingRight)
			{
				m_Shape.left += m_Velocity.x * elapsedSec;
			}
			else
			{
				m_Shape.left -= m_Velocity.x * elapsedSec;
			}
			// Collision
			DoCollision(pAvatar);
		}
	}
	else
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
			m_Exploding = false;
			m_AnimTime = 0.f;
			m_AnimFrame = 0;
		}
	}
}

void ThrowingKnife::DoCollision(Avatar* pAvatar)
{
	Rectf avatarShape{ pAvatar->GetShape() };

	if (pAvatar->IsAttacking())
	{
		Rectf swordHitBox;
		if (pAvatar->GetFacingRight())
		{
			swordHitBox = Rectf{ avatarShape.left + avatarShape.width, avatarShape.bottom + avatarShape.width / 3, avatarShape.width, avatarShape.height * 2 / 3 };
		}
		else
		{
			swordHitBox = Rectf{ avatarShape.left - avatarShape.width, avatarShape.bottom, avatarShape.width, avatarShape.height };
		}
		if (utils::IsOverlapping(swordHitBox, m_Shape))
		{
			m_Exploding = true;
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
			m_Exploding = true;
		}
	}
	if (pAvatar->IsjumpSlashing())
	{
		if (utils::IsOverlapping(m_Shape, avatarShape))
		{
			m_Exploding = true;
			return;
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
			m_Exploding = true;
		}
	}


	if (utils::IsOverlapping(m_Shape, avatarShape))
	{
		if (m_Shape.left + m_Shape.width / 2 >= avatarShape.left + avatarShape.width / 2)
		{
			pAvatar->TakeDamage(true);
		}
		else
		{
			pAvatar->TakeDamage(false);
		}
	}

}

void ThrowingKnife::Destroy()
{
	m_Exploding = true;
}
