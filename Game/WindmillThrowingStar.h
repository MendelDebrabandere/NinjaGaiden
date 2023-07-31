#pragma once
#include "Projectile.h"

class TextureManager;

class WindmillThrowingStar final: public Projectile
{
public:
	explicit WindmillThrowingStar(TextureManager* TM, Point2f spawnPos);
	~WindmillThrowingStar() override;
	WindmillThrowingStar(const WindmillThrowingStar& other) = delete;
	WindmillThrowingStar& operator=(const WindmillThrowingStar& other) = delete;
	WindmillThrowingStar(WindmillThrowingStar&& other) = delete;
	WindmillThrowingStar& operator=(WindmillThrowingStar&& other) = delete;


	void Draw() const override;
	void Update(float elapsedSec, const Rectf& cameraWindow, std::vector<Enemy*>&, Avatar* pAvatar) override;

	void Destroy() override;

private:
	static int s_InstanceCounter;

	int m_AnimFrame;
	float m_AnimTime;
	float m_TimeFlying;
	float m_SpeedFactor;

	void DoCollision(Enemy* pEnemy);
};

