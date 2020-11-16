#include "Audio.h"

bool KdSoundEffect::Load(const std::string& filename)
{
	if (KD_AUDIO.GetAudioEngine().get() != nullptr)
	{
		try
		{
			std::wstring wFilename = sjis_to_wide(filename);

			m_soundEffect =
				std::make_unique<DirectX::SoundEffect>
				(KD_AUDIO_ENGINE.get(), wFilename.c_str());
		}
		catch (...)
		{
			assert(0 && "Sound File Load Error");
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

void KdSoundInstance::Init(const std::shared_ptr<KdSoundEffect>& soundEffect)
{
	if (!soundEffect) { return; }

	DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags =
		DirectX::SoundEffectInstance_Default;
	m_instance = (soundEffect->CreateInstance(flags));
	m_soundData = soundEffect;
}

void KdSoundInstance::Play()
{
	if (!m_instance) { return; }
	m_instance->Play();

	KD_AUDIO.AddPlayerList(shared_from_this());
}

bool KdAudioManager::Play(const std::string& rName)
{
	if (!m_audioEng) { return false; }

	std::shared_ptr<KdSoundEffect> soundData = KdResFac.GetSound(rName);

	if (!soundData) { return false; }

	std::shared_ptr<KdSoundInstance> instance = std::make_shared<KdSoundInstance>();

	if (!instance) { return false; }

	instance->Init(soundData);
	instance->Play();

	return true;
}

bool KdAudioManager::Init()
{
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;

	m_audioEng = std::make_unique<DirectX::AudioEngine>(eflags);
	m_audioEng->SetReverb(DirectX::Reverb_Default);

	return true;
}

void KdAudioManager::Release()
{
}

void KdAudioManager::Update()
{
	if (m_audioEng != nullptr)
	{
		m_audioEng->Update();
	}

	for (auto iter = m_playList.begin(); iter != m_playList.end();)
	{
		if (!iter->second->IsPlaying())
		{
			iter = m_playList.erase(iter);
			continue;
		}

		++iter;
	}
}
