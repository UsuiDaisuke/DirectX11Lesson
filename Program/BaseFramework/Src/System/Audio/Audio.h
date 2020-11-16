#pragma once

class KdSoundEffect
{
public:

	KdSoundEffect() {}

	~KdSoundEffect() {
		m_soundEffect = nullptr;
	}

	bool Load(const std::string& filename);

	std::unique_ptr<DirectX::SoundEffectInstance> CreateInstance
	(DirectX::SOUND_EFFECT_INSTANCE_FLAGS flag)
	{
		if (!m_soundEffect) { return nullptr; }

		return m_soundEffect->CreateInstance(flag);
	}

private:

	std::unique_ptr<DirectX::SoundEffect> m_soundEffect;

	KdSoundEffect(const KdSoundEffect& src) = delete;
	void operator=(const KdSoundEffect& src) = delete;
};

class KdSoundInstance : public std::enable_shared_from_this<KdSoundInstance>
{
public:

	KdSoundInstance() {}

	void Init(const std::shared_ptr<KdSoundEffect>& soundEffect);

	void Play();
	bool KdSoundInstance::IsPlaying()
	{
		if (!m_instance) { return false; }

		return (m_instance->GetState() == DirectX::SoundState::PLAYING);
	}

private:

	std::unique_ptr<DirectX::SoundEffectInstance>
		m_instance;

	std::shared_ptr<KdSoundEffect>
		m_soundData;

	KdSoundInstance(const KdSoundInstance& src) = delete;
	void operator=(const KdSoundInstance& src) = delete;

};

class KdAudioManager
{
public:
	bool Init();
	void Release();

	void Update();
	void AddPlayerList(const std::shared_ptr<KdSoundInstance>& rSound)
	{
		if (!rSound.get()) { return; }
		m_playList[(UINT)rSound.get()] = rSound;
	}

	std::unique_ptr<DirectX::AudioEngine>&
		GetAudioEngine() { return m_audioEng; }

	bool Play(const std::string& rName);

private:

	std::map<UINT, std::shared_ptr<KdSoundInstance>> m_playList;

	std::unique_ptr<DirectX::AudioEngine> m_audioEng;

public:

	static KdAudioManager& GetInstance()
	{
		static KdAudioManager instance;
		return instance;
	}

private:

	KdAudioManager() {}
};

#define KD_AUDIO KdAudioManager::GetInstance()
#define KD_AUDIO_ENGINE KD_AUDIO.GetAudioEngine()