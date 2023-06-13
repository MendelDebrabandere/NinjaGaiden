#include "pch.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "Biker.h"
#include "KnifeMan.h"
#include "Fighter.h"
#include "Dog.h"
#include "Boss.h"
#include <fstream>

EnemyManager::EnemyManager(SoundManager* SM, TextureManager* TM, bool bossRoom)
	: m_BossRoom {bossRoom}
{
	ReadAndInitEnemies(SM, TM, bossRoom);
	//if (!bossRoom)
	//{
		//m_EnemyPtrs.resize(26);

		//m_EnemyPtrs[0] = new Biker(SM, TM, Point2f{ 268, 20 });
		//m_EnemyPtrs[1] = new Biker(SM, TM, Point2f{ 300, 20 });
		//m_EnemyPtrs[2] = new Biker(SM, TM, Point2f{ 332, 20 });
		//m_EnemyPtrs[3] = new Biker(SM, TM, Point2f{ 537, 20 });
		//m_EnemyPtrs[4] = new Biker(SM, TM, Point2f{ 760, 131 });
		//m_EnemyPtrs[5] = new Biker(SM, TM, Point2f{ 810, 131 });
		//m_EnemyPtrs[6] = new Biker(SM, TM, Point2f{ 1320, 20 });
		//m_EnemyPtrs[7] = new Biker(SM, TM, Point2f{ 1510, 87 });
		//m_EnemyPtrs[8] = new Biker(SM, TM, Point2f{ 2215, 150 });
		//m_EnemyPtrs[9] = new Biker(SM, TM, Point2f{ 2380, 38 });
		//m_EnemyPtrs[10] = new Biker(SM, TM, Point2f{ 2470, 38 });
		//m_EnemyPtrs[11] = new KnifeMan(SM, TM, Point2f{ 645, 20 });
		//m_EnemyPtrs[12] = new KnifeMan(SM, TM, Point2f{ 985, 20 });
		//m_EnemyPtrs[13] = new KnifeMan(SM, TM, Point2f{ 2612, 20 });
		//m_EnemyPtrs[14] = new Fighter(SM, TM, Point2f{ 1100, 90 });
		//m_EnemyPtrs[15] = new Fighter(SM, TM, Point2f{ 1908, 38 });
		//m_EnemyPtrs[16] = new Fighter(SM, TM, Point2f{ 1972, 38 });
		//m_EnemyPtrs[17] = new Fighter(SM, TM, Point2f{ 2090, 20 });
		//m_EnemyPtrs[18] = new Fighter(SM, TM, Point2f{ 2855, 20 });
		//m_EnemyPtrs[19] = new Fighter(SM, TM, Point2f{ 2915, 20 });
		//m_EnemyPtrs[20] = new Fighter(SM, TM, Point2f{ 2975, 20 });
		//m_EnemyPtrs[21] = new Dog(SM, TM, Point2f{ 365, 20 }, true);
		//m_EnemyPtrs[22] = new Dog(SM, TM, Point2f{ 740, 38 }, false);
		//m_EnemyPtrs[23] = new Dog(SM, TM, Point2f{ 1170, 20 }, true);
		//m_EnemyPtrs[24] = new Dog(SM, TM, Point2f{ 1692, 20 }, false);
		//m_EnemyPtrs[25] = new Dog(SM, TM, Point2f{ 2650, 20 }, true);
		//// 11 bikers
		//// 3 knifeMan
		//// 7 fighter
		//// 5 dog
	//}
	//else
	//{
		//m_EnemyPtrs.resize(1);
		//m_EnemyPtrs[0] = new Boss(SM, TM, Point2f{ 170, 20 });
	//}
}

EnemyManager::~EnemyManager()
{
	for (int idx{}; idx < m_EnemyPtrs.size(); idx++)
	{
		delete m_EnemyPtrs[idx];
	}
}

void EnemyManager::Draw() const
{
	for (int idx{}; idx < m_EnemyPtrs.size(); idx++)
	{
		m_EnemyPtrs[idx]->Draw();
	}
}

void EnemyManager::Update(float elapsedSec, const Level* pLevel, const Rectf& cameraWindow, Avatar* pAvatar, ProjectileManager* pPManager, bool timePaused)
{
	for (int idx{}; idx < m_EnemyPtrs.size(); idx++)
	{
		m_EnemyPtrs[idx]->Update(elapsedSec, pLevel, cameraWindow, pAvatar, pPManager, timePaused);
	}
}

std::vector<Enemy*>& EnemyManager::GetEnemies()
{
	return m_EnemyPtrs;
}

int EnemyManager::GetHP() const
{
	if (!m_BossRoom)
	{
		return 16;
	}
	else
	{
		return m_EnemyPtrs[0]->GetHP();
	}
}

void EnemyManager::ReadAndInitEnemies(SoundManager* SM, TextureManager* TM, bool bossRoom)
{
	std::string location;
	if (!bossRoom)  location = "resources/objectdata/mainstageenemies.txt";
	else location = "resources/objectdata/BossRoomEnemies.txt";

	std::ifstream is {location};
	std::string line{};
	while (std::getline(is, line))
	{
		if (line.find("Biker") != std::string::npos)
		{
			float xPos{ std::stof(line.substr(line.find("= ") + 2, line.find(", ")))};
			float yPos{ std::stof(line.substr(line.find(", ") + 2)) };
			m_EnemyPtrs.push_back(new Biker(SM, TM, Point2f{ xPos, yPos }));
		}
		else if (line.find("KnifeMan") != std::string::npos)
		{
			float xPos{ std::stof(line.substr(line.find("= ") + 2, line.find(", "))) };
			float yPos{ std::stof(line.substr(line.find(", ") + 2)) };
			m_EnemyPtrs.push_back(new KnifeMan(SM, TM, Point2f{ xPos, yPos }));
		}
		else if (line.find("Fighter") != std::string::npos)
		{
			float xPos{ std::stof(line.substr(line.find("= ") + 2, line.find(", "))) };
			float yPos{ std::stof(line.substr(line.find(", ") + 2)) };
			m_EnemyPtrs.push_back(new Fighter(SM, TM, Point2f{ xPos, yPos }));
		}
		else if (line.find("Dog") != std::string::npos)
		{
			float xPos{ std::stof(line.substr(line.find("= ") + 2, line.find(", "))) };
			float yPos{ std::stof(line.substr(line.find(", ") + 2, line.find("; "))) };

			bool facingRight{ line.substr(line.find("; ") + 2) == "true" };

			m_EnemyPtrs.push_back(new Dog(SM, TM, Point2f{ xPos, yPos }, facingRight));
		}
		else if (line.find("Boss") != std::string::npos)
		{
			float xPos{ std::stof(line.substr(line.find("= ") + 2, line.find(", "))) };
			float yPos{ std::stof(line.substr(line.find(", ") + 2)) };
			m_EnemyPtrs.push_back(new Boss(SM, TM, Point2f{ xPos, yPos }));
		}
	}
}