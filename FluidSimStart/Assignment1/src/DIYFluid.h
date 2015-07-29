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
	void	BoxVelocityPush(int a_iCentreX, int a_iCentreY, int a_iBoxSize, glm::vec2 a_vVelocity, float a_fDt);
	glm::ivec2	DIYFluid::GetGridCoordsFromWorldCoords(glm::vec2 worldPos);
	void	BoxAddDye(int a_iCentreX, int a_iCentreY, int a_iBoxSize, glm::vec3 a_vColour, float a_fDt);

	void	SwapColours();
	void	SwapVelocities();
	void	SwapPressures();

	glm::vec2	vLastMouseWorldCoords;
	float	fViscosity;
	float	fCellDist;
	float	fQuadHalfSize;
	float	fDebugTimer;
	FluidCells	frontCells;
	FluidCells	backCells;
	float*	aFDivergence;
	int	iWidth, iHeight;
	unsigned int	uiProgram;
};


#endif	//	_DIYFLUID_H