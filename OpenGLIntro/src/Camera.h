#ifndef _CAMERA_H_
#define _CAMERA_H_

#define	GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class Camera
{
public:
	Camera(vec3 a_pos, vec3 a_lookAt, float a_fFov, float a_fAspect, float a_fNear, float a_fFar);
	Camera(){};
	~Camera();
	virtual	bool	update(float dT) = 0;
	void	SetPerspective(float a_fFov, float a_fAspect, float a_fNear, float a_fFar);
	void	SetLookAt(vec3 a_pos, vec3 a_lookAt, vec3 a_up);
	void	SetPosition(vec3 a_pos);
	mat4	GetProjection();
	mat4	GetProjectionView();
	void	UpdateProjectionViewTransform();
	vec3	PickAgainstPlane(float x, float y, vec4 plane);

	
	mat4	m_worldTransform;
	mat4	m_viewTransform;
	mat4	m_projectionTransform;
	mat4	m_projectionViewTransform;
private:
};

class FlyCamera : public Camera
{
public:
	FlyCamera(vec3 a_pos, vec3 a_lookAt, float a_fFov, float a_fAspect, float a_fNear, float a_fFar);
	FlyCamera(){};
	~FlyCamera();
	bool	update(float dT);
	void	SetSpeed(float a_fSpeed);

private:
	float	m_fSpeed;
};
#endif
