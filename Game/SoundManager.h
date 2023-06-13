#pragma once
#include <map>

class SoundEffect;
class SoundStream;

class SoundManager final
{
public:
	explicit SoundManager();
	~SoundManager();
	SoundManager& operator=(const SoundManager& rhs) = delete;
	SoundManager(const SoundManager& other) = delete;
	SoundManager& operator=(SoundManager&& rhs) = delete;
	SoundManager(SoundManager&& other) = delete;

	void PlayEffect(const std::string& fileName) const;
	void LoadEffect(const std::string& fileName);
	void DeleteEffect(const std::string& fileName);

	void PlayStream(const std::string& fileName) const;
	void PauseStream() const;
	void LoadStream(const std::string& fileName);
	void DeleteStream(const std::string& fileName);

private:
	std::map<std::string, SoundEffect*> m_EffectMap;
	std::map<std::string, SoundStream*> m_StreamMap;
};

