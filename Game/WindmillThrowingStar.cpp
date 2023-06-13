#include "pch.h"
#include "WindmillThrowingStar.h"
#include "Texture.h"
#include "Avatar.h"
#include "Enemy.h"
#include "utils.h"
#include "TextureManager.h"

int WindmillThrowingStar::s_InstanceCounter{};

WindmillThrowingStar::WindmillThrowingStar(TextureManager* TM, Point2f spawnPos)
	: m_AnimFrame{ 0 }
	, m_AnimTime{ 0.f }
	, m_TimeFlying{ 0.f }
	, m_SpeedFactor{5.f}
{
	m_pTM = TM;
	m_TextureStr = "resources/images/throwingstars.png";
	m_pTM->LoadTexture(m_TextureStr);

	s_InstanceCounter++;

	m_Shape = Rectf{ spawnPos.x, spawnPos.y ,m_pTM->GetTexture(m_TextureStr)->GetWidth() / 2, m_pTM->GetTexture(m_TextureStr)->GetHeight() * 2 / 3 };

	if (!m_FacingRight)
	{
		m_Velocity.x *= -m_SpeedFactor;
	}
	else
	{
		m_Velocity.x *= m_SpeedFactor;
	}
}

WindmillThrowingStar::~WindmillThrowingStar()
{
	s_InstanceCounter--;
	if (s_InstanceCounter == 0)
	{
		m_pTM->DeleteTexture(m_TextureStr);
	}
}

void WindmillThrowingStar::Draw() const
{
	if (m_IsInFrame)
	{
		Rectf srcRect{ 0, m_pTM->GetTexture(m_TextureStr)->GetHeight(), m_Shape.width, m_Shape.height};
		srcRect.left = m_AnimFrame * m_Shape.width;

		m_pTM->GetTexture(m_TextureStr)->Draw(m_Shape, srcRect);
	}
}

void WindmillThrowingStar::Update(float elapsedSec, const Rectf& cameraWindow, std::vector<Enemy*>& pEnemyPtrs, Avatar* pAvatar)
{
	CheckIfIsInFrame(cameraWindow);

	if (m_IsInFrame)
	{
		m_TimeFlying += elapsedSec;

		float horAccelSpeed{ 45.f };
		float verAccelSpeed{ 15.f };
		Point2f avatarMiddle{ pAvatar->GetShape().left + pAvatar->GetShape().width / 2, pAvatar->GetShape().bottom + pAvatar->GetShape().height / 2 + m_Shape.height / 2 };
		Point2f starMiddle{ m_Shape.left + m_Shape.width / 2, m_Shape.bottom + m_Shape.height / 2 };
		Vector2f accelDirection{ avatarMiddle.x - starMiddle.x, avatarMiddle.y - starMiddle.y };

		if (abs(accelDirection.y) >= 40)
		{
			m_Velocity.y = accelDirection.y;
		}
		if (abs(accelDirection.x) >= 100)
		{
			m_Velocity.x = accelDirection.x;
		}

		m_Velocity.x += accelDirection.x * elapsedSec * horAccelSpeed;
		m_Velocity.y += accelDirection.y * elapsedSec * verAccelSpeed;

		m_Shape.left += m_Velocity.x * elapsedSec;
		m_Shape.bottom += m_Velocity.y * elapsedSec;

		// Collision
		for (int i{}; i < pEnemyPtrs.size(); i++)
		{
			DoCollision(pEnemyPtrs[i]);
		}

		// Check if it returns to avatar
		if (m_TimeFlying >= 0.2)
		{
			if (utils::IsPointInRect(avatarMiddle, m_Shape))
			{
				m_Shape.left = -500;
				m_Velocity = Vector2f{ 120 * m_SpeedFactor, 0 };
				m_TimeFlying = 0.f;
			}
		}

		int framesPerSec{ 10 };
		int nrOfFrames{ 2 };
		m_AnimTime += elapsedSec;
		if (m_AnimTime >= 1.f / framesPerSec)
		{
			m_AnimTime -= 1.f / framesPerSec;
			++m_AnimFrame %= nrOfFrames;
		}
	}
	else
	{
		m_Shape.left = -500;
		m_Velocity = Vector2f{ 120 * m_SpeedFactor, 0 };
		m_TimeFlying = 0.f;
	}
}

void WindmillThrowingStar::Destroy()
{
}

void WindmillThrowingStar::DoCollision(Enemy* pEnemy)
{
	if (utils::IsOverlapping(m_Shape, pEnemy->GetShape()))
	{
		pEnemy->TakeDamage();
		Destroy();
	}
}
