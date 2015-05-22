#ifndef SOUND_PROGRAMMING_H_
#define SOUND_PROGRAMMING_H_

#include "Application.h"
#include "Camera.h"

#include "fmod.hpp"
#include "glm/glm.hpp"

using glm::vec3;
using glm::vec4;
using glm::mat4;

class SoundProgramming : public Application
{
public:
    virtual bool startup();
    virtual void shutdown();
    virtual bool update();
    virtual void draw();
	bool	OnCreate();

    FlyCamera m_camera;

	FMOD::System*	m_pFmodSystem;
	FMOD::ChannelGroup*	m_pGeneralSoundsGroup;
	FMOD::Channel*	m_pMainChannel;
	FMOD::Channel*	m_pMusicChannel;
	FMOD::Sound*	m_pJaguarSound;
	FMOD::Sound*	m_pMusicSound;
//	FMOD::Sound*	m_pSound;
//	FMOD::Sound*	m_pSound;
//	FMOD::Sound*	m_pSound;
	FMOD::DSP* pEchoDSP;
	//	Audio Listener properties
	FMOD_VECTOR position;
	FMOD_VECTOR velocity;
	FMOD_VECTOR forward;
	FMOD_VECTOR up;

	//	GeneralSoundsGroup properties
	FMOD_VECTOR GenSoundPosition;
	FMOD_VECTOR GenSoundVelocity;

	vec4	m_vLastCamPos;
};

#endif //CAM_PROJ_H_