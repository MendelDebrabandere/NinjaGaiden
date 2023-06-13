#pragma once
#include "Enemy.h"

class ProjectileManager;

class KnifeMan final: public Enemy
{
public:
	explicit KnifeMan(SoundManager* SM, TextureManager* TM, const Point2f& spawnPos);
	virtual ~KnifeMan();

	KnifeMan(const KnifeMan& other) = delete;
	KnifeMan& operator=(const KnifeMan& other) = delete;
	KnifeMan(KnifeMan&& other) = delete;
	KnifeMan& operator=(KnifeMan&& other) = delete;

	virtual void Draw() const override;
	virtual void Update(float elapsedSec, const Level* pLevel, const Rectf& cameraWindow, Avatar* pAvatar, ProjectileManager* pPManager, bool timePaused) override;

private:
	float m_SecondsTillNextAttack;
	bool m_NextAttackChrouch;

	static int s_InstanceCounter;


	void Attack(const Avatar* pAvatar, ProjectileManager* pPManager);
};

