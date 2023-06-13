#include "pch.h"
#include "Projectile.h"
#include "utils.h"
#include "Powerup.h"

Projectile::Projectile()
	: m_IsInFrame{false}
	, m_FacingRight{true}
	, m_Velocity{ Vector2f{120,0} }
	, m_pTM{ nullptr }
	, m_TextureStr{}
{
}

Projectile::~Projectile()
{
}

void Projectile::SetPosition(Point2f newPos)
{
	m_Shape.left = newPos.x;
	m_Shape.bottom = newPos.y;
}

void Projectile::SetFacingRight(bool facing)
{
	if (!facing && abs(m_Velocity.x) > 350)
	{
		m_Velocity.x *= -1;
	}
	m_FacingRight = facing;
}

bool Projectile::GetIsInFrame() const
{
	return m_IsInFrame;
}

void Projectile::CheckIfIsInFrame(const Rectf& cameraWindow)
{
	if (m_Shape.left >= cameraWindow.left && m_Shape.left + m_Shape.width <= cameraWindow.left + cameraWindow.width)
	{
		m_IsInFrame = true;
	}
	else
	{
		m_IsInFrame = false;
		m_Shape.left = -500;
	}
}

Rectf Projectile::GetShape() const
{
	return m_Shape;
}

void Projectile::CheckPowerupHit(std::vector<Powerup*>& pPowerups)
{
	for (int i{}; i < pPowerups.size(); i++)
	{
		Rectf lanternShape{ pPowerups[i]->GetLanternShape() };
		if (lanternShape.left != 0)
		{
			if (utils::IsOverlapping(lanternShape, m_Shape))
			{
				Destroy();
				pPowerups[i]->Drop();
			}
		}
	}
}

void Projectile::Destroy()
{
}

