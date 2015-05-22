#include "SoundProgramming.h"

#include "fmod_studio.hpp"
#include "fmod_errors.h"

#include <iostream>

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#ifdef _DEBUG
//	doing assert this way causes an instant crash at the Assert line, with no corrupted call stack
#define	Assert(value)	if (value) {} else {*((char*)0) = 0;}
#else
#define	Assert(value) 
#endif

bool SoundProgramming::startup()
{
    if (Application::startup() == false) 
    {
        return false;
    }
	glfwSwapInterval(0);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    Gizmos::create();

	m_camera = FlyCamera(1280.0f / 720.0f, 40.0f);
    m_camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
    m_camera.sensitivity = 3;

	//	set up the FMOD system
	bool bResult = OnCreate();
	Assert(bResult);

	//	now load the sound banks
	FMOD_RESULT result;
	result = m_pFmodSystem->loadBankFile("./audio/Banks/Vehicles.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &m_pVehicleBank);
	Assert(result == FMOD_OK);

	result = m_pFmodSystem->loadBankFile("./audio/Banks/Master Bank.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &m_pMasterBank);
	Assert(result == FMOD_OK);
	result = m_pFmodSystem->loadBankFile("./audio/Banks/Master Bank.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &m_pStringBank);
	Assert(result == FMOD_OK);
	result = m_pFmodSystem->loadBankFile("./audio/Banks/Surround_Ambience.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &m_pSurroundAmbienceBank);
	Assert(result == FMOD_OK);
	result = m_pFmodSystem->loadBankFile("./audio/Banks/Music.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &m_pMusicBank);
	Assert(result == FMOD_OK);
	result = m_pFmodSystem->loadBankFile("./audio/Banks/Weapons.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &m_pWeaponsBank);
	Assert(result == FMOD_OK);
	result = m_pFmodSystem->lookupID("event:/Vehicles/Basic Engine", &m_EngineID);
	Assert(result == FMOD_OK);
	result = m_pFmodSystem->getEvent(&m_EngineID, FMOD_STUDIO_LOAD_BEGIN_NOW, &m_pEngineDescription);
	Assert(result == FMOD_OK);
	result = m_pEngineDescription->createInstance(&m_pEngineInstance);
	Assert(result == FMOD_OK);
	result = m_pEngineInstance->getParameter("RPM", &m_pEngineRPM);
	Assert(result == FMOD_OK);

	result = m_pFmodSystem->lookupID("event:/Vehicles/Car Engine", &m_CarEngineID);
	Assert(result == FMOD_OK);
	result = m_pFmodSystem->getEvent(&m_CarEngineID, FMOD_STUDIO_LOAD_BEGIN_NOW, &m_pCarEngineDescription);
	Assert(result == FMOD_OK);
	result = m_pCarEngineDescription->createInstance(&m_pCarEngineInstance);
	Assert(result == FMOD_OK);
	result = m_pCarEngineInstance->getParameter("RPM", &m_pCarEngineRPM);
	Assert(result == FMOD_OK);
	result = m_pCarEngineInstance->getParameter("Load", &m_pCarEngineLoad);
	Assert(result == FMOD_OK);

	//m_pEngineInstance->start();
	m_fEngineRPM = 100.0f;
	m_pEngineRPM->setValue(m_fEngineRPM);
	m_fEngineChangeDir = 1.0f;

	//m_pCarEngineInstance->start();
	m_fCarEngineRPM = 100.0f;
	m_pCarEngineRPM->setValue(m_fCarEngineRPM);
	m_fCarEngineChangeDir = 1.0f;
	m_pCarEngineLoad->setValue(1.0f);
	/*
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}
	*/


	int	iEventsRead = 0;
	FMOD::Studio::EventDescription*	aEventDescriptions[32];
	result = m_pVehicleBank->getEventList(aEventDescriptions, 32, &iEventsRead);
	Assert(result == FMOD_OK);
	printf("Events read: %d\n", iEventsRead);
	for (int i = 0; i < iEventsRead; ++i)
	{
		//	first get the path
		char	szPath[256] = {};
		int		iCharsRead = 0;
		aEventDescriptions[i]->getPath(szPath, 255, &iCharsRead);
		printf("%s\n", szPath);
		//	next get the parameter count
		int		iParameterCount = 0;
		aEventDescriptions[i]->getParameterCount(&iParameterCount);
		printf("\t%d parameters:\n", iParameterCount);
		//	now get all the parameters
		for (int j = 0; j < iParameterCount; ++j)
		{
			FMOD_STUDIO_PARAMETER_DESCRIPTION	ParameterDescription;
			aEventDescriptions[i]->getParameterByIndex(j, &ParameterDescription);
			printf("\t\tName: %s  Min: %1.1f  Max: %1.1f  Type: %d\n", ParameterDescription.name, ParameterDescription.minimum, ParameterDescription.maximum, ParameterDescription.type);
		}
	}
	/*
	Returned:
	Events read: 2
	event:/Vehicles/Car Engine
		2 parameters:
			Name: RPM  Min: 0.0  Max: 10000.0  Type: 0
			Name: Load  Min: -1.0  Max: 1.0  Type: 0
	event:/Vehicles/Basic Engine
		2 parameters:
			Name: Load  Min: -1.0  Max: 1.0  Type: 0
			Name: RPM  Min: 0.0  Max: 2000.0  Type: 0
	*/
	iEventsRead = 0;
	FMOD::Studio::EventDescription*	aSurroundEventDescriptions[32];
	result = m_pSurroundAmbienceBank->getEventList(aSurroundEventDescriptions, 32, &iEventsRead);
	Assert(result == FMOD_OK);
	printf("Surround_Ambience bank events read: %d\n", iEventsRead);
	for (int i = 0; i < iEventsRead; ++i)
	{
		//	first get the path
		char	szPath[256] = {};
		int		iCharsRead = 0;
		aSurroundEventDescriptions[i]->getPath(szPath, 255, &iCharsRead);
		printf("%s\n", szPath);
		//	next get the parameter count
		int		iParameterCount = 0;
		aSurroundEventDescriptions[i]->getParameterCount(&iParameterCount);
		printf("\t%d parameters:\n", iParameterCount);
		//	now get all the parameters
		for (int j = 0; j < iParameterCount; ++j)
		{
			FMOD_STUDIO_PARAMETER_DESCRIPTION	ParameterDescription;
			aSurroundEventDescriptions[i]->getParameterByIndex(j, &ParameterDescription);
			printf("\t\tName: %s  Min: %1.1f  Max: %1.1f  Type: %d\n", ParameterDescription.name, ParameterDescription.minimum, ParameterDescription.maximum, ParameterDescription.type);
		}
	}
	/*
	Returned:
	event:/Ambience/Country
		1 parameters:
			Name: Time  Min: 0.0  Max: 1.0  Type: 0
	*/
	result = m_pFmodSystem->lookupID("event:/Ambience/Country", &m_SurroundAmbienceID);
	Assert(result == FMOD_OK);
	result = m_pFmodSystem->getEvent(&m_SurroundAmbienceID, FMOD_STUDIO_LOAD_BEGIN_NOW, &m_pSurroundAmbienceDescription);
	Assert(result == FMOD_OK);
	result = m_pSurroundAmbienceDescription->createInstance(&m_pSurroundAmbienceInstance);
	Assert(result == FMOD_OK);
	//m_pSurroundAmbienceInstance->start();

	iEventsRead = 0;
	FMOD::Studio::EventDescription*	aMusicDescriptions[32];
	result = m_pMusicBank->getEventList(aMusicDescriptions, 32, &iEventsRead);
	Assert(result == FMOD_OK);
	printf("Music bank events read: %d\n", iEventsRead);
	for (int i = 0; i < iEventsRead; ++i)
	{
		//	first get the path
		char	szPath[256] = {};
		int		iCharsRead = 0;
		aMusicDescriptions[i]->getPath(szPath, 255, &iCharsRead);
		printf("%s\n", szPath);
		//	next get the parameter count
		int		iParameterCount = 0;
		aMusicDescriptions[i]->getParameterCount(&iParameterCount);
		printf("\t%d parameters:\n", iParameterCount);
		//	now get all the parameters
		for (int j = 0; j < iParameterCount; ++j)
		{
			FMOD_STUDIO_PARAMETER_DESCRIPTION	ParameterDescription;
			aMusicDescriptions[i]->getParameterByIndex(j, &ParameterDescription);
			printf("\t\tName: %s  Min: %1.1f  Max: %1.1f  Type: %d\n", ParameterDescription.name, ParameterDescription.minimum, ParameterDescription.maximum, ParameterDescription.type);
		}
	}
	/*
	Music bank events read: 1
	event:/Music/Music
        2 parameters:
                Name: Pickup  Min: 0.0  Max: 1.0  Type: 0
                Name: Progression  Min: 0.0  Max: 1.0  Type: 0
	*/
	result = m_pFmodSystem->lookupID("event:/Music/Music", &m_MusicID);
	Assert(result == FMOD_OK);
	result = m_pFmodSystem->getEvent(&m_MusicID, FMOD_STUDIO_LOAD_BEGIN_NOW, &m_pMusicDescription);
	Assert(result == FMOD_OK);
	result = m_pMusicDescription->createInstance(&m_pMusicInstance);
	Assert(result == FMOD_OK);
	result = m_pMusicInstance->getParameter("Pickup", &m_pMusicPickup);
	Assert(result == FMOD_OK);
	result = m_pMusicInstance->getParameter("Progression", &m_pMusicProgression);
	Assert(result == FMOD_OK);
	m_pMusicInstance->start();
	m_fMusicPickup = 0.0f;
	m_fMusicProgression = 0.0f;
	m_fMusicParamChangeRate = 0.2f;	//	per second parameter change

	iEventsRead = 0;
	FMOD::Studio::EventDescription*	aWeaponsDescriptions[32];
	result = m_pWeaponsBank->getEventList(aWeaponsDescriptions, 32, &iEventsRead);
	Assert(result == FMOD_OK);
	printf("Weapons bank events read: %d\n", iEventsRead);
	for (int i = 0; i < iEventsRead; ++i)
	{
		//	first get the path
		char	szPath[256] = {};
		int		iCharsRead = 0;
		aWeaponsDescriptions[i]->getPath(szPath, 255, &iCharsRead);
		printf("%s\n", szPath);
		//	next get the parameter count
		int		iParameterCount = 0;
		aWeaponsDescriptions[i]->getParameterCount(&iParameterCount);
		printf("\t%d parameters:\n", iParameterCount);
		//	now get all the parameters
		for (int j = 0; j < iParameterCount; ++j)
		{
			FMOD_STUDIO_PARAMETER_DESCRIPTION	ParameterDescription;
			aWeaponsDescriptions[i]->getParameterByIndex(j, &ParameterDescription);
			printf("\t\tName: %s  Min: %1.1f  Max: %1.1f  Type: %d\n", ParameterDescription.name, ParameterDescription.minimum, ParameterDescription.maximum, ParameterDescription.type);
		}
	}
	/*
	Weapons bank events read: 4
	event:/Explosions/Single Explosion
		1 parameters:
			Name: Size  Min: 0.0  Max: 3.0  Type: 0
	event:/Weapons/Single-Shot Random
		0 parameters:
	event:/Weapons/Single-Shot
		0 parameters:
	event:/Weapons/Full Auto Loop
		0 parameters:
	*/
	result = m_pFmodSystem->lookupID("event:/Weapons/Single-Shot Random", &m_WeaponsID);
	Assert(result == FMOD_OK);
	result = m_pFmodSystem->getEvent(&m_WeaponsID, FMOD_STUDIO_LOAD_BEGIN_NOW, &m_pWeaponsDescription);
	Assert(result == FMOD_OK);
	result = m_pWeaponsDescription->createInstance(&m_pWeaponsInstance);
	Assert(result == FMOD_OK);
	//m_pWeaponsInstance->start();


	position = { 0, 0, 0 };
	velocity = { 0, 0, 0 };
	forward = { 1, 0, 0 };
	up = { 0, 1, 0 };

	GenSoundPosition = { 0, 0, 0 };
	GenSoundVelocity = { 0, 0, 0 };

	return true;
}

void SoundProgramming::shutdown()
{
    Gizmos::destroy();

	//	now release FMOD's resources and shut it down
	m_pEngineInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	m_pEngineInstance->release();
	m_pCarEngineInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	m_pCarEngineInstance->release();
	m_pSurroundAmbienceInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	m_pSurroundAmbienceInstance->release();
	m_pMusicInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	m_pMusicInstance->release();
	m_pWeaponsInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	m_pWeaponsInstance->release();

	m_pFmodSystem->release();

    Application::shutdown();
}

bool SoundProgramming::update()
{
    if (Application::update() == false)
    {
        return false;
    }

    Gizmos::clear();

    float dt = (float)glfwGetTime();
    glfwSetTime(0.0);
	m_camera.update(dt);

    vec4 white(1);
    vec4 black(0, 0, 0, 1);
    for (int i = 0; i <= 20; ++i)
    {
        Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
            i == 10 ? white : black);
        Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
            i == 10 ? white : black);
    }

	if ((m_fEngineRPM > 2000.0f) || (m_fEngineRPM < 100.0f))
	{
		m_fEngineChangeDir *= -1.0f;
	}
	m_fEngineRPM += 50.0f * dt * m_fEngineChangeDir;
	FMOD_RESULT	result = m_pEngineRPM->setValue(m_fEngineRPM);
	Assert(result == FMOD_OK);

	if ((m_fCarEngineRPM > 10000.0f) || (m_fCarEngineRPM < 100.0f))
	{
		m_fCarEngineChangeDir *= -1.0f;
		m_pCarEngineLoad->setValue(m_fCarEngineChangeDir);
	}
	m_fCarEngineRPM += 500.0f * dt * m_fCarEngineChangeDir;
	result = m_pCarEngineRPM->setValue(m_fCarEngineRPM);
	Assert(result == FMOD_OK);

	if (glfwGetKey(m_window, GLFW_KEY_I) == GLFW_PRESS)
	{
		//	increase Pickup
		m_fMusicPickup += m_fMusicParamChangeRate * dt;
		if (m_fMusicPickup > 1.0f)
		{
			m_fMusicPickup = 1.0f;
		}
		printf("Pickup Level: %1.3f  Progression Level: %1.3f\n", m_fMusicPickup, m_fMusicProgression);
		result = m_pMusicPickup->setValue(m_fMusicPickup);
		Assert(result == FMOD_OK);
	}
	if (glfwGetKey(m_window, GLFW_KEY_K) == GLFW_PRESS)
	{
		//	decrease Pickup
		m_fMusicPickup -= m_fMusicParamChangeRate * dt;
		if (m_fMusicPickup < 0.0f)
		{
			m_fMusicPickup = 0.0f;
		}
		printf("Pickup Level: %1.3f  Progression Level: %1.3f\n", m_fMusicPickup, m_fMusicProgression);
		result = m_pMusicPickup->setValue(m_fMusicPickup);
		Assert(result == FMOD_OK);
	}
	if (glfwGetKey(m_window, GLFW_KEY_P) == GLFW_PRESS)
	{
		//	increase progression
		m_fMusicProgression += m_fMusicParamChangeRate * dt;
		if (m_fMusicProgression > 1.0f)
		{
			m_fMusicProgression = 1.0f;
		}
		printf("Pickup Level: %1.3f  Progression Level: %1.3f\n", m_fMusicPickup, m_fMusicProgression);
		result = m_pMusicProgression->setValue(m_fMusicProgression);
		Assert(result == FMOD_OK);
	}
	if (glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS)
	{
		//	decrease progression
		m_fMusicProgression -= m_fMusicParamChangeRate * dt;
		if (m_fMusicProgression < 0.0f)
		{
			m_fMusicProgression = 0.0f;
		}
		printf("Pickup Level: %1.3f  Progression Level: %1.3f\n", m_fMusicPickup, m_fMusicProgression);
		result = m_pMusicProgression->setValue(m_fMusicProgression);
		Assert(result == FMOD_OK);
	}

	vec4	camPos = m_camera.world[3];
	vec4	camForward = m_camera.world[2] * (-1.0f);
	vec4	camUp = m_camera.world[1] * (-1.0f);

	vec4	camVel = (camPos - m_vLastCamPos) / dt;

	position = { camPos.x, camPos.y, camPos.z };
	velocity = { camVel.x, camVel.y, camVel.z };
	forward = { camForward.x, camForward.y, camForward.z };
	up = { camUp.x, camUp.y, camUp.z };

//	m_pFmodSystem->set3DListenerAttributes(0, &position, &velocity, &forward, &up);


	m_pFmodSystem->update();
	m_vLastCamPos = camPos;
    return true;
}

void SoundProgramming::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Gizmos::draw(m_camera.proj, m_camera.view);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

bool SoundProgramming::OnCreate()
{
	// Initialize FMOD
	FMOD_RESULT result;
	m_pFmodSystem = NULL;
	// Create the main system object.
	result = FMOD::Studio::System::create(&m_pFmodSystem);
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}
	// Initialize FMOD with 512 channels
	result = m_pFmodSystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}
	return true;
}
