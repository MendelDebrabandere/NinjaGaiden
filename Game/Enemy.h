#pragma once
#include "Vector2f.h"

class Level;
class Avatar;
class Texture;
class ProjectileManager;
class TextureManager;
class SoundManager;

class Enemy
{
public:

	explicit Enemy();
	virtual ~Enemy();

	Enemy(const Enemy& other) = delete;
	Enemy& operator=(const Enemy& other) = delete;
	Enemy(Enemy&& other) = delete;
	Enemy& operator=(Enemy&& other) = delete;

	
	virtual void Draw() const = 0;
	virtual void Update(float elapsedSec, const Level* pLevel, const Rectf& cameraWindow, Avatar* pAvatar, ProjectileManager* pPManager, bool timePaused) = 0;

	int GetHP() const;
	Rectf GetShape() const;

	void TakeDamage();
protected:
	enum class m_ActionState {
		moving,
		dying,
		attacking,
		crouchAttacking
	};

	Rectf m_Shape;
	bool m_IsInFrame;
	bool m_IsDead;
	int m_HP;
	m_ActionState m_CurrentState;
	bool m_FacingRight;
	Point2f m_SpawnPos;
	float m_AnimTime;
	int m_AnimFrame;
	Vector2f m_Velocity;

	SoundManager* m_pSM;
	TextureManager* m_pTM;
	std::string m_TextureString;
	std::string m_DeathSoundStr;
	float m_WalkSpeed;
	int m_NrFramesPerSec;
	int m_NrOfFrames;



	void HandleHitboxes(Avatar* pAvatar, bool canTakeDamage);
	void CheckHP();
	void CheckSpawning(const Rectf& cameraWindow, const Point2f& spawnPos, const Avatar* pAvatar);
	void DrawExplosion() const;
	void UpdateExplosion(float elapsedSec);
	void LookAtPlayer(const Avatar* pAvatar);
private:

	std::string m_ExplosionTextureStr;

	static int s_InstanceCounter;
};

