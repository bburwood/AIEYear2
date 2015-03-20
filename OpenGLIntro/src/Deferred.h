#ifndef _DEFERRED_H_
#define _DEFERRED_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"

#include "AntTweakBar.h"

class Deferred : public Application
{
public:
	Deferred();
	virtual	~Deferred();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

	void	ReloadShader();

	void	BuildMeshes();
	void	BuildGBuffer();
	void	BuildLightBuffer();
	void	BuildQuad();
	void	RenderDirectionalLight(vec3 light_dir, vec3 light_colour);

	//	load up a mesh
	OpenGLData	m_Bunny;
	OpenGLData	m_ScreenSpaceQuad;

	//	generate a g-buffer
	unsigned int	m_uiGBufferFBO;
	unsigned int	m_uiAlbedoTexture;
	unsigned int	m_uiNormalsTexture;
	unsigned int	m_uiPositionsTexture;
	unsigned int	m_uiGBufferDepth;

	//	render lights
	unsigned int	m_uiLightFBO;
	unsigned int	m_uiLightTexture;

	//	we need our shaders
	unsigned int	m_uiGBufferProgram;
	unsigned int	m_uiDirectionalLightProgram;
	unsigned int	m_uiPointLightProgram;
	unsigned int	m_uiSpotLightProgram;
	unsigned int	m_uiCompositeProgram;

	mat4	m_view;
	mat4	m_projection;
	float	m_camera_x;
	float	m_camera_z;
	float	m_timer;
	unsigned int	m_uiProgramID;
	FlyCamera	m_FlyCamera;
	vec4	m_BackgroundColour;
	float	m_fFPS;

	TwBar*	m_bar;

	bool	m_bDrawGizmos;
};

#endif	//	_DEFERRED_H_
