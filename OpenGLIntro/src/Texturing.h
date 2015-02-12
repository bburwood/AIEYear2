#ifndef _TEXTURING_H_
#define	_TEXTURING_H_

#include "Application.h"
#include "Camera.h"
#include "GLMHeader.h"

class Texturing : public Application
{
public:
	Texturing();
	~Texturing();
	bool	startup();
	void	shutdown();
	bool	update();
	void	draw();
	void	GenerateGrid(unsigned int a_uiRows, unsigned int a_uiCols);
	void	GenerateShader();
	void	LoadTexture(const char* a_szFileName);
	void	GenerateQuad(float a_fSize);
	unsigned int	m_uiProgramID;
	unsigned int	m_uiTextureID;
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


#endif // !_TEXTURING_H_
