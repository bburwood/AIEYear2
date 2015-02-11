#ifndef _RENDERING_GEOMETRY_H_
#define _RENDERING_GEOMETRY_H_

#include "Application.h"
#include "Camera.h"
#include "GLMHeader.h"

class RenderingGeometry : public Application
{
public:
	RenderingGeometry();
	~RenderingGeometry();
	bool	startup();
	void	shutdown();
	bool	update();
	void	draw();
	void	GenerateGrid(unsigned int a_uiRows, unsigned int a_uiCols);
	void	GenerateShader();
	unsigned int	m_uiProgramID;
	unsigned int	m_uiIndexCount;

	FlyCamera	m_FlyCamera;

	unsigned int	m_uiVAO;
	unsigned int	m_uiVBO;
	unsigned int	m_uiIBO;
private:
	unsigned int	m_uiRows;
	unsigned int	m_uiCols;
	float	m_timer;
};

#endif	//	_RENDERING_GEOMETRY_H_
