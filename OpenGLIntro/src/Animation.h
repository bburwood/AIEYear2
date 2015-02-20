#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "Application.h"
#include "Camera.h"
#include "FBXFile.h"
#include "Vertex.h"

class Animation : public Application
{
public:
	Animation();
	virtual	~Animation();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();
	void	GenerateGLMeshes(FBXFile* fbx);
	void	EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton* skeleton, float timer);
	void	UpdateBones(FBXSkeleton* skeleton);

	mat4	m_view;
	mat4	m_projection;
	float	m_camera_x;
	float	m_camera_z;
	float	m_timer;
	FlyCamera	m_FlyCamera;
	vec4	m_BackgroundColour;

	FBXFile*	m_file;
	std::vector<OpenGLData>	m_meshes;
	unsigned int	m_uiProgramID;

	bool	m_bDrawGizmos;
};

#endif	//	_ANIMATION_H_
