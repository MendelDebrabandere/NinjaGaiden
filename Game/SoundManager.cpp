#include "pch.h"
#include "SoundManager.h"
#include "SoundEffect.h"
#include "SoundStream.h"
#include <iostream>

SoundManager::SoundManager()
	:m_EffectMap{}
{
}

SoundManager::~SoundManager()
{
	for (const std::pair<std::string, SoundEffect*>& p : m_EffectMap)
	{
		delete p.second;
	}
	m_EffectMap.clear();

	for (const std::pair<std::string, SoundStream*>& p : m_StreamMap)
	{
		delete p.second;
	}
	m_StreamMap.clear();
}

void SoundManager::PlayEffect(const std::string& fileName) const
{
	if (m_EffectMap.contains(fileName))
	{
		m_EffectMap.find(fileName)->second->Play(0);
	}
	else
	{
		std::cout << "SoundManager: Tried to play soundeffect [" << fileName << "] but wasn't initialized correctly\n";
	}
}

void SoundManager::LoadEffect(const std::string& fileName)
{
	if (!m_EffectMap.contains(fileName))
	{
		m_EffectMap.emplace(fileName, new SoundEffect( fileName ));
		m_EffectMap.find(fileName)->second->SetVolume(20);
	}
}

void SoundManager::DeleteEffect(const std::string& fileName)
{
	if (m_EffectMap.contains(fileName))
	{
		delete m_EffectMap[fileName];
	}
	m_EffectMap.erase(fileName);
}

void SoundManager::PlayStream(const std::string& fileName) const
{
	if (m_StreamMap.contains(fileName))
	{
		m_StreamMap.find(fileName)->second->Play(true);
	}
	else
	{
		std::cout << "SoundManager: Tried to play soundstream " << fileName << " but wasn't initialized correctly\n";
	}
}

void SoundManager::PauseStream() const
{
	for (const std::pair<std::string, SoundStream*>& element : m_StreamMap)
	{
		element.second->Pause();
	}
}


void SoundManager::LoadStream(const std::string& fileName)
{
	if (!m_StreamMap.contains(fileName))
	{
		m_StreamMap.emplace(fileName, new SoundStream(fileName));
		m_StreamMap.find(fileName)->second->SetVolume(8);
	}
}

void SoundManager::DeleteStream(const std::string& fileName)
{
	if (m_StreamMap.contains(fileName))
	{
		delete m_StreamMap[fileName];
		m_StreamMap.erase(fileName);
	}
}
