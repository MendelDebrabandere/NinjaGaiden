#pragma once
#include <vector>
#include "Vector2f.h"

class Platform;
class Walls;
class SoundManager;
class TextureManager;

class Level final
{
public:
	explicit Level(SoundManager* SM, TextureManager* TM, bool bossRoom);
	~Level();
	Level& operator=(const Level& rhs) = delete;
	Level(const Level& other) = delete;
	Level& operator=(const Level&& other) = delete;
	Level(const Level&& other) = delete;

	void DrawBackground() const;
	bool HandleHorizontalCollision(Rectf& actorShape, Vector2f& actorVelocity) const;
	bool HandleVerticalCollision(Rectf& actorShape, Vector2f& actorVelocity) const;
	float GetLevelWidth() const;

	bool GetIsBossRoom() const;

	int IsFullEnemyOnPlatform(Rectf& actorShape) const;
	// This function returns ints beacuase there are 3 possibilities:
	// 0 -> fully on platform / not at all on platform
	// 1 -> left foot is not on the platform
	// 2 -> right foot is not on the platform

	bool BounceOfWall(Rectf& actorShape, Vector2f& actorVelocity) const;
	// Meant for enemies to interract with walls of the level but not climable walls

private:
	TextureManager* m_pTM;
	std::string m_BGTextureStr;
	std::vector<std::vector<Point2f>> m_Floor;
	Platform* m_pPlatforms;
	Walls* m_pWalls;
	SoundManager* m_pSM;
	std::string m_SoundStr;

	bool m_BossRoom;
};

