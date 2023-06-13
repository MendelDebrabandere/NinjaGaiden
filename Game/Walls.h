#pragma once
#include <vector>

class Walls final
{
public:
	explicit Walls(bool bossRoom);

	bool HandleHorizontalCollision(Rectf& actorShape, Vector2f& actorVelocity);

private:
	std::vector<std::vector<Point2f>> m_WallVerticesRight;
	std::vector<std::vector<Point2f>> m_WallVerticesLeft;
};