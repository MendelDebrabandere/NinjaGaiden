#pragma once
#include "Vector2f.h"

class Level;
class ProjectileManager;
class SoundManager;
class TextureManager;
class Texture;

class Avatar final
{
public:
	explicit Avatar(SoundManager* SM, TextureManager* TM);
	~Avatar();
	Avatar& operator=(const Avatar& rhs) = delete;
	Avatar(const Avatar& other) = delete;
	Avatar(Avatar&& other) = delete;
	Avatar& operator=(Avatar&& other) = delete;

	void Update(float elapsedSec, const Level* pLevel);
	void Draw() const;
	void Jump();
	void Attack();
	void TakeDamage(bool formRight);
	void AddScore(int amount);
	void PowerupPickedUp(int power);
	void UseItem(ProjectileManager* pPManager);
	void Reset();
	void FullReset();
	bool CheckIfDead();
	void SetPos(Point2f newPos);
	void Die();

	Rectf GetShape() const;
	Vector2f GetVelocity() const;
	bool IsAttacking() const;
	bool IsCrouchAttacking() const;
	bool IsjumpSlashing() const;
	bool IsFireWheelOn() const;
	bool GetFacingRight() const;
	bool GetClimbing() const;
	int GetHP() const;
	int GetScore() const;
	int GetLives() const;
	int GetEnergy() const;
	int GetPowerup() const;

private:
	enum class m_ActionState {
		idle,
		moving,
		hurt,
		falling,
		crouching,
		usingItem,
		climbing,
		attacking,
		crouchAttacking,
		jumpSlashing
	};

	TextureManager* m_pTM;
	SoundManager* m_pSM;
	std::string m_TextureStr;
	std::string m_InfFireWheelStr;
	std::string m_JumpSlashingStr;
	Rectf m_Shape;
	bool m_Slashing;
	float m_JumpSpeed;
	float m_WalkSpeed;
	Vector2f m_Velocity;
	float m_FallAccel;
	m_ActionState m_CurrentState;
	int m_NrOfFrames;
	int m_NrFramesPerSec;
	float m_AnimTime;
	int m_AnimFrame;
	bool m_FacingRight;
	Rectf m_srcRect;
	bool m_ChangedDirectionMidAir;
	float m_SecondsInAction;
	int m_HP;
	int m_Lives;
	int m_Score;
	int m_Energy;
	bool m_Untargetable;
	float m_SecondsUntargetable;
	int m_Powerup;
	float m_SecWInvincibleFireWheel;
	int m_SecWInvFireWheelInteger;
	float m_FireWheelAnimTime;
	int m_FireWheelAnimFrame;

	std::string m_SlashSoundStr;
	std::string m_JumpSoundStr;
	std::string m_DamageSoundStr;
	std::string m_ThrowStarSoundStr;
	std::string m_JumpSlashSoundStr;
	std::string m_FireWheelSoundStr;
	std::string m_InfFireWheelSoundStr;
	std::string m_WindmillStarSoundStr;

	bool DoMovement(const Level* level);
	void DoFalling(float elapsedSec);
	void UpdatePos(float elapsedSec);
	void UpdateSprite(float elapsedSec);
};

