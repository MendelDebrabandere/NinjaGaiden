#pragma once
#include "Enemy.h"

class Biker final: public Enemy
{
public:
	explicit Biker(SoundManager* SM, TextureManager* TM, const Point2f& spawnPos);
	~Biker() override;

	Biker(const Biker& other) = delete;
	Biker& operator=(const Biker& other) = delete;
	Biker(Biker&& other) = delete;
	Biker& operator=(Biker&& other) = delete;


	void Draw() const override;
	void Update(float elapsedSec, const Level* pLevel, const Rectf& cameraWindow, Avatar* pAvatar, ProjectileManager* pPManager, bool timePaused) override;

private:
	static int s_InstanceCounter;


};

