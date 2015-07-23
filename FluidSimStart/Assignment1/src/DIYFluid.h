#ifndef _DIYFLUID_H
#define _DIYFLUID_H

#include "glm/glm.hpp"

struct FluidCells
{
	float*	fPressure;
	glm::vec2*	vVelocity;
	glm::vec3*	vDyeColour;
};

class DIYFluid
{
public:
	DIYFluid(int a_iWidth, int a_iHeight, float a_fViscosity, float a_fCellDist);
	~DIYFluid();
	void	UpdateFluid(float fDt);
	void	RenderFluid(glm::mat4 viewProj);

	void	Advect(float fDt);
	void	Diffuse(float fDt);
	void	Divergence(float fDt);
	void	UpdatePressure(float fDt);
	void	ApplyPressure(float fDt);
	void	UpdateBoundary();

	void	SwapColours();
	void	SwapVelocities();
	void	SwapPressures();

	float	fViscosity;
	float	fCellDist;
	FluidCells	frontCells;
	FluidCells	backCells;
	float*	aFDivergence;
	int	iWidth, iHeight;
	unsigned int	uiProgram;
};


#endif	//	_DIYFLUID_H