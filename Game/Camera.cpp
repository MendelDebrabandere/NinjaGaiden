#include "pch.h"
#include "Camera.h"

Camera::Camera(float width)
	: m_Width{ width }
	, m_LevelBoundaries{0,0, width, 208 }
{
}

void Camera::SetLevelBoundaries(const Rectf& levelBoundaries)
{
	m_LevelBoundaries = levelBoundaries;
}

Point2f Camera::Track(const Rectf& target) const
{
	// Camera never moves vertically so the yValue is set to 0 instantly
	Point2f center{ target.left + target.width / 2, 0 };
	center.x -= m_Width / 2;
	return center;
}

Point2f Camera::Clamp(const Point2f& bottomLeftPos) const
{
	Point2f newPos{ bottomLeftPos };
	if (bottomLeftPos.x <= m_LevelBoundaries.left)
	{
		newPos.x = m_LevelBoundaries.left;
	}
	else if (bottomLeftPos.x + m_Width >= m_LevelBoundaries.left + m_LevelBoundaries.width)
	{
		newPos.x = m_LevelBoundaries.left + m_LevelBoundaries.width - m_Width;
	}
	return newPos;
}

void Camera::Transform(const Rectf& target) const
{
	Point2f bottomLeft{ Track(target) };
	bottomLeft = Clamp(bottomLeft);
	glTranslatef(-bottomLeft.x, -bottomLeft.y, 0);
}

Rectf Camera::GetWindow(const Rectf& target) const
{
	Point2f bottomLeft{ Track(target) };
	bottomLeft = Clamp(bottomLeft);
	return Rectf{ bottomLeft.x, 0, m_Width, 0 };
}
