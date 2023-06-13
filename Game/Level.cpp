#include "pch.h"
#include "Level.h"
#include "Texture.h"
#include "SVGParser.h"
#include "utils.h"
#include "Platform.h"
#include "Walls.h"
#include "SoundManager.h"
#include "TextureManager.h"

Level::Level(SoundManager* SM, TextureManager* TM, bool bossRoom)
	: m_pTM{ TM }
	, m_pSM{ SM }
{
	if (!bossRoom)
	{
		m_BGTextureStr = "Resources/images/levelbg.png";
		SVGParser::GetVerticesFromSvgFile("resources/svgs/levelbg.svg", m_Floor);
		m_SoundStr = "resources/sounds/soundstreams/mainlevel.mp3";
	}
	else
	{
		m_BGTextureStr = "Resources/images/bossroom.png";
		SVGParser::GetVerticesFromSvgFile("resources/svgs/bossroombg.svg", m_Floor);
		m_SoundStr = "resources/sounds/soundstreams/bossroom.mp3";
	}
	
	m_pTM->LoadTexture(m_BGTextureStr);
	m_pSM->LoadStream(m_SoundStr);
	m_pSM->PlayStream(m_SoundStr);

	m_pPlatforms = new Platform(bossRoom);
	m_pWalls = new Walls(bossRoom);
	m_BossRoom = bossRoom;
}

Level::~Level()
{
	m_pTM->DeleteTexture(m_BGTextureStr);
	delete m_pPlatforms;
	m_pPlatforms = nullptr;
	delete m_pWalls;
	m_pWalls = nullptr;
	m_pSM->DeleteStream(m_SoundStr);
}

void Level::DrawBackground() const
{
	m_pTM->GetTexture(m_BGTextureStr)->Draw(Point2f{ 0,0 });
}

bool Level::HandleHorizontalCollision(Rectf& actorShape, Vector2f& actorVelocity) const
{
	int raycastMargin{ 3 };

	utils::HitInfo info{};
	Point2f leftOfActor{ actorShape.left - raycastMargin, actorShape.bottom + raycastMargin };
	Point2f rightOfActor{ actorShape.left + actorShape.width + raycastMargin, actorShape.bottom + raycastMargin };

	for (int i{}; i < m_Floor.size(); i++)
	{
		// Bottom ray of hitbox
		if (utils::Raycast(m_Floor[i], leftOfActor, rightOfActor, info))
		{
			if (info.intersectPoint.x < (leftOfActor.x + rightOfActor.x) / 2 )
			{
				actorShape.left = info.intersectPoint.x + raycastMargin;
			}
			else
			{
				actorShape.left = info.intersectPoint.x - actorShape.width - raycastMargin;
			}
			actorVelocity.x = 0;
		}
	}
	return m_pWalls->HandleHorizontalCollision(actorShape, actorVelocity);
}

bool Level::HandleVerticalCollision(Rectf& actorShape, Vector2f& actorVelocity) const
{
	bool isOnGround{ false };

	utils::HitInfo info{};
	Point2f bottomLeftOfActor{ actorShape.left , actorShape.bottom - 1 };
	Point2f topLeftOfActor{ actorShape.left, actorShape.bottom + actorShape.height };

	Point2f bottomMiddleOfActor{ actorShape.left + actorShape.width / 2 , actorShape.bottom - 1 };
	Point2f topMiddleOfActor{ actorShape.left + actorShape.width / 2 , actorShape.bottom + actorShape.height };

	Point2f bottomRightOfActor{ actorShape.left + actorShape.width, actorShape.bottom - 1 };
	Point2f topRightOfActor{ actorShape.left + actorShape.width, actorShape.bottom + actorShape.height };

	for (int i{}; i < m_Floor.size(); i++)
	{
		// Vertical rays
		if (actorVelocity.y <= 0)
		{
			// Left ray of hitbox
			if (utils::Raycast(m_Floor[i], topLeftOfActor, bottomLeftOfActor, info))
			{
				actorShape.bottom = info.intersectPoint.y - 1;
				actorVelocity.y = 0;
				isOnGround = true;
				break;
			}

			// Middle Ray of hitbox
			else if (utils::Raycast(m_Floor[i], topMiddleOfActor, bottomMiddleOfActor, info))
			{
				actorShape.bottom = info.intersectPoint.y - 1;
				actorVelocity.y = 0;
				isOnGround = true;
				break;
			}

			// Right Ray of hitbox
			else if (utils::Raycast(m_Floor[i], topRightOfActor, bottomRightOfActor, info))
			{
				actorShape.bottom = info.intersectPoint.y - 1;
				actorVelocity.y = 0;
				isOnGround = true;
				break;
			}
		}
	}
	return (isOnGround || m_pPlatforms->HandleVerticalCollision(actorShape, actorVelocity));
}

float Level::GetLevelWidth() const
{
	return 	m_pTM->GetTexture(m_BGTextureStr)->GetWidth();
}

int Level::IsFullEnemyOnPlatform(Rectf& actorShape) const
{
	utils::HitInfo info{};
	Point2f bottomLeftOfActor{ actorShape.left , actorShape.bottom - 1 };
	Point2f topLeftOfActor{ actorShape.left, actorShape.bottom + actorShape.height };

	Point2f bottomRightOfActor{ actorShape.left + actorShape.width, actorShape.bottom - 1 };
	Point2f topRightOfActor{ actorShape.left + actorShape.width, actorShape.bottom + actorShape.height };

	for (int i{}; i < m_Floor.size(); i++)
	{
		// Left ray of hitbox
		if (utils::Raycast(m_Floor[i], topLeftOfActor, bottomLeftOfActor, info))
		{
			actorShape.bottom = info.intersectPoint.y;
			if (utils::Raycast(m_Floor[i], topRightOfActor, bottomRightOfActor, info))
			{
				return 0;
			}
			return 2;
		}

		// Right Ray of hitbox
		if (utils::Raycast(m_Floor[i], topRightOfActor, bottomRightOfActor, info))
		{
			return 1;
		}
	}

	return m_pPlatforms->IsFullEnemyOnPlatform(actorShape);
}

bool Level::BounceOfWall(Rectf& actorShape, Vector2f& actorVelocity) const
{
	if (m_pWalls->HandleHorizontalCollision(actorShape, actorVelocity))
	{
		return false;
	}


	int raycastMargin{ 3 };

	utils::HitInfo info{};
	Point2f leftOfActor{ actorShape.left - raycastMargin, actorShape.bottom + raycastMargin };
	Point2f rightOfActor{ actorShape.left + actorShape.width + raycastMargin, actorShape.bottom + raycastMargin };

	for (int i{}; i < m_Floor.size(); i++)
	{
		// Bottom ray of hitbox
		if (utils::Raycast(m_Floor[i], leftOfActor, rightOfActor, info))
		{
			if (info.intersectPoint.x < (leftOfActor.x + rightOfActor.x) / 2 && actorVelocity.x < 0)
			{
				return true;
			}
			else if (info.intersectPoint.x > (leftOfActor.x + rightOfActor.x) / 2 && actorVelocity.x > 0)
			{
				return true;
			}
		}
	}
	return false;
}

bool Level::GetIsBossRoom() const
{
	return m_BossRoom;
}


