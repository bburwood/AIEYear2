#include "Camera.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

#include <cstdio>


/////////////////////////////////////////////////////////////////////////////////////
//	Camera functions in this section
/////////////////////////////////////////////////////////////////////////////////////
Camera::Camera(vec3 a_pos, vec3 a_lookAt, float a_fFov, float a_fAspect, float a_fNear, float a_fFar)
{
	SetPosition(a_pos);
	SetLookAt(a_pos, a_lookAt, vec3(0, 1, 0));
	SetPerspective(a_fFov, a_fAspect, a_fNear, a_fFar);
}

Camera::~Camera()
{
}

void	Camera::SetPerspective(float a_fFov, float a_fAspect, float a_fNear, float a_fFar)
{
	m_projectionTransform = glm::perspective(a_fFov, a_fAspect, a_fNear, a_fFar);
}

void	Camera::SetLookAt(vec3 a_pos, vec3 a_lookAt, vec3 a_up)
{
	m_viewTransform = glm::lookAt(a_pos, a_lookAt, a_up);
	m_worldTransform = glm::inverse(m_viewTransform);
	UpdateProjectionViewTransform();
}

void	Camera::SetPosition(vec3 a_pos)
{
	m_worldTransform = glm::translate(a_pos);
	m_viewTransform = glm::inverse(m_worldTransform);
	UpdateProjectionViewTransform();
}

mat4	Camera::GetProjection()
{
	return m_projectionTransform;
}

mat4	Camera::GetProjectionView()
{
	return m_projectionViewTransform;
}

void	Camera::UpdateProjectionViewTransform()
{
	m_projectionViewTransform = m_projectionTransform * m_viewTransform;
}

vec3	Camera::GetForwardDirection()
{
	vec3	vForward = (m_worldTransform[2].xyz) * (-1.0f);
	return vForward;
}

vec3	Camera::GetUpDirection()
{
	vec3	vForward = (m_worldTransform[1].xyz);
	return vForward;
}

vec3	Camera::GetRightDirection()
{
	vec3	vForward = (m_worldTransform[0].xyz);
	return vForward;
}

vec3	Camera::GetPosition()
{
	return m_worldTransform[3].xyz;
}



/////////////////////////////////////////////////////////////////////////////////////
//	FlyCamera functions in this section
/////////////////////////////////////////////////////////////////////////////////////
FlyCamera::FlyCamera(vec3 a_pos, vec3 a_lookAt, float a_fFov, float a_fAspect, float a_fNear, float a_fFar)
	: m_fSpeed(20.0f)
{
	SetPosition(a_pos);
	SetLookAt(a_pos, a_lookAt, vec3(0, 1, 0));
	SetPerspective(a_fFov, a_fAspect, a_fNear, a_fFar);
}

FlyCamera::~FlyCamera()
{
}

bool	FlyCamera::update(float dT)
{
	GLFWwindow*	pCurrentWindow = glfwGetCurrentContext();
	if (glfwGetKey(pCurrentWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		//	camera's position -= forward, z, axis direction * speed * dT
		m_worldTransform[3] -= m_worldTransform[2] * m_fSpeed * dT;
		//printf("Pressed W\n");
	}
	if (glfwGetKey(pCurrentWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_worldTransform[3] += m_worldTransform[2] * m_fSpeed * dT;
	}
	if (glfwGetKey(pCurrentWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		//	camera's position -= x axis direction * speed * dT
		m_worldTransform[3] -= m_worldTransform[0] * m_fSpeed * dT;
	}
	if (glfwGetKey(pCurrentWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_worldTransform[3] += m_worldTransform[0] * m_fSpeed * dT;
	}
	if (glfwGetKey(pCurrentWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		//	Down: camera's position -= x axis direction * speed * dT
		m_worldTransform[3] -= m_worldTransform[1] * m_fSpeed * dT;
	}
	if (glfwGetKey(pCurrentWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		//	Up
		m_worldTransform[3] += m_worldTransform[1] * m_fSpeed * dT;
	}

	//	now do the mouse stuff
	if (glfwGetMouseButton(pCurrentWindow, 1))
	{
		//	do the mouse stuff if the right button has been pressed
		double	dXDelta, dYDelta;
		glfwGetCursorPos(pCurrentWindow, &dXDelta, &dYDelta);
		glfwSetCursorPos(pCurrentWindow, 1280.0 * 0.5, 720.0 * 0.5);

		//calculate x and y deltas here ...
		dXDelta -= 1280.0f / 2.0f;
		dYDelta -= 720.0f / 2.0f;

		dXDelta /= -1280.0f / 2.0f;
		dYDelta /= -720.0f / 2.0f;
		vec3	cameraRight = (vec3)m_worldTransform[0];
		mat4	cameraYaw = glm::rotate((float)dXDelta, vec3(0, 1, 0));
		mat4	cameraPitch = glm::rotate((float)dYDelta, cameraRight);
		m_LastRotation = cameraYaw * cameraPitch;	//	store this along with a bool that flags when a rotation has happened this frame.  This can then be used to rotate the F16.
		m_worldTransform[0] = m_LastRotation * m_worldTransform[0];
		m_worldTransform[1] = m_LastRotation * m_worldTransform[1];
		m_worldTransform[2] = m_LastRotation * m_worldTransform[2];
		m_bRotatedThisFrame = true;
	}
	else
	{
		m_bRotatedThisFrame = false;
	}
	m_worldTransform[3][3] = 1;
	m_viewTransform = glm::inverse(m_worldTransform);

	UpdateProjectionViewTransform();
	return true;
}

void	FlyCamera::SetSpeed(float a_fSpeed)
{
	m_fSpeed = a_fSpeed;
}
