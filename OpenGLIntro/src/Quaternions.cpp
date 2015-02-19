#include "Quaternions.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

Quaternions::Quaternions()
{
}
Quaternions::~Quaternions()
{
}

mat4	EvaluateKeyFrames(KeyFrame start, KeyFrame end, float t)
{
	vec3	pos = glm::mix(start.position, end.position, t);
	glm::quat	rot = glm::slerp(start.rotation, end.rotation, t);
	mat4	result = glm::translate(pos) * glm::toMat4(rot);

	return result;
}

bool	Quaternions::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create();

	glm::quat	myBoringQuaternion(1, 0, 0, 0);
	glm::quat	myEulerQuaternion(vec3(3, 5 ,7));	//	using Euler angles, in radians
	glm::quat	myMixedQuaternion = glm::slerp(myBoringQuaternion, myEulerQuaternion, 0.8f);

	//	now initialise the FlyCamera
	m_FlyCamera = FlyCamera(vec3(10, 10, 10), vec3(0, 0, 0), glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

	m_positions[0] = vec3(10, 5, 5);
	m_rotations[0] = glm::quat(vec3(0, -1, 0));

	m_positions[1] = vec3(-10, 0, -10);
	m_rotations[1] = glm::quat(vec3(0, 1, 0));

	m_HipFrames[0].position = vec3(0, 5, 0);
	m_HipFrames[0].rotation = glm::quat(vec3(-1, 0, 0));
	m_HipFrames[1].position = vec3(0, 5, 0);
	m_HipFrames[1].rotation = glm::quat(vec3(1, 0, 0));

	m_KneeFrames[0].position = vec3(0, -2.5, 0);
	m_KneeFrames[0].rotation = glm::quat(vec3(-1, 0, 0));
	m_KneeFrames[1].position = vec3(0, -2.5, 0);
	m_KneeFrames[1].rotation = glm::quat(vec3(0, 0, 0));

	m_AnkleFrames[0].position = vec3(0, -2.5, 0);
	m_AnkleFrames[0].rotation = glm::quat(vec3(0, 0, 0));
	m_AnkleFrames[1].position = vec3(0, -2.5, 0);
	m_AnkleFrames[1].rotation = glm::quat(vec3(0, 0, 0));

	return true;
}

void	Quaternions::shutdown()
{
	//	now clean up
	Gizmos::destroy();
}

bool	Quaternions::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float	dT = (float)glfwGetTime();
	glfwSetTime(0.0f);
	//	now we get to the fun stuff
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();
	Gizmos::addTransform(mat4(1));

	m_timer += dT;
	float	sine_wave = sinf(m_timer) * 0.5f + 0.5f;	//	goes from 0 - 1 ... use it as the t-value for slerp-ing
	float	sine_wave2 = sinf(m_timer * 2.0f) * 0.5f + 0.5f;	//	goes from 0 - 1 ... use it as the t-value for slerp-ing
	vec3	final_pos = glm::mix(m_positions[0], m_positions[1], sine_wave);
	glm::quat	final_rot = glm::slerp(m_rotations[0], m_rotations[1], sine_wave);
	glm::mat4	transform = glm::translate(final_pos) * glm::toMat4(final_rot);

	Gizmos::addTransform(transform);
	Gizmos::addAABBFilled(final_pos, vec3(1), vec4(0, 0, 1, 1), &transform);

	hip_bone = EvaluateKeyFrames(m_HipFrames[0], m_HipFrames[1], sine_wave2);
	knee_bone = EvaluateKeyFrames(m_KneeFrames[0], m_KneeFrames[1], sine_wave2);
	ankle_bone = EvaluateKeyFrames(m_AnkleFrames[0], m_AnkleFrames[1], sine_wave2);

	mat4	globalHip = hip_bone;
	mat4	globalKnee = hip_bone * knee_bone;
	mat4	globalAnkle = globalKnee * ankle_bone;

	vec3	hipPos = globalHip[3].xyz;
	vec3	kneePos = globalKnee[3].xyz;
	vec3	anklePos = globalAnkle[3].xyz;

	Gizmos::addAABBFilled(hipPos, vec3(0.5f), vec4(1, 1, 0, 1), &globalHip);
	Gizmos::addAABBFilled(kneePos, vec3(0.5f), vec4(1, 0, 0, 1), &globalKnee);
	Gizmos::addAABBFilled(anklePos, vec3(0.5f), vec4(0, 0, 1, 1), &globalAnkle);

	Gizmos::addLine(hipPos, kneePos, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));
	Gizmos::addLine(kneePos, anklePos, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));



	m_FlyCamera.update(dT);

	vec4	white(1);
	vec4	black(0, 0, 0, 1);
	vec4	blue(0, 0, 1, 1);
	vec4	yellow(1, 1, 0, 1);
	vec4	green(0, 1, 0, 1);
	vec4	red(1, 0, 0, 1);
	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? white : black);
	}




	return true;
}

void	Quaternions::draw()
{
	//	Gizmos::draw(m_FlyCamera.m_projectionTransform, m_FlyCamera.m_viewTransform);
	Gizmos::draw(m_FlyCamera.GetProjectionView());
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

