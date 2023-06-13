#include "pch.h"
#include "utils.h"
#include "SVGParser.h"
#include "Walls.h"

Walls::Walls(bool bossRoom)
{
	if (!bossRoom)
	{
		SVGParser::GetVerticesFromSvgFile("resources/svgs/levelwallsright.svg", m_WallVerticesRight);
		SVGParser::GetVerticesFromSvgFile("resources/svgs/levelwallsleft.svg", m_WallVerticesLeft);
	}
	else
	{
		SVGParser::GetVerticesFromSvgFile("resources/svgs/bossroomwallsright.svg", m_WallVerticesRight);
		SVGParser::GetVerticesFromSvgFile("resources/svgs/bossroomwallsleft.svg", m_WallVerticesLeft);
	}
}


bool Walls::HandleHorizontalCollision(Rectf& actorShape, Vector2f& actorVelocity)
{
	float verticalRaycastMargin{ actorShape.width/3 };

	utils::HitInfo info{};
	Point2f leftOfActor{ actorShape.left, actorShape.bottom + verticalRaycastMargin };
	Point2f rightOfActor{ actorShape.left + actorShape.width, actorShape.bottom + verticalRaycastMargin };

	if (actorVelocity.x > 0)
	{
		for (int i{}; i < m_WallVerticesRight.size(); i++)
		{
			// Bottom ray of hitbox
			if (utils::Raycast(m_WallVerticesRight[i], leftOfActor, rightOfActor, info))
			{
				if (info.intersectPoint.x > (leftOfActor.x + rightOfActor.x) / 2)
				{
					actorShape.left = info.intersectPoint.x - actorShape.width;
					actorVelocity.x = 0;
					return true;
				}
			}
		}
	}

	else if (actorVelocity.x <= 0.f)
	{
		for (int i{}; i < m_WallVerticesLeft.size(); i++)
		{
			// Bottom ray of hitbox
			if (utils::Raycast(m_WallVerticesLeft[i], leftOfActor, rightOfActor, info))
			{
				if (info.intersectPoint.x < (leftOfActor.x + rightOfActor.x) / 2)
				{
					actorShape.left = info.intersectPoint.x;
					actorVelocity.x = 0;
					return true;
				}
			}
		}
	}

	return false;
}
