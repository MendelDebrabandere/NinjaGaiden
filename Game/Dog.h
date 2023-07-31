#pragma once
#include "Enemy.h"

class Dog final: public Enemy
{
public:
	explicit Dog(SoundManager* SM, TextureManager* TM, const Point2f& spawnPos, bool spawnBehindYou);
	~Dog() override;

	Dog(const Dog& other) = delete;
	Dog& operator=(const Dog& other) = delete;
	Dog(Dog&& other) = delete;
	Dog& operator=(Dog&& other) = delete;


	void Draw() const override;
	void Update(float elapsedSec, const Level* pLevel, const Rectf& cameraWindow, Avatar* pAvatar, ProjectileManager* pPManager, bool timePaused) override;

private:
	bool m_Spawned;
	bool m_SpawnBehindYou;

	static int s_InstanceCounter;

	void CheckDogSpawning(const Rectf& cameraWindow, const Point2f& spawnPos, const Avatar* pAvatar);
};

