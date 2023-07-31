#pragma once
#include "Enemy.h"

class Boss final: public Enemy
{
public:
	explicit Boss(SoundManager* SM, TextureManager* TM, const Point2f& spawnPos);
	~Boss() override;

	Boss(const Boss& other) = delete;
	Boss& operator=(const Boss& other) = delete;
	Boss(Boss&& other) = delete;
	Boss& operator=(Boss&& other) = delete;


	void Draw() const override;
	void Update(float elapsedSec, const Level* pLevel, const Rectf& cameraWindow, Avatar* pAvatar, ProjectileManager* pPManager, bool timePaused) override;

private:
	int m_NrFramesPerSec;
	int m_NrOfFrames;
	const float m_WalkSpeed{ 20.f };

	float m_AttackCooldown;
	int m_TimesAttacked;

	float m_InvincibilityCooldown;

	std::string m_ExplosionTextureString;
	std::string m_ExplosionSoundString;
	float m_BossExplosionTimer;


	void ResetShape();
	void DrawBossExplosion() const;
	void UpdateBossExplosion(float elapsedSec);
};