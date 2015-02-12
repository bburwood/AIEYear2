#ifndef _LIGHTING_H_
#define _LIGHTING_H_

#include "Application.h"
#include "Camera.h"
#include "GLMHeader.h"

class	FBXFile;

class Lighting : public Application
{
public:
	Lighting();
	~Lighting();
	bool	startup();
	void	shutdown();
	bool	update();
	void	draw();

	void	CreateOpenGlBuffers(FBXFile* a_file);
	void	CleanUpOpenGLBuffers(FBXFile* a_file);

	void	ReloadShader();


	FlyCamera	m_FlyCamera;
	FBXFile*	m_FBXfile;



	void	GenerateGrid(unsigned int a_uiRows, unsigned int a_uiCols);
	void	GenerateShader();
	unsigned int	m_uiProgramID;
	unsigned int	m_uiIndexCount;


	unsigned int	m_uiVAO;
	unsigned int	m_uiVBO;
	unsigned int	m_uiIBO;
private:
	unsigned int	m_uiRows;
	unsigned int	m_uiCols;
	float	m_timer;
};

#endif	//	_LIGHTING_H_
