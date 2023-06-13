#pragma once
#include <vector>

class Enemy;
class Avatar;
class Projectile;
class Powerup;
class TextureManager;

class ProjectileManager final
{
public:
	explicit ProjectileManager(TextureManager* TM);
	~ProjectileManager();

	ProjectileManager(const ProjectileManager& other) = delete;
	ProjectileManager& operator=(const ProjectileManager& other) = delete;
	ProjectileManager(ProjectileManager&& other) = delete;
	ProjectileManager& operator=(ProjectileManager&& other) = delete;

	void Draw() const;
	void Update(float elapsedSec, const Rectf& cameraWindow, std::vector<Enemy*>&, Avatar* pAvatar, std::vector<Powerup*>& pPowerups);

	void RequestKnife(Point2f spawnPos, bool facingRight);
	void RequestStar(Point2f spawnPos, bool facingRight);
	void RequestArtOfTheFireWheel(Point2f spawnPos, bool facingRight);
	void RequestWindmillThrowingStar (Point2f spawnPos, bool facingRight);

private:
	std::vector<Projectile*> m_ProjectilePtrs;

	void CollisionBetweenProjectiles();
};

