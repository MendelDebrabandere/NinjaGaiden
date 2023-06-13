#pragma once
#include <vector>

class Avatar;
class Powerup;
class Level;
class Texture;
class TextureManager;
class SoundManager;

class PowerupManager final
{
public:
	explicit PowerupManager(SoundManager* SM, TextureManager* TM, bool bossRoom);
	~PowerupManager();

	PowerupManager(const PowerupManager& other) = delete;
	PowerupManager& operator=(const PowerupManager& other) = delete;
	PowerupManager(PowerupManager&& other) = delete;
	PowerupManager& operator=(PowerupManager&& other) = delete;

	void Draw() const;
	bool Update(float elapsedSec, const Rectf& cameraWindow, const Level* pLevel, Avatar* pAvatar);

	const Texture* GetTexture() const;
	std::vector<Powerup*>& GetPowerups();

private:
	void ReadAndInitPowerups(SoundManager* SM, TextureManager* TM, bool bossRoom);

	std::vector<Powerup*> m_PowerupPtrs;
};

