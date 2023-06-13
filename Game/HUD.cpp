#include "pch.h"
#include "HUD.h"
#include "utils.h"
#include "Texture.h"
#include "TextureManager.h"

HUD::HUD(TextureManager* TM, bool bossRoom)
	: m_Score{}
	, m_Stage{ 1 }
	, m_Timer{ 150.f }
	, m_NinjaHP{ 16 }
	, m_EnemyHP{ 16 }
	, m_Lives{ 2 }
	, m_Energy{ 45 }
	, m_Powerup{ 0 }
	, m_EnrgyTextureStr{ "resources/images/energyIcon.png" }
	, m_PowerupBorderTexture("resources/images/powerupborder.png")
	, m_pTM {TM}
{
	m_pTM->LoadTexture(m_EnrgyTextureStr);
	m_pTM->LoadTexture(m_PowerupBorderTexture);


	if (bossRoom) m_Stage = 2;

	m_TextureStr.resize(17);

	// Numbers
	for (int i{}; i < 10; i++)
	{
		m_TextureStr[i] = std::to_string(i);
	}

	// A char that repeats a lot
	m_TextureStr[10] = "-";

	// Words used in UI (these stay constant so no need to get every char seperate)
	m_TextureStr[11] = "SCORE";
	m_TextureStr[12] = "STAGE 1";
	m_TextureStr[13] = "TIMER";
	m_TextureStr[14] = "NINJA";
	m_TextureStr[15] = "ENEMY";
	m_TextureStr[16] = "P";


	Color4f fontColor{ 1,1,1,1 };
	std::string fontLocation{ "resources/fonts/font.ttf" };
	int fontSize{ 8 };

	for (int i{}; i < m_TextureStr.size(); i++)
	{
		m_pTM->LoadTexture(m_TextureStr[i], fontLocation, fontSize, fontColor);
	}
}

HUD::~HUD()
{
	for (int i{}; i < m_TextureStr.size(); i++)
	{
		m_pTM->DeleteTexture(m_TextureStr[i]);
	}
	m_pTM->DeleteTexture(m_EnrgyTextureStr);
	m_pTM->DeleteTexture(m_PowerupBorderTexture);
}

void HUD::Draw(const Texture* pPowerupTexture) const
{
	float numberWidth{ 8 };
	float dashWidth{ 8 };

	// Drawing the score
	Point2f leftPoint{ 25, 200 };
	// SCORE
	m_pTM->GetTexture(m_TextureStr[11])->Draw(leftPoint);
	leftPoint.x += m_pTM->GetTexture(m_TextureStr[11])->GetWidth();
	// -
	m_pTM->GetTexture(m_TextureStr[10])->Draw(leftPoint);
	leftPoint.x += dashWidth;
	// numbers
	m_pTM->GetTexture(m_TextureStr[(m_Score % 1000000) / 100000])->Draw(leftPoint);
	leftPoint.x += numberWidth;
	m_pTM->GetTexture(m_TextureStr[(m_Score % 100000) / 10000])->Draw(leftPoint);
	leftPoint.x += numberWidth;
	m_pTM->GetTexture(m_TextureStr[(m_Score % 10000) / 1000])->Draw(leftPoint);
	leftPoint.x += numberWidth;
	m_pTM->GetTexture(m_TextureStr[(m_Score % 1000) / 100])->Draw(leftPoint);
	leftPoint.x += numberWidth;
	m_pTM->GetTexture(m_TextureStr[(m_Score % 100) / 10])->Draw(leftPoint);
	leftPoint.x += numberWidth;
	m_pTM->GetTexture(m_TextureStr[m_Score % 10])->Draw(leftPoint);

	// Drawing the timer
	leftPoint = Point2f{ 25, 192 };
	// TIMER
	m_pTM->GetTexture(m_TextureStr[13])->Draw(leftPoint);
	leftPoint.x += m_pTM->GetTexture(m_TextureStr[13])->GetWidth();
	// -
	m_pTM->GetTexture(m_TextureStr[10])->Draw(leftPoint);
	leftPoint.x += dashWidth;
	// numbers
	m_pTM->GetTexture(m_TextureStr[(int(m_Timer) % 1000) / 100])->Draw(leftPoint);
	leftPoint.x += numberWidth;
	m_pTM->GetTexture(m_TextureStr[(int(m_Timer) % 100) / 10])->Draw(leftPoint);
	leftPoint.x += numberWidth;
	m_pTM->GetTexture(m_TextureStr[int(m_Timer) % 10])->Draw(leftPoint);

	// Drawing the lives
	leftPoint = Point2f{ 25, 184 };
	// P
	m_pTM->GetTexture(m_TextureStr[16])->Draw(leftPoint);
	leftPoint.x += m_pTM->GetTexture(m_TextureStr[16])->GetWidth();
	// -
	m_pTM->GetTexture(m_TextureStr[10])->Draw(leftPoint);
	leftPoint.x += dashWidth;
	// numbers
	m_pTM->GetTexture(m_TextureStr[(int(m_Lives) % 100) / 10])->Draw(leftPoint);
	leftPoint.x += numberWidth;
	m_pTM->GetTexture(m_TextureStr[int(m_Lives) % 10])->Draw(leftPoint);
	leftPoint.x += numberWidth;

	// Drawing the energy
	leftPoint.x += numberWidth;
	m_pTM->GetTexture(m_EnrgyTextureStr)->Draw(leftPoint);
	leftPoint.x += m_pTM->GetTexture(m_EnrgyTextureStr)->GetWidth();
	// -
	m_pTM->GetTexture(m_TextureStr[10])->Draw(leftPoint);
	leftPoint.x += dashWidth;
	// numbers
	m_pTM->GetTexture(m_TextureStr[(int(m_Energy) % 100) / 10])->Draw(leftPoint);
	leftPoint.x += numberWidth;
	m_pTM->GetTexture(m_TextureStr[int(m_Energy) % 10])->Draw(leftPoint);
	leftPoint.x += numberWidth;

	// Drawing the powerup
	// border
	glPushMatrix();
	glTranslatef(leftPoint.x + 2, leftPoint.y + 1, 0);
	m_pTM->GetTexture(m_PowerupBorderTexture)->Draw();
	glTranslatef(0, 14, 0);
	glScalef(1, -1, 1);
	m_pTM->GetTexture(m_PowerupBorderTexture)->Draw();
	glTranslatef(26, 0, 0);
	glScalef(-1, 1, 1);
	m_pTM->GetTexture(m_PowerupBorderTexture)->Draw();
	glTranslatef(0, 14, 0);
	glScalef(1, -1, 1);
	m_pTM->GetTexture(m_PowerupBorderTexture)->Draw();
	glPopMatrix();
	// Powerup
	if (m_Powerup != 0 && m_Powerup != 8)
	{
		leftPoint.x += 7;
		Rectf srcRect{ pPowerupTexture->GetWidth() / 12 * (1 + m_Powerup),pPowerupTexture->GetHeight(), pPowerupTexture->GetWidth() / 12 , pPowerupTexture->GetHeight() };
		pPowerupTexture->Draw(leftPoint, srcRect);
	}

	// Drawing the stage
	leftPoint = Point2f{ 128, 200 };
	// STAGE 1
	m_pTM->GetTexture(m_TextureStr[12])->Draw(leftPoint);
	leftPoint.x += m_pTM->GetTexture(m_TextureStr[12])->GetWidth();
	// -
	m_pTM->GetTexture(m_TextureStr[10])->Draw(leftPoint);
	leftPoint.x += dashWidth;
	// number
	m_pTM->GetTexture(m_TextureStr[m_Stage])->Draw(leftPoint);

	// Drawing Ninja HP
	leftPoint = Point2f{ 128, 192 };
	// STAGE 1
	m_pTM->GetTexture(m_TextureStr[14])->Draw(leftPoint);
	leftPoint.x += m_pTM->GetTexture(m_TextureStr[14])->GetWidth();
	// -
	m_pTM->GetTexture(m_TextureStr[10])->Draw(leftPoint);
	leftPoint.x += dashWidth + 1;
	for (int i{}; i < 16; i++)
	{
		if (i < m_NinjaHP)
		{
			utils::SetColor(Color4f{ 254 / 255.f, 103 / 255.f, 144 / 255.f, 1 });
			utils::FillRect(leftPoint, 3, 7);
		}
		else
		{
			utils::SetColor(Color4f{ 1,1,1,1 });
			utils::FillRect(leftPoint, 3, 7);
			utils::SetColor(Color4f{ 0,0,0,1 });
			utils::FillRect(leftPoint.x + 1, leftPoint.y + 1, 1, 5);
		}
		leftPoint.x += 4;
	}

	// Drawing Enemy HP
	leftPoint = Point2f{ 128, 184 };
	// STAGE 1
	m_pTM->GetTexture(m_TextureStr[15])->Draw(leftPoint);
	leftPoint.x += m_pTM->GetTexture(m_TextureStr[15])->GetWidth();
	// -
	m_pTM->GetTexture(m_TextureStr[10])->Draw(leftPoint);
	leftPoint.x += dashWidth + 1;
	for (int i{}; i < 16; i++)
	{
		if (i < m_EnemyHP)
		{
			utils::SetColor(Color4f{ 254 / 255.f, 103 / 255.f, 144 / 255.f, 1 });
			utils::FillRect(leftPoint, 3, 7);
		}
		else
		{
			utils::SetColor(Color4f{ 1,1,1,1 });
			utils::FillRect(leftPoint, 3, 7);
			utils::SetColor(Color4f{ 0,0,0,1 });
			utils::FillRect(leftPoint.x + 1, leftPoint.y + 1, 1, 5);
		}
		leftPoint.x += 4;
	}
}

void HUD::Update(float timer, int score, int ninjaHealth, int lives, int enemyHP, int energy, int powerup)
{
	m_Timer = timer;
	m_Score = score;
	m_NinjaHP = ninjaHealth;
	m_Lives = lives;
	m_EnemyHP = enemyHP;
	m_Energy = energy;
	m_Powerup = powerup;

	if (m_Timer <= 0.f)
	{
		m_Timer = 0.f;
	}
}

