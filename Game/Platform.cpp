#include "pch.h"
#include "Platform.h"
#include "SVGParser.h"
#include "utils.h"

Platform::Platform(bool bossRoom)
{
	if (!bossRoom)
	{
		SVGParser::GetVerticesFromSvgFile("resources/svgs/levelplatforms.svg", m_Platform);
	}
	else
	{
		SVGParser::GetVerticesFromSvgFile("resources/svgs/bossroomplatforms.svg", m_Platform);
	}
}

bool Platform::HandleVerticalCollision(Rectf& actorShape, Vector2f& actorVelocity) const
{
	bool isOnGround{ false };

	if (actorVelocity.y <= 0.f)
	{
		utils::HitInfo info{};

		// Because there are small platforms in the game
		// I use 5 raycasts so that the platform wouldnt go throught the
		// 'holes' of the raycasts

		int verticalRayLength{ 10 };

		Point2f topLeftOfActor{ actorShape.left, actorShape.bottom + verticalRayLength };
		Point2f bottomLeftOfActor{ actorShape.left , actorShape.bottom - 1 };

		Point2f bottomMiddleLeftOfActor{ actorShape.left + actorShape.width / 4 , actorShape.bottom - 1 };
		Point2f topMiddleLeftOfActor{ actorShape.left + actorShape.width / 4 , actorShape.bottom + verticalRayLength };

		Point2f bottomMiddleOfActor{ actorShape.left + actorShape.width / 2 , actorShape.bottom - 1 };
		Point2f topMiddleOfActor{ actorShape.left + actorShape.width / 2 , actorShape.bottom + verticalRayLength };

		Point2f bottomMiddleRightOfActor{ actorShape.left + 3 * actorShape.width / 4 , actorShape.bottom - 1 };
		Point2f topMiddleRightOfActor{ actorShape.left + 3 * actorShape.width / 4 , actorShape.bottom + verticalRayLength };

		Point2f bottomRightOfActor{ actorShape.left + actorShape.width, actorShape.bottom - 1 };
		Point2f topRightOfActor{ actorShape.left + actorShape.width, actorShape.bottom + verticalRayLength };

		for (int i{}; i < m_Platform.size(); i++)
		{
			// Vertical rays

			// Left ray of hitbox
			if (utils::Raycast(m_Platform[i], topLeftOfActor, bottomLeftOfActor, info))
			{
				actorShape.bottom = info.intersectPoint.y - 1;
				actorVelocity.y = 0;
				isOnGround = true;
				break;
			}

			// Middle left ray of hitbox
			else if (utils::Raycast(m_Platform[i], topMiddleLeftOfActor, bottomMiddleLeftOfActor, info))
			{
				actorShape.bottom = info.intersectPoint.y - 1;
				actorVelocity.y = 0;
				isOnGround = true;
				break;
			}

			// Middle Ray of hitbox
			else if (utils::Raycast(m_Platform[i], topMiddleOfActor, bottomMiddleOfActor, info))
			{
				actorShape.bottom = info.intersectPoint.y - 1;
				actorVelocity.y = 0;
				isOnGround = true;
				break;
			}

			// Middle right ray of hitbox
			else if (utils::Raycast(m_Platform[i], topMiddleRightOfActor, bottomMiddleRightOfActor, info))
			{
				actorShape.bottom = info.intersectPoint.y - 1;
				actorVelocity.y = 0;
				isOnGround = true;
				break;
			}

			// Right Ray of hitbox
			else if (utils::Raycast(m_Platform[i], topRightOfActor, bottomRightOfActor, info))
			{
				actorShape.bottom = info.intersectPoint.y - 1;
				actorVelocity.y = 0;
				isOnGround = true;
				break;
			}
		}
	}
	return isOnGround;
}

int Platform::IsFullEnemyOnPlatform(Rectf& actorShape) const
{
	utils::HitInfo info{};

	Point2f topLeftOfActor{ actorShape.left, actorShape.bottom + 7 };
	Point2f bottomLeftOfActor{ actorShape.left , actorShape.bottom - 1 };

	Point2f bottomRightOfActor{ actorShape.left + actorShape.width, actorShape.bottom - 1 };
	Point2f topRightOfActor{ actorShape.left + actorShape.width, actorShape.bottom + 7 };

	for (int i{}; i < m_Platform.size(); i++)
	{
		// Left ray of hitbox
		if (utils::Raycast(m_Platform[i], topLeftOfActor, bottomLeftOfActor, info))
		{
			actorShape.bottom = info.intersectPoint.y;
			if (utils::Raycast(m_Platform[i], topRightOfActor, bottomRightOfActor, info))
			{
				return 0;
			}
			return 2;
		}

		// Right Ray of hitbox
		if (utils::Raycast(m_Platform[i], topRightOfActor, bottomRightOfActor, info))
		{
			return 1;
		}
	}

	return 0;
}