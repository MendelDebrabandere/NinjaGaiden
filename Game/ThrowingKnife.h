#pragma once
#include "Projectile.h"

class TextureManager;

class ThrowingKnife final: public Projectile
{
public:
	explicit ThrowingKnife(TextureManager* TM, Point2f spawnPos);
	virtual ~ThrowingKnife();

	ThrowingKnife(const ThrowingKnife& other) = delete;
	ThrowingKnife& operator=(const ThrowingKnife& other) = delete;
	ThrowingKnife(ThrowingKnife&& other) = delete;
	ThrowingKnife& operator=(ThrowingKnife&& other) = delete;

	virtual void Draw() const;
	virtual void Update(float elapsedSec, const Rectf& cameraWindow, std::vector<Enemy*>&, Avatar* pAvatar);


	virtual void Destroy() override;

private:
	static int s_InstanceCounter;
	std::string m_ExpTextureStr;

	bool m_Exploding;
	int m_AnimFrame;
	float m_AnimTime;

	void DoCollision(Avatar* pAvatar);
};

