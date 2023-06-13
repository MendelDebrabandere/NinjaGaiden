#pragma once
#include <vector>

class Texture;
class TextureManager;

class HUD final
{
public:
	explicit HUD(TextureManager* TM, bool bossRoom);
	~HUD();

	HUD(const HUD& other) = delete;
	HUD& operator=(const HUD& other) = delete;
	HUD(HUD&& other) = delete;
	HUD& operator=(HUD&& other) = delete;

	void Draw(const Texture* pPowerupTexture) const;
	void Update(float timer, int score, int ninjaHealth, int lives, int enemyHP, int energy, int powerup);

private:
	int m_Score;
	int m_Stage;
	float m_Timer;
	int m_NinjaHP;
	int m_EnemyHP;
	int m_Lives;
	int m_Energy;
	int m_Powerup;

	TextureManager* m_pTM;

	std::vector<std::string> m_TextureStr;
	std::string m_EnrgyTextureStr;
	std::string m_PowerupBorderTexture;
};

