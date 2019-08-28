#include "SoundEngine.h"
#include <iostream>

using namespace std;
CSoundEngine* CSoundEngine::soundEngine = NULL;
// Constructor
CSoundEngine::CSoundEngine()
{
}

// Destructor
CSoundEngine::~CSoundEngine()
{
	// Clear out the sound map
	soundMap.clear();

	// Delete the sound engine
	if (theSoundEngine)
	{
		delete theSoundEngine;
		theSoundEngine = NULL;
	}
}

// Init this class and it will create the Sound Engine
bool CSoundEngine::Init(void)
{
	// Create the sound engine
	theSoundEngine = createIrrKlangDevice();
	if (!theSoundEngine)
		return false;	// error starting up the sound engine

	return true;
}

// Get the handler to the sound engine
ISoundEngine* CSoundEngine::GetSoundEngine(void)
{
	if (theSoundEngine == NULL)
	{
		if (Init() == false)
		{
			cout << "SoundEngine::GetSoundEngine() - The Sound Engine has not been initialised yet." << endl;
			return NULL;
		}
	}

	return theSoundEngine;
}

ISoundEngine * CSoundEngine::SetListenerPosition(vec3df _position, vec3df _target)
{
	theSoundEngine->setListenerPosition(_position, _target);
	return theSoundEngine;
}

// Add a sound to this library
void CSoundEngine::AddSound(const std::string& _soundIndex, const std::string& _soundSource)
{
	if (theSoundEngine == NULL)
	{
		if (Init() == false)
		{
			cout << "SoundEngine::AddSound() - The Sound Engine has not been initialised yet." << endl;
			return;
		}
	}

	// Clean up first if there is an existing goodie with the same name
	RemoveSound(_soundIndex);

	// Add the mesh now
	soundMap[_soundIndex] = _soundSource;
}

// Get a sound from this map 
std::string CSoundEngine::GetSound(const std::string& _soundIndex)
{
	if (theSoundEngine == NULL)
	{
		if (Init() == false)
		{
			cout << "SoundEngine::GetSound() - The Sound Engine has not been initialised yet." << endl;
			return std::string();
		}
	}

	if (soundMap.count(_soundIndex) != 0)
	{
		return soundMap[_soundIndex];
	}

	return std::string();
}

// Remove a sound from this map
bool CSoundEngine::RemoveSound(const std::string& _soundIndex)
{
	if (theSoundEngine == NULL)
	{
		if (Init() == false)
		{
			cout << "SoundEngine::RemoveSound() - The Sound Engine has not been initialised yet." << endl;
			return NULL;
		}
	}

	std::string aSoundName = GetSound(_soundIndex);
	if (!aSoundName.empty())
	{
		soundMap.erase(_soundIndex);
		return true;
	}
	return false;
}

// Play a sound from this map
void CSoundEngine::PlayASound(const std::string& _soundIndex)
{
	std::string aSound = GetSound(_soundIndex);
	// Play a sound
	theSoundEngine->play2D(aSound.c_str(), false, false);
}

void CSoundEngine::Play3D(const std::string & _soundIndex, vec3df _position)
{
	theSoundEngine->play3D(GetSound(_soundIndex).c_str(), _position, false, false, false);
}
bool CSoundEngine::CurentlyPlaying(const std::string & _soundIndex)
{
	return theSoundEngine->isCurrentlyPlaying(GetSound(_soundIndex).c_str());
}
void CSoundEngine::setSoundVolume(float volume)
{
	theSoundEngine->setSoundVolume(volume);
}
void CSoundEngine::SetmaxDist3D(float volume)
{
	theSoundEngine->setDefault3DSoundMaxDistance(volume);
}
void CSoundEngine::SetminDist3D(float volume)
{
	theSoundEngine->setDefault3DSoundMinDistance(volume);
}
void CSoundEngine::StopSound()
{
	theSoundEngine->stopAllSounds();
}
