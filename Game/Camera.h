#pragma once

class Camera final
{
public:
	explicit Camera(float width);
	void SetLevelBoundaries(const Rectf& levelBoundaries);
	void Transform(const Rectf& target) const;

	Rectf GetWindow(const Rectf& target) const;
private:
	float m_Width;
	Rectf m_LevelBoundaries;

	Point2f Track(const Rectf& target) const;
	Point2f Clamp(const Point2f& bottomLeftPos) const;
};