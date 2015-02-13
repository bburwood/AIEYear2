#ifndef _ADVANCED_TEXTURING_H_
#define _ADVANCED_TEXTURING_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"

class AdvancedTexturing : public Application
{
public:
	AdvancedTexturing();
	virtual	~AdvancedTexturing();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();
	void	LoadTextures();
	void	GenerateQuad(float a_fSize);
	void	ReloadShader();

	mat4	m_view;
	mat4	m_projection;
	float	m_camera_x;
	float	m_camera_z;
	float	m_timer;
	FlyCamera	m_FlyCamera;
	OpenGLData	m_quad;
	unsigned int	m_uiDiffuseTexture;
	unsigned int	m_uiNormalTexture;
	unsigned int	m_uiSpecularTexture;
	unsigned int	m_uiProgramID;
	vec3 m_ambient_light;
	vec3 m_light_dir;
	vec3 m_light_colour;
	vec3 m_material_colour;
	float	m_fSpecular_power;
};

#endif	//	_ADVANCED_TEXTURING_H_
