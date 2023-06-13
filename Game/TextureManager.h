#pragma once
#include <map>

class Texture;

class TextureManager final
{
public:
	explicit TextureManager();
	~TextureManager();
	TextureManager& operator=(const TextureManager& rhs) = delete;
	TextureManager(const TextureManager& other) = delete;
	TextureManager& operator=( TextureManager&& rhs) = delete;
	TextureManager( TextureManager&& other) = delete;

	Texture* GetTexture(const std::string& fileName) const;
	void LoadTexture(const std::string& fileName);
	void DeleteTexture(const std::string& filenName);

	void LoadTexture(const std::string& text, const std::string& fontLocation, int fontSize, const Color4f& fontColor);
private:
	std::map<std::string, Texture*> m_TexturesMap;
};

