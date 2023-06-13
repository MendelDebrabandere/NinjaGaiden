#pragma once
#include <vector>
#include "Vector2f.h"

class Platform final
{
public:
	explicit Platform(bool bossRoom);

	bool HandleVerticalCollision(Rectf& actorShape, Vector2f& actorVelocity) const;
	
	int IsFullEnemyOnPlatform(Rectf& actorShape) const;
	// This function returns ints beacuase there are 3 possibilities:
	// 0 -> fully on platform / not at all on platform
	// 1 -> left foot is not on the platform
	// 2 -> right foot is not on the platform

private:
	std::vector<std::vector<Point2f>> m_Platform;
};

