#pragma once
#include <vector>
#include "Vector2f.h"

class Enemy;
class Avatar;
class Powerup;
class TextureManager;

class Projectile
{
public:
	explicit Projectile();
	virtual ~Projectile();

	Projectile(const Projectile& other) = delete;
	Projectile& operator=(const Projectile& other) = delete;
	Projectile(const Projectile&&) = delete;
	Projectile& operator=(const Projectile&& other) = delete;

	virtual void Draw() const = 0;
	virtual void Update(float elapsedSec, const Rectf& cameraWindow, std::vector<Enemy*>& pEnemyPtrs, Avatar* pAvatar) = 0;

	void SetPosition(Point2f newPos);
	void SetFacingRight(bool facing);
	bool GetIsInFrame() const;
	Rectf GetShape() const;
	void CheckPowerupHit(std::vector<Powerup*>& pPowerups);
	virtual void Destroy();

protected:
	TextureManager* m_pTM;
	std::string m_TextureStr;

	Rectf m_Shape;
	bool m_IsInFrame;
	bool m_FacingRight;
	Vector2f m_Velocity;

	void CheckIfIsInFrame(const Rectf& cameraWindow);
};

