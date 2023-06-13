#pragma once
#include "Vector2f.h"

class Texture;
class TextureManager;
class Level;
class Avatar;
class SoundManager;

class Powerup final
{
public:
	explicit Powerup(SoundManager* SM, TextureManager* TM, Point2f spawnPos, int powerup);
	~Powerup();

	Powerup(const Powerup& other) = delete;
	Powerup& operator=(const Powerup& other) = delete;
	Powerup(Powerup&& other) = delete;
	Powerup& operator=(Powerup&& other) = delete;

	void Draw() const;
	bool Update(float elapsedSec, const Rectf& cameraWindow, const Level* pLevel, Avatar* pAvatar);
	// This bool is true when time needs to be stopped

	const Texture* GetTexture() const;
	Rectf GetLanternShape() const;
	void Drop();
private:
	enum class m_Powerup
	{
		bonus500 = 1,
		bonus1000 = 2,
		energy5 = 3,
		energy10 = 4,
		star = 5,
		fireWheel = 6,
		jumpSlash = 7,
		invincibleFireWheel = 8,
		windmillStar = 9,
		timeFreeze = 10
	};
	enum class m_State
	{
		lantern,
		dropped,
		gone
	};

	Rectf m_Shape;
	m_Powerup m_Power;
	m_State m_CurrentState;
	float m_DroppedTimer;
	Vector2f m_Velocity;
	bool m_IsInFrame;

	const int m_NrFramesPerSec{ 2 };
	const int m_NrOfFrames{ 2 };
	
	float m_AnimTime;
	int m_AnimFrame;

	static int s_InstanceCounter;

	TextureManager* m_pTM;
	std::string m_TextureStr;

	SoundManager* m_pSM;
	std::string m_PickupSoundStr;

	void LanternCollisionCheck(const Avatar* pAvatar);
	bool DroppedCollisionCheck(Avatar* pAvatar);
	// This bool is true when time needs to be stopped

	void CheckSpawning(const Rectf& cameraWindow);
};

