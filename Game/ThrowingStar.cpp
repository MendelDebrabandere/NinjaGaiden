#include "pch.h"
#include "ThrowingStar.h"
#include "Texture.h"
#include "Enemy.h"
#include "utils.h"
#include "TextureManager.h"	

int ThrowingStar::s_InstanceCounter{};

ThrowingStar::ThrowingStar(TextureManager* TM, Point2f spawnPos)
	: m_AnimFrame{ 0 }
	, m_AnimTime{ 0.f }
{
	m_pTM = TM;
	m_TextureStr = "resources/images/throwingstars.png";
	m_pTM->LoadTexture(m_TextureStr);
	s_InstanceCounter++;

	Texture* texture { m_pTM->GetTexture(m_TextureStr) };
	m_Shape = Rectf{ spawnPos.x - texture->GetWidth() / 8, spawnPos.y - texture->GetHeight() / 6,texture->GetWidth() / 4, texture->GetHeight() / 3};
	m_Velocity.x *= 2;
}

ThrowingStar::~ThrowingStar()
{
	s_InstanceCounter--;
	if (s_InstanceCounter == 0)
	{
		m_pTM->DeleteTexture(m_TextureStr);
	}
}

void ThrowingStar::Draw() const
{
	if (m_IsInFrame)
	{
		Rectf srcRect{ 0, m_Shape.height, m_Shape.width, m_Shape.height};
		srcRect.left = m_AnimFrame * m_Shape.width;

		if (!m_FacingRight)
		{
			glPushMatrix();
			glTranslatef(2 * m_Shape.left + m_Shape.width, 0, 0);
			glScalef(-1, 1, 1);
			m_pTM->GetTexture(m_TextureStr)->Draw(m_Shape, srcRect);
			glPopMatrix();
		}
		else
		{
			m_pTM->GetTexture(m_TextureStr)->Draw(m_Shape, srcRect);
		}
	}
}

void ThrowingStar::Update(float elapsedSec, const Rectf& cameraWindow, std::vector<Enemy*>& pEnemyPtrs, Avatar* pAvatar)
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
		for (int i{}; i < pEnemyPtrs.size(); i++)
		{
			DoCollision(pEnemyPtrs[i]);
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
}

void ThrowingStar::DoCollision(Enemy* pEnemy)
{
	if (utils::IsOverlapping(m_Shape, pEnemy->GetShape()))
	{
		pEnemy->TakeDamage();
		m_Shape.left = -500;
	}
}

void ThrowingStar::Destroy()
{
	m_Shape.left = -500;
}