#pragma once
#include "Projectile.h"

class TextureManager;

class ThrowingKnife final: public Projectile
{
public:
	explicit ThrowingKnife(TextureManager* TM, Point2f spawnPos);
	~ThrowingKnife() override;

	ThrowingKnife(const ThrowingKnife& other) = delete;
	ThrowingKnife& operator=(const ThrowingKnife& other) = delete;
	ThrowingKnife(ThrowingKnife&& other) = delete;
	ThrowingKnife& operator=(ThrowingKnife&& other) = delete;

	void Draw() const override;
	void Update(float elapsedSec, const Rectf& cameraWindow, std::vector<Enemy*>&, Avatar* pAvatar) override;


	void Destroy() override;

private:
	static int s_InstanceCounter;
	std::string m_ExpTextureStr;

	bool m_Exploding;
	int m_AnimFrame;
	float m_AnimTime;

	void DoCollision(Avatar* pAvatar);
};

