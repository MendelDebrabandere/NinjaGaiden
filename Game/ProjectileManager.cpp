#include "pch.h"
#include "ProjectileManager.h"
#include "ThrowingKnife.h"
#include "ThrowingStar.h"
#include "ArtOfTheFireWheel.h"
#include "WindmillThrowingStar.h"
#include "utils.h"

ProjectileManager::ProjectileManager(TextureManager* TM)
{
	m_ProjectilePtrs.resize(11);

	for (int idx{}; idx < m_ProjectilePtrs.size(); idx++)
	{
		if (idx < 6)
		{
			 m_ProjectilePtrs[idx] = new ThrowingKnife(TM, Point2f{-500, 0});
		}
		else if (idx < 9)
		{
			m_ProjectilePtrs[idx] = new ThrowingStar(TM, Point2f{-500, 0});
		}
		else if (idx < 10)
		{
			m_ProjectilePtrs[idx] = new ArtOfTheFireWheel(TM, Point2f{ -500, 0 });
		}
		else if (idx < 11)
		{
			m_ProjectilePtrs[idx] = new WindmillThrowingStar(TM, Point2f{ -500, 0 });
		}
	}
	// Knives = 6
	// Shuriken = 3
	// ArtOfTheFireWheel = 1
	// WindmillThrowingStar = 1
}

ProjectileManager::~ProjectileManager()
{
	for (int idx{}; idx < m_ProjectilePtrs.size(); idx++)
	{
		delete m_ProjectilePtrs[idx];
	}
}

void ProjectileManager::Draw() const
{
	for (int idx{}; idx < m_ProjectilePtrs.size(); idx++)
	{
		m_ProjectilePtrs[idx]->Draw();
	}
}

void ProjectileManager::Update(float elapsedSec, const Rectf& cameraWindow, std::vector<Enemy*>& enemyPtrs, Avatar* pAvatar, std::vector<Powerup*>& pPowerups)
{
	for (int idx{}; idx < m_ProjectilePtrs.size(); idx++)
	{
		m_ProjectilePtrs[idx]->Update(elapsedSec, cameraWindow, enemyPtrs, pAvatar);
		m_ProjectilePtrs[idx]->CheckPowerupHit(pPowerups);
	}
	CollisionBetweenProjectiles();
}

void ProjectileManager::RequestKnife(Point2f spawnPos, bool facingRight)
{
	for (int i{}; i < 6; i++)
	{
		if (!m_ProjectilePtrs[i]->GetIsInFrame())
		{
			m_ProjectilePtrs[i]->SetPosition(spawnPos);
			m_ProjectilePtrs[i]->SetFacingRight(facingRight);
			break;
		}
	}
}

void ProjectileManager::RequestStar(Point2f spawnPos, bool facingRight)
{
	for (int i{6}; i < 9; i++)
	{
		if (!m_ProjectilePtrs[i]->GetIsInFrame())
		{
			m_ProjectilePtrs[i]->SetPosition(spawnPos);
			m_ProjectilePtrs[i]->SetFacingRight(facingRight);
			break;
		}
	}
}

void ProjectileManager::RequestArtOfTheFireWheel(Point2f spawnPos, bool facingRight)
{
	for (int i{ 9 }; i < 10; i++)
	{
		if (!m_ProjectilePtrs[i]->GetIsInFrame())
		{
			m_ProjectilePtrs[i]->SetPosition(spawnPos);
			m_ProjectilePtrs[i]->SetFacingRight(facingRight);
			break;
		}
	}
}

void ProjectileManager::RequestWindmillThrowingStar(Point2f spawnPos, bool facingRight)
{
	for (int i{ 10 }; i < 11; i++)
	{
		if (!m_ProjectilePtrs[i]->GetIsInFrame())
		{
			m_ProjectilePtrs[i]->SetPosition(spawnPos);
			m_ProjectilePtrs[i]->SetFacingRight(facingRight);
			break;
		}
	}
}

void ProjectileManager::CollisionBetweenProjectiles()
{
	for (int i{}; i < 6; i++)
	{
		if (m_ProjectilePtrs[i]->GetIsInFrame())
		{
			for (int idx{ 6 }; idx < 10; idx++)
			{
				if (m_ProjectilePtrs[idx]->GetIsInFrame())
				{
					if (utils::IsOverlapping(m_ProjectilePtrs[i]->GetShape(), m_ProjectilePtrs[idx]->GetShape()))
					{
						m_ProjectilePtrs[i]->Destroy();
						m_ProjectilePtrs[idx]->Destroy();
					}
				}
			}
		}
	}
}

