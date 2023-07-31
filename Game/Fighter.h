#pragma once
#include "Enemy.h"

class Fighter final: public Enemy
{
public:
	explicit Fighter(SoundManager* SM, TextureManager* TM, const Point2f& spawnPos);
	~Fighter() override;

	Fighter(const Fighter& other) = delete;
	Fighter& operator=(const Fighter& other) = delete;
	Fighter(Fighter&& other) = delete;
	Fighter& operator=(Fighter&& other) = delete;

	void Draw() const override;
	void Update(float elapsedSec, const Level* pLevel, const Rectf& cameraWindow, Avatar* pAvatar, ProjectileManager* pPManager, bool timePaused) override;

private:
	float m_AttackCooldown;

	static int s_InstanceCounter;


	void Attack(bool rightDirection);
};

