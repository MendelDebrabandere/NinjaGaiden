#pragma once
#include "Projectile.h"

class TextureManager;

class ThrowingStar final: public Projectile
{
public:
	explicit ThrowingStar(TextureManager* TM, Point2f spawnPos);
	virtual ~ThrowingStar();

	ThrowingStar(const ThrowingStar& other) = delete;
	ThrowingStar& operator=(const ThrowingStar& other) = delete;
	ThrowingStar(ThrowingStar&& other) = delete;
	ThrowingStar& operator=(ThrowingStar&& other) = delete;



	virtual void Draw() const;
	virtual void Update(float elapsedSec, const Rectf& cameraWindow, std::vector<Enemy*>&, Avatar* pAvatar);

	virtual void Destroy() override;

private:
	static int s_InstanceCounter;

	int m_AnimFrame;
	float m_AnimTime;

	void DoCollision(Enemy * pEnemy);

};

