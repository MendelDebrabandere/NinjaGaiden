#pragma once
#include "Projectile.h"

class Texturemanager;

class ArtOfTheFireWheel final: public Projectile
{
public:
	explicit ArtOfTheFireWheel(TextureManager* TM, Point2f spawnPos);
	virtual ~ArtOfTheFireWheel();

	ArtOfTheFireWheel(const ArtOfTheFireWheel& other) = delete;
	ArtOfTheFireWheel& operator=(const ArtOfTheFireWheel& other) = delete;
	ArtOfTheFireWheel(ArtOfTheFireWheel&& other) = delete;
	ArtOfTheFireWheel& operator=(ArtOfTheFireWheel&& other) = delete;


	virtual void Draw() const;
	virtual void Update(float elapsedSec, const Rectf& cameraWindow, std::vector<Enemy*>&, Avatar* pAvatar);

private:
	static int s_InstanceCounter;
	TextureManager* m_pTM;
	std::string m_TextureStr;

	Rectf m_Shape1;
	Rectf m_Shape2;
	Rectf m_Shape3;

	int m_AnimFrame;
	float m_AnimTime;

	void DoCollision(Enemy* pEnemy);


};

