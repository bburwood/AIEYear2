#include "SoundProgramming.h"

#include "fmod.hpp"
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

	//	now load a few sounds
	FMOD_RESULT result;
	//result = m_pFmodSystem->createSound("./audio/jaguar.wav", FMOD_DEFAULT, 0, &m_pJaguarSound);
	//	set linear roll off for the sound
	result = m_pFmodSystem->createSound("./audio/jaguar.wav", FMOD_DEFAULT | FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &m_pJaguarSound);
	result = m_pFmodSystem->createSound("./audio/background_music.ogg", FMOD_DEFAULT | FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &m_pMusicSound);
	//	The next two will both create a stream  (and are functionally the same):
	//result = m_pFmodSystem->createSound("jaguar.wav", FMOD_CREATESTREAM,, 0, &m_pJaguarSound);
	//result = m_pFmodSystem->createStream("jaguar.wav", FMOD_DEFAULT, 0, &m_pJaguarSound);
	Assert(result == FMOD_OK); 

	result = m_pFmodSystem->createChannelGroup("GeneralSoundsGroup", &m_pGeneralSoundsGroup);
	Assert(result == FMOD_OK);

	result = m_pFmodSystem->createDSPByType(FMOD_DSP_TYPE_ECHO, &pEchoDSP);
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}

	//result = m_pFmodSystem->playSound(m_pJaguarSound, 0, false, &m_pMainChannel);
	result = m_pFmodSystem->playSound(m_pJaguarSound, m_pGeneralSoundsGroup, false, &m_pMainChannel);
	Assert(result == FMOD_OK);
	result = m_pFmodSystem->playSound(m_pMusicSound, 0, true, &m_pMusicChannel);
	Assert(result == FMOD_OK);

	//result = m_pMusicChannel->addDSP(0, pEchoDSP);
	Assert(result == FMOD_OK);

	m_pMusicChannel->setPaused(false);

	m_pFmodSystem->set3DListenerAttributes(0, &position, &velocity, &forward, &up);

	m_pGeneralSoundsGroup->set3DAttributes(&GenSoundPosition, &GenSoundVelocity);
	m_pGeneralSoundsGroup->set3DMinMaxDistance(5.0f, 20.0f);

	m_pMainChannel->set3DAttributes(&GenSoundPosition, &GenSoundVelocity);
	m_pMusicChannel->set3DAttributes(&GenSoundPosition, &GenSoundVelocity);
	m_pMainChannel->set3DMinMaxDistance(5.0f, 200.0f);
	m_pMusicChannel->set3DMinMaxDistance(5.0f, 200.0f);

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
	//	First release the sounds
	m_pJaguarSound->release();

	//	Then close and release the FMOD system object
	m_pFmodSystem->close();
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

	vec4	camPos = m_camera.world[3];
	vec4	camForward = m_camera.world[2] * (-1.0f);
	vec4	camUp = m_camera.world[1] * (-1.0f);

	vec4	camVel = (camPos - m_vLastCamPos) / dt;

	position = { camPos.x, camPos.y, camPos.z };
	velocity = { camVel.x, camVel.y, camVel.z };
	forward = { camForward.x, camForward.y, camForward.z };
	up = { camUp.x, camUp.y, camUp.z };

	m_pFmodSystem->set3DListenerAttributes(0, &position, &velocity, &forward, &up);


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
	result = FMOD::System_Create(&m_pFmodSystem);
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}
	// Initialize FMOD with 512 channels
	result = m_pFmodSystem->init(512, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}
	return true;
}
