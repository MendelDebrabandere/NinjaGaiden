#include "pch.h"
#include "PowerupManager.h"
#include "Powerup.h"
#include <fstream>

PowerupManager::PowerupManager(SoundManager* SM, TextureManager* TM, bool bossRoom)
{
	ReadAndInitPowerups(SM, TM, bossRoom);

	//if (!bossRoom)
	//{
	//	m_PowerupPtrs.resize(18);

	//	m_PowerupPtrs[0] = new Powerup(SM, TM, Point2f{ 380, 57 }, 3);
	//	m_PowerupPtrs[1] = new Powerup(SM, TM, Point2f{ 443, 57 }, 3);
	//	m_PowerupPtrs[2] = new Powerup(SM, TM, Point2f{ 649, 87 }, 4);
	//	m_PowerupPtrs[3] = new Powerup(SM, TM, Point2f{ 750, 150 }, 5);
	//	m_PowerupPtrs[4] = new Powerup(SM, TM, Point2f{ 845, 87 }, 1);
	//	m_PowerupPtrs[5] = new Powerup(SM, TM, Point2f{ 1032, 119 }, 3);
	//	m_PowerupPtrs[6] = new Powerup(SM, TM, Point2f{ 1085, 119 }, 3);
	//	m_PowerupPtrs[7] = new Powerup(SM, TM, Point2f{ 1128, 57 }, 2);
	//	m_PowerupPtrs[8] = new Powerup(SM, TM, Point2f{ 1463, 57 }, 1);
	//	m_PowerupPtrs[9] = new Powerup(SM, TM, Point2f{ 1515, 150 }, 6);
	//	m_PowerupPtrs[10] = new Powerup(SM, TM, Point2f{ 1593, 150 }, 7);
	//	m_PowerupPtrs[11] = new Powerup(SM, TM, Point2f{ 1689, 71 }, 3);
	//	m_PowerupPtrs[12] = new Powerup(SM, TM, Point2f{ 1738, 104 }, 8);
	//	m_PowerupPtrs[13] = new Powerup(SM, TM, Point2f{ 1961, 150 }, 10);
	//	m_PowerupPtrs[14] = new Powerup(SM, TM, Point2f{ 2010, 71 }, 1);
	//	m_PowerupPtrs[15] = new Powerup(SM, TM, Point2f{ 2250, 100 }, 9);
	//	m_PowerupPtrs[16] = new Powerup(SM, TM, Point2f{ 2250, 60 }, 3);
	//	m_PowerupPtrs[17] = new Powerup(SM, TM, Point2f{ 2570, 71 }, 3);
	//}
	//else
	//{
	//	m_PowerupPtrs.resize(1);
	//	m_PowerupPtrs[0] = new Powerup(SM, TM, Point2f{ 2570, 71 }, 3);
	//}
}

PowerupManager::~PowerupManager()
{
	for (int i{}; i < m_PowerupPtrs.size(); i++)
	{
		delete m_PowerupPtrs[i];
		m_PowerupPtrs[i] = nullptr;
	}
}

void PowerupManager::Draw() const
{
	for (int i{}; i < m_PowerupPtrs.size(); i++)
	{
		m_PowerupPtrs[i]->Draw();
	}
}

bool PowerupManager::Update(float elapsedSec, const Rectf& cameraWindow, const Level* pLevel, Avatar* pAvatar)
{
	bool flag{ false };
	for (int i{}; i < m_PowerupPtrs.size(); i++)
	{
		if (m_PowerupPtrs[i]->Update(elapsedSec, cameraWindow, pLevel, pAvatar)) flag = true;
	}
	return flag;
}

const Texture* PowerupManager::GetTexture() const
{
	if (m_PowerupPtrs.size() >= 1)
	{
		return m_PowerupPtrs[0]->GetTexture();
	}
	else
	{
		return nullptr;
	}
}

std::vector<Powerup*>& PowerupManager::GetPowerups()
{
	return m_PowerupPtrs;
}

void PowerupManager::ReadAndInitPowerups(SoundManager* SM, TextureManager* TM, bool bossRoom)
{
	std::string location;
	if (!bossRoom)  location = "resources/objectdata/mainstagepowerups.txt";
	else location = "resources/objectdata/BossRoompowerups.txt";

	std::ifstream is{ location };
	std::string line{};
	while (std::getline(is, line))
	{
		if (line.size() > 1)
		{
			float xPos{ std::stof(line.substr(line.find("; ") + 2, line.find(", "))) };
			float yPos{ std::stof(line.substr(line.find(", ") + 2)) };

			int powerupVal{ std::stoi(line.substr(0,line.find("; "))) };

			m_PowerupPtrs.push_back(new Powerup(SM, TM, Point2f{ xPos, yPos }, powerupVal));
		}
	}
}
