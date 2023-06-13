#pragma once

class Level;
class Avatar;
class Camera;
class EnemyManager;
class ProjectileManager;
class HUD;
class PowerupManager;
class TextureManager;
class Texture;
class SoundManager;

class Game final
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	~Game();

	void Update( float elapsedSec );
	void Draw( ) const;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e );
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e );
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e );
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e );
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e );

private:
	// DATA MEMBERS
	const Window m_Window;
	Level* m_pLevel;
	Avatar* m_pAvatar;
	Camera* m_pCamera;
	EnemyManager* m_pEnemyManager;
	ProjectileManager* m_pProjectileManager;
	HUD* m_pHUD;
	PowerupManager* m_pPowerupManager;

	TextureManager* m_pTextureManager;
	SoundManager* m_pSoundManager;

	bool m_Jumped;
	bool m_Attacked;
	float m_Timer;
	bool m_TimePaused;
	float m_TimePausedTimer;
	int m_TimePausedTimerInt;

	bool m_Respawining;
	float m_RespawnTimer;
	bool m_GameOver;
	float m_GameOverTimer;

	std::string m_GameOverTextureStr;
	std::string m_GameOverSoundStr;
	std::string m_TimePausedSoundStr;

	bool m_IsInBossRoom;

	// FUNCTIONS
	void Initialize( );
	void Cleanup( );
	void ClearBackground( ) const;

	void DrawColorOverlays() const;
	void GameReset();
	void FullGameReset();

	void AdvanceLevel();
	void CheckAdvanceLevel();
};