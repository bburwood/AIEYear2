#ifndef _PHYSICALLY_BASED_LIGHTING_H_
#define _PHYSICALLY_BASED_LIGHTING_H_

#include "Application.h"
#include "Camera.h"
#include "GLMHeader.h"
#include "tiny_obj_loader.h"
#include <vector>
#include "Vertex.h"
//class	FBXFile;

class PhysicallyBasedRendering : public Application
{
public:
	PhysicallyBasedRendering();
	~PhysicallyBasedRendering();
	bool	startup();
	void	shutdown();
	bool	update();
	void	draw();

	void	CreateOpenGlBuffers(std::vector<tinyobj::shape_t> & a_shapes);
	void	CleanUpOpenGLBuffers();

	void	ReloadShader();


	FlyCamera	m_FlyCamera;
	//FBXFile*	m_FBXfile;

	///
	///	basically not much done here - only up to page 2 in the tutorial
	///

	void	GenerateGrid(unsigned int a_uiRows, unsigned int a_uiCols);
	void	GenerateShader();
	unsigned int	m_uiProgramID;
	unsigned int	m_uiIndexCount;
	std::vector<OpenGLData>	m_GLData;


	unsigned int	m_uiVAO;
	unsigned int	m_uiVBO;
	unsigned int	m_uiIBO;
	vec3 light_dir;
	vec3 light_colour;
	vec3 material_colour;
	vec3 ambient_light;
	float	specular_power;
private:
	unsigned int	m_uiRows;
	unsigned int	m_uiCols;
	float	m_timer;
};

#endif	//	_PHYSICALLY_BASED_LIGHTING_H_
