#include "pch.h"
#include "ArtOfTheFireWheel.h"
#include "Texture.h"
#include "Enemy.h"
#include "utils.h"
#include "TextureManager.h"

int ArtOfTheFireWheel::s_InstanceCounter{};

ArtOfTheFireWheel::ArtOfTheFireWheel(TextureManager* TM, Point2f spawnPos)
	: m_AnimFrame{ 0 }
	, m_AnimTime{ 0.f }
	, m_TextureStr{ "resources/images/artofthefirewheel.png" }
	, m_pTM{TM}
{
	m_pTM->LoadTexture(m_TextureStr);
	s_InstanceCounter++;

	Texture* texture{ m_pTM->GetTexture(m_TextureStr) };

	m_Shape = Rectf{ spawnPos.x - texture->GetWidth() / 8, spawnPos.y - texture->GetHeight(), texture->GetWidth() / 4, texture->GetHeight() };
	m_Velocity.x *= 2;
	m_Velocity.y = 160;
	Rectf m_Shape1{ m_Shape };
	Rectf m_Shape2{ m_Shape };
	Rectf m_Shape3{ m_Shape };
}

ArtOfTheFireWheel::~ArtOfTheFireWheel()
{
	s_InstanceCounter--;
	if (s_InstanceCounter == 0)
	{
		m_pTM->DeleteTexture(m_TextureStr);
	}
}

void ArtOfTheFireWheel::Draw() const
{
	if (m_IsInFrame)
	{
		Texture* texture{ m_pTM->GetTexture(m_TextureStr) };

		Rectf srcRect{ 0, texture->GetHeight(), texture->GetWidth()/4, texture->GetHeight() };
		srcRect.left = m_AnimFrame * srcRect.width;

		if (!m_FacingRight)
		{
			glPushMatrix();
			glTranslatef(2 * m_Shape.left + m_Shape.width, 0, 0);
			glScalef(-1, 1, 1);
			texture->Draw(m_Shape1, srcRect);
			texture->Draw(m_Shape2, srcRect);
			texture->Draw(m_Shape3, srcRect);
			glPopMatrix();
		}
		else
		{
			texture->Draw(m_Shape1, srcRect);
			texture->Draw(m_Shape2, srcRect);
			texture->Draw(m_Shape3, srcRect);
		}
	}
}

void ArtOfTheFireWheel::Update(float elapsedSec, const Rectf& cameraWindow, std::vector<Enemy*>& pEnemyPtrs, Avatar* pAvatar)
{
	Rectf camera{ cameraWindow };
	camera.left -= 50;
	camera.bottom -= 50;
	camera.width += 100;
	camera.height += 100;

	bool isInLastFrame{ m_IsInFrame };
	CheckIfIsInFrame(camera);
	if ((m_IsInFrame != isInLastFrame) && (m_IsInFrame == true))
	{
		m_Shape.bottom -= m_Shape.height;
		m_Shape.left -= m_Shape.width/2;
	}

	if (m_IsInFrame)
	{
		if (m_FacingRight)
		{
			m_Shape.left += m_Velocity.x * elapsedSec;
			m_Shape.bottom += m_Velocity.y * elapsedSec;
		}
		else
		{
			m_Shape.left -= m_Velocity.x * elapsedSec;
			m_Shape.bottom += m_Velocity.y * elapsedSec;
		}

		// spread box
		int spreadPerSec{ 140 };
		m_Shape.height += spreadPerSec * elapsedSec;
		m_Shape.bottom -= spreadPerSec / 2.f * elapsedSec;
		
		// set every box right
		m_Shape1.left = m_Shape.left;
		m_Shape1.bottom = m_Shape.bottom + m_Shape.height - m_Shape1.height;

		m_Shape2.left = m_Shape.left;
		m_Shape2.bottom = m_Shape.bottom + m_Shape.height/2 - m_Shape2.height/2;

		m_Shape3.left = m_Shape.left;
		m_Shape3.bottom = m_Shape.bottom;


		// Collision
		for (int i{}; i < pEnemyPtrs.size(); i++)
		{
			DoCollision(pEnemyPtrs[i]);
		}

		int framesPerSec{ 24 };
		int nrOfFrames{ 4 };
		m_AnimTime += elapsedSec;
		if (m_AnimTime >= 1.f / framesPerSec)
		{
			m_AnimTime -= 1.f / framesPerSec;
			++m_AnimFrame %= nrOfFrames;
		}
	}
	else
	{
		m_Shape.height = m_Shape.width;
	}
}

void ArtOfTheFireWheel::DoCollision(Enemy* pEnemy)
{
	if (utils::IsOverlapping(m_Shape, pEnemy->GetShape()))
	{
		pEnemy->TakeDamage();
	}
}
