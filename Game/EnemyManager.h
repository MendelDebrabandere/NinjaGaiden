#pragma once
#include <vector>

class Enemy;
class Level;
class Avatar;
class ProjectileManager;
class TextureManager;
class SoundManager;

class EnemyManager final
{
public:
	explicit EnemyManager(SoundManager* SM, TextureManager* TM, bool bossRoom);
	~EnemyManager();

	EnemyManager(const EnemyManager& other) = delete;
	EnemyManager& operator=(const EnemyManager& other) = delete;
	EnemyManager(EnemyManager&& other) = delete;
	EnemyManager& operator=(EnemyManager&& other) = delete;

	void Draw() const;
	void Update(float elapsedSec, const Level* pLevel, const Rectf& cameraWindow, Avatar* pAvatar, ProjectileManager* pPManager, bool timePaused);

	int GetHP() const;

	std::vector<Enemy*>& GetEnemies();
	// Not const because it returns by reference,
	// The caller of this function wants to change the enemies.

private:
	void ReadAndInitEnemies(SoundManager* SM, TextureManager* TM, bool bossRoom);

	std::vector<Enemy*> m_EnemyPtrs;

	bool m_BossRoom;
};