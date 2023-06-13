#include "pch.h"
#include "TextureManager.h"
#include "Texture.h"

TextureManager::TextureManager()
{
	m_TexturesMap.clear();
}

TextureManager::~TextureManager()
{
	for (const std::pair<std::string, Texture*>& p : m_TexturesMap)
	{
		delete p.second;
	}
	m_TexturesMap.clear();
}

Texture* TextureManager::GetTexture(const std::string& fileName) const
{
	if (m_TexturesMap.contains(fileName))
	{
		return m_TexturesMap.find(fileName)->second;
	}
	return nullptr;
}

void TextureManager::LoadTexture(const std::string& fileName)
{
	if (!m_TexturesMap.contains(fileName))
	{
		m_TexturesMap.emplace(fileName, new Texture{ fileName });
	}
}

void TextureManager::LoadTexture(const std::string& text, const std::string& fontLocation, int fontSize, const Color4f& fontColor)
{
	if (!m_TexturesMap.contains(text))
	{
		m_TexturesMap.emplace(text, new Texture{ text, fontLocation, fontSize, fontColor });
	}
}

void TextureManager::DeleteTexture(const std::string& fileName)
{
	if (m_TexturesMap.contains(fileName))
	{
		delete m_TexturesMap[fileName];
	}
	m_TexturesMap.erase(fileName);
}

