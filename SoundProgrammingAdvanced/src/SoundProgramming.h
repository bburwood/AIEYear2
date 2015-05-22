#ifndef SOUND_PROGRAMMING_H_
#define SOUND_PROGRAMMING_H_

#include "Application.h"
#include "Camera.h"

#include "fmod_studio.hpp"
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

	FMOD::Studio::System*	m_pFmodSystem;
	FMOD::Studio::Bank*		m_pVehicleBank;
	FMOD::Studio::Bank*		m_pStringBank;
	FMOD::Studio::Bank*		m_pMasterBank;
	FMOD::Studio::Bank*		m_pSurroundAmbienceBank;
	FMOD::Studio::Bank*		m_pMusicBank;
	FMOD::Studio::Bank*		m_pWeaponsBank;
	FMOD::Studio::ID		m_EngineID;
	FMOD::Studio::ID		m_CarEngineID;
	FMOD::Studio::ID		m_SurroundAmbienceID;
	FMOD::Studio::ID		m_MusicID;
	FMOD::Studio::ID		m_WeaponsID;
	FMOD::Studio::EventDescription*	m_pEngineDescription;
	FMOD::Studio::EventDescription*	m_pCarEngineDescription;
	FMOD::Studio::EventDescription*	m_pSurroundAmbienceDescription;
	FMOD::Studio::EventDescription*	m_pMusicDescription;
	FMOD::Studio::EventDescription*	m_pWeaponsDescription;
	FMOD::Studio::EventInstance*	m_pEngineInstance;
	FMOD::Studio::EventInstance*	m_pCarEngineInstance;
	FMOD::Studio::EventInstance*	m_pSurroundAmbienceInstance;
	FMOD::Studio::EventInstance*	m_pMusicInstance;
	FMOD::Studio::EventInstance*	m_pWeaponsInstance;
	FMOD::Studio::ParameterInstance*	m_pEngineRPM;
	FMOD::Studio::ParameterInstance*	m_pCarEngineRPM;
	FMOD::Studio::ParameterInstance*	m_pCarEngineLoad;
	FMOD::Studio::ParameterInstance*	m_pMusicPickup;
	FMOD::Studio::ParameterInstance*	m_pMusicProgression;
	float	m_fEngineRPM;
	float	m_fEngineChangeDir;
	float	m_fCarEngineRPM;
	float	m_fCarEngineChangeDir;
	float	m_fMusicPickup;
	float	m_fMusicProgression;
	float	m_fMusicParamChangeRate;

	//	FMOD::DSP* pEchoDSP;
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