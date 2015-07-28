//	Extra exercises:
//	Add dye with mouse
//	Add velocity with mouse

//	Add a collider

#include "DIYFluid.h"
#include "Utilities.h"
#include "glm/glm.hpp"
#include "gl_core_4_4.h"
#include <glfw3.h>
#include <iostream>

DIYFluid::DIYFluid(int a_iWidth, int a_iHeight, float a_fViscosity, float a_fCellDist)
{
	iWidth = a_iWidth;
	iHeight = a_iHeight;
	fViscosity = a_fViscosity;
	fCellDist = a_fCellDist;
	fQuadHalfSize = 5.0f;
	fDebugTimer = 0.0f;

	int	iCellCount = iWidth * iHeight;

	frontCells.vVelocity = new glm::vec2[iCellCount];
	frontCells.vDyeColour = new glm::vec3[iCellCount];
	frontCells.fPressure = new float[iCellCount];

	backCells.vVelocity = new glm::vec2[iCellCount];
	backCells.vDyeColour = new glm::vec3[iCellCount];
	backCells.fPressure = new float[iCellCount];

	aFDivergence = new float[iCellCount];

	//	initialise memory to zeroes
	memset(frontCells.vVelocity, 0, sizeof(glm::vec2) * iCellCount);
	memset(frontCells.vDyeColour, 0, sizeof(glm::vec2) * iCellCount);
	memset(frontCells.fPressure, 0, sizeof(float) * iCellCount);

	memset(backCells.vVelocity, 0, sizeof(glm::vec2) * iCellCount);
	memset(backCells.vDyeColour, 0, sizeof(glm::vec2) * iCellCount);
	memset(backCells.fPressure, 0, sizeof(float)* iCellCount);

	memset(aFDivergence, 0, sizeof(float)* iCellCount);

	for (int i = 0; i < iWidth * iHeight; ++i)
	{
		frontCells.vDyeColour[i] = glm::vec3(
			(float)i / (2.0f * (float)iWidth),
			(float)i / (1.0f * (float)iWidth),
			(float)i / (0.5f * (float)iWidth));
	}

	LoadShader("./shaders/simple_vertex.vs", 0, "./shaders/simple_texture.fs", &uiProgram);
}

DIYFluid::~DIYFluid()
{
	delete[]	frontCells.fPressure;
	delete[]	frontCells.vDyeColour;
	delete[]	frontCells.vVelocity;

	delete[]	backCells.fPressure;
	delete[]	backCells.vDyeColour;
	delete[]	backCells.vVelocity;

	delete[]	aFDivergence;
}

void	DIYFluid::SwapColours()
{
	glm::vec3*	temp = frontCells.vDyeColour;
	frontCells.vDyeColour = backCells.vDyeColour;
	backCells.vDyeColour = temp;
}

void	DIYFluid::SwapVelocities()
{
	glm::vec2*	temp = frontCells.vVelocity;
	frontCells.vVelocity = backCells.vVelocity;
	backCells.vVelocity = temp;
}

void	DIYFluid::SwapPressures()
{
	float*	temp = frontCells.fPressure;
	frontCells.fPressure = backCells.fPressure;
	backCells.fPressure = temp;
}
/*
glm::vec3	PickAgainstPlane(float x, float y, glm::vec4 plane)
{
	float nxPos = x / (float)BUFFER_WIDTH;
	float nyPos = y / (float)BUFFER_HEIGHT;

	float sxPos = nxPos - 0.5f;
	float syPos = nyPos - 0.5f;

	float fxPos = sxPos * 2.0f;
	float fyPos = syPos * -2.0f;

	glm::mat4 invViewProj = glm::inverse(m_projectionViewTransform);
	glm::vec4 mousePos(fxPos, fyPos, 1.0f, 1.0f);
	glm::vec4 worldPos = invViewProj * mousePos;

	worldPos /= worldPos.w;

	glm::vec3 camPos = m_worldTransform[3].xyz;
	glm::vec3 dir = worldPos.xyz() - camPos;

	float t = -(glm::dot(camPos, plane.xyz()) + plane.w) / (glm::dot(dir, plane.xyz()));

	vec3 result = camPos + dir * t;

	return result;
}
*/

glm::vec2	GetMouseWorldCoords(float x, float y, int windowWidth, int windowHeight, float realWidth, float AR)
{
	glm::vec2	result(0.0f);
	float nxPos = 2.0f * ((x / (float)windowWidth) - 0.5f);
	float nyPos = 2.0f * ((y / (float)windowHeight) - 0.5f);

	float halfWidth = realWidth * 0.5f;

	result.x = halfWidth * nxPos;
	result.y = -halfWidth * nyPos / AR;

	return result;
}

glm::ivec2	DIYFluid::GetGridCoordsFromWorldCoords(glm::vec2 worldPos)
{
	glm::ivec2	result(0);

	glm::vec2	texOrigin(-fQuadHalfSize, -fQuadHalfSize);

	glm::vec2	tempResult = ((worldPos - texOrigin) * glm::vec2((float)iWidth, (float)iHeight) / (2.0f * fQuadHalfSize));

	result.x = glm::clamp((int)tempResult.x, 0, iWidth);
	result.y = glm::clamp((int)tempResult.y, 0, iHeight);

	return result;
}

void	DIYFluid::UpdateFluid(float fDt)
{
	Advect(fDt);
	SwapVelocities();
	SwapColours();

	for (int iDiffuseStep = 0; iDiffuseStep < 20; ++iDiffuseStep)
	{
		Diffuse(fDt);
		SwapVelocities();
	}

	Divergence(fDt);

	for (int iPressureStep = 0; iPressureStep < 25; ++iPressureStep)
	{
		UpdatePressure(fDt);
		SwapPressures();
	}

	ApplyPressure(fDt);
	SwapVelocities();

	UpdateBoundary();

	//BoxVelocityPush(iWidth / 2 - 5, iHeight / 4, 4, glm::vec2(0.0f, 0.0001f), fDt);	//	add a vertical velocity push in the centre
	
	//BoxVelocityPush(iWidth / 4, 5 + iHeight / 3, 4, glm::vec2(10.0f, 20.0f), fDt);
	//BoxVelocityPush(iWidth * 3 / 4, 5 + iHeight * 5 / 6, 4, glm::vec2(10.0f, -15.0f), fDt);

	//	now check for mouse clicks to add other pushes ...
	int	width = 0, height = 0;
	GLFWwindow*	pWindow = glfwGetCurrentContext();
	glfwGetWindowSize(pWindow, &width, &height);
	float AR = (float)width / (float)height;
	//	fluid.RenderFluid(glm::ortho<float>(-10, 10, -10 / AR, 10 / AR, -1.0f, 1.0f));	//	original render call from main ...
//	fDebugTimer += fDt;
//	if (fDebugTimer > 1.0f)
//	{
//		std::cout << "Window size: " << width << " / " << height << "  AR: " << AR << '\n';
//		fDebugTimer = 0.0f;
//	}
	if (glfwGetMouseButton(pWindow, 0))
	{
		//	do the mouse stuff if the left button has been pressed
		double	dXDelta, dYDelta;
		glfwGetCursorPos(pWindow, &dXDelta, &dYDelta);
		glm::vec2 mouseWorldPos = GetMouseWorldCoords((float)dXDelta, (float)dYDelta, width, height, 20.0f, AR);
		//	now get the mouse coords in grid coords ...
		glm::ivec2	mouseGridPos = GetGridCoordsFromWorldCoords(mouseWorldPos);
		//std::cout << "Mouse Coord (world): " << mouseWorldPos.x << " / " << mouseWorldPos.y
		//	<< "  Mouse Coord (Grid): " << mouseGridPos.x << " / " << mouseGridPos.y << '\n';
		BoxVelocityPush(mouseGridPos.x, mouseGridPos.y, 4, glm::vec2(75.0f, 75.0f), fDt);
	}
	if (glfwGetMouseButton(pWindow, 1))
	{
		//	add dye if the right button is pressed
		double	dXDelta, dYDelta;
		glfwGetCursorPos(pWindow, &dXDelta, &dYDelta);
		glm::vec2 mouseWorldPos = GetMouseWorldCoords((float)dXDelta, (float)dYDelta, width, height, 20.0f, AR);
		//	now get the mouse coords in grid coords ...
		glm::ivec2	mouseGridPos = GetGridCoordsFromWorldCoords(mouseWorldPos);
		BoxAddDye(mouseGridPos.x, mouseGridPos.y, 10, glm::vec3(100.0f, 100.0f, 255.0f), fDt);
	}
	if (glfwGetMouseButton(pWindow, 2))
	{
		//	add dye if the centre button is pressed
		double	dXDelta, dYDelta;
		glfwGetCursorPos(pWindow, &dXDelta, &dYDelta);
		glm::vec2 mouseWorldPos = GetMouseWorldCoords((float)dXDelta, (float)dYDelta, width, height, 20.0f, AR);
		//	now get the mouse coords in grid coords ...
		glm::ivec2	mouseGridPos = GetGridCoordsFromWorldCoords(mouseWorldPos);
		BoxAddDye(mouseGridPos.x, mouseGridPos.y, 10, glm::vec3(-100.0f, -100.0f, -255.0f), fDt);
	}
}

void	DIYFluid::BoxAddDye(int a_iCentreX, int a_iCentreY, int a_iBoxSize, glm::vec3 a_vColour, float a_fDt)
{
	int	iHalfBoxSize = a_iBoxSize / 2;
	int iCellIndex = 0;
	int	iLowerBoundY = glm::clamp(a_iCentreY - iHalfBoxSize, 0, iHeight - 1);
	int	iUpperBoundY = glm::clamp(a_iCentreY + iHalfBoxSize, 0, iHeight - 1);
	int	iLowerBoundX = glm::clamp(a_iCentreX - iHalfBoxSize, 0, iWidth - 1);
	int	iUpperBoundX = glm::clamp(a_iCentreX + iHalfBoxSize, 0, iWidth - 1);
	for (int y = iLowerBoundY; y < iUpperBoundY; ++y)
	{
		iCellIndex = y * iWidth;
		for (int x = iLowerBoundX; x < iUpperBoundX; ++x)
		{
			frontCells.vDyeColour[iCellIndex + x] += a_vColour * a_fDt;
		}
	}
}

void	DIYFluid::BoxVelocityPush(int a_iCentreX, int a_iCentreY, int a_iBoxSize, glm::vec2 a_vVelocity, float a_fDt)
{
	int	iHalfBoxSize = a_iBoxSize / 2;
	int iCellIndex = 0;
	int	iLowerBoundY = glm::clamp(a_iCentreY - iHalfBoxSize, 0, iHeight - 1);
	int	iUpperBoundY = glm::clamp(a_iCentreY + iHalfBoxSize, 0, iHeight - 1);
	int	iLowerBoundX = glm::clamp(a_iCentreX - iHalfBoxSize, 0, iWidth - 1);
	int	iUpperBoundX = glm::clamp(a_iCentreX + iHalfBoxSize, 0, iWidth - 1);
	for (int y = iLowerBoundY; y < iUpperBoundY; ++y)
	{
		iCellIndex = y * iWidth;
		for (int x = iLowerBoundX; x < iUpperBoundX; ++x)
		{
			frontCells.vVelocity[iCellIndex + x] += a_vVelocity * a_fDt;
		}
	}
}

void	DIYFluid::Advect(float fDt)
{
	//	loop over every cell
	for (int y = 0; y < iHeight; ++y)
	{
		int iCellIndex = y * iWidth;
		for (int x = 0; x < iWidth; ++x)
		{
			//	find the point to sample for this cell
			glm::vec2	vVel = frontCells.vVelocity[iCellIndex] * fDt;
			glm::vec2	vSamplePoint = glm::vec2(
				(float)x - vVel.x / fCellDist,
				(float)y - vVel.y / fCellDist);	//	in cell grid space
			vSamplePoint.x = glm::clamp(vSamplePoint.x, 0.0f, (float)iWidth - 1.1f);
			vSamplePoint.y = glm::clamp(vSamplePoint.y, 0.0f, (float)iWidth - 1.1f);

			//	read each value from front cells and store in back cells
			//	first compute bilerp for the point
			glm::vec2	bl = glm::vec2(floorf(vSamplePoint.x), floorf(vSamplePoint.y));
			glm::vec2	br = bl + glm::vec2(1, 0);
			glm::vec2	tl = bl + glm::vec2(0, 1);
			glm::vec2	tr = bl + glm::vec2(1, 1);
			int	bli = (int)bl.x + iWidth * (int)bl.y;	//	bli = bottom left index ... etc
			int	bri = (int)br.x + iWidth * (int)br.y;
			int	tli = (int)tl.x + iWidth * (int)tl.y;
			int	tri = (int)tr.x + iWidth * (int)tr.y;

			glm::vec2	sampleFract = vSamplePoint - bl;
			//	actually advecting ...
			glm::vec3	dye_b = glm::mix(frontCells.vDyeColour[bli], frontCells.vDyeColour[bri], sampleFract.x);
			glm::vec3	dye_t = glm::mix(frontCells.vDyeColour[tli], frontCells.vDyeColour[tri], sampleFract.x);
			glm::vec3	new_dye = glm::mix(dye_b, dye_t, sampleFract.y);
			backCells.vDyeColour[iCellIndex] = new_dye;

			glm::vec2	vel_b = glm::mix(frontCells.vVelocity[bli], frontCells.vVelocity[bri], sampleFract.x);
			glm::vec2	vel_t = glm::mix(frontCells.vVelocity[tli], frontCells.vVelocity[tri], sampleFract.x);
			glm::vec2	new_vel = glm::mix(vel_b, vel_t, sampleFract.y);
			//	store the point
			backCells.vVelocity[iCellIndex] = new_vel;
			++iCellIndex;
		}
	}
}

void	DIYFluid::Diffuse(float fDt)
{
	float	fInverseViscDT = 1.0f / (fViscosity * fDt);
	for (int y = 0; y < iHeight; ++y)
	{
		int iCellIndex = y * iWidth;
		for (int x = 0; x < iWidth; ++x)
		{
//			int	iCellIndex = x + y * iWidth;
			//	gather the 4 velocities around us
			int	xp1 = glm::clamp(x + 1, 0, iWidth - 1);
			int	xm1 = glm::clamp(x - 1, 0, iWidth - 1);
			int	yp1 = glm::clamp(y + 1, 0, iHeight - 1);
			int	ym1 = glm::clamp(y - 1, 0, iHeight - 1);

			int	up = x + yp1 * iWidth;
			int	down = x + ym1 * iWidth;
			int	right = xp1 + y * iWidth;
			int	left = xm1 + y * iWidth;

			glm::vec2	velUp = frontCells.vVelocity[up];
			glm::vec2	velDown = frontCells.vVelocity[up];
			glm::vec2	velLeft = frontCells.vVelocity[up];
			glm::vec2	velRight = frontCells.vVelocity[up];

			glm::vec2	velCentre = frontCells.vVelocity[iCellIndex];

			//	put in diffusion equation

			float	denom = 1.0f / (4.0 + fInverseViscDT);

			glm::vec2	diffusedVelocity = (velUp + velRight + velDown + velLeft + velCentre * fInverseViscDT) * denom;

			backCells.vVelocity[iCellIndex] = diffusedVelocity;
			++iCellIndex;
		}

	}
}

void	DIYFluid::Divergence(float fDt)
{
	float	fInverseCellDist = 1.0f / (2.0f * fCellDist);
	for (int y = 0; y < iHeight; ++y)
	{
		int iCellIndex = y * iWidth;
		for (int x = 0; x < iWidth; ++x)
		{
//			int	iCellIndex = x + y * iWidth;
			//	gather the 4 velocities around us
			int	xp1 = glm::clamp(x + 1, 0, iWidth - 1);
			int	xm1 = glm::clamp(x - 1, 0, iWidth - 1);
			int	yp1 = glm::clamp(y + 1, 0, iHeight - 1);
			int	ym1 = glm::clamp(y - 1, 0, iHeight - 1);

			int	up = x + yp1 * iWidth;
			int	down = x + ym1 * iWidth;
			int	right = xp1 + y * iWidth;
			int	left = xm1 + y * iWidth;

			float	fVelUp = frontCells.vVelocity[up].y;
			float	fVelDown = frontCells.vVelocity[down].y;
			float	fVelLeft = frontCells.vVelocity[left].x;
			float	fVelRight = frontCells.vVelocity[right].x;

			float	fDivergence = ((fVelRight - fVelLeft) + (fVelUp - fVelDown)) * fInverseCellDist;
			aFDivergence[iCellIndex] = fDivergence;
			++iCellIndex;
		}
	}
}

void	DIYFluid::UpdatePressure(float fDt)
{
//	float	fInverseCellDist = 1.0f / (2.0f * fCellDist);
	for (int y = 0; y < iHeight; ++y)
	{
		int iCellIndex = y * iWidth;
		for (int x = 0; x < iWidth; ++x)
		{
//			int	iCellIndex = x + y * iWidth;
			//	gather the 4 velocities around us
			int	xp1 = glm::clamp(x + 1, 0, iWidth - 1);
			int	xm1 = glm::clamp(x - 1, 0, iWidth - 1);
			int	yp1 = glm::clamp(y + 1, 0, iHeight - 1);
			int	ym1 = glm::clamp(y - 1, 0, iHeight - 1);

			int	up = x + yp1 * iWidth;
			int	down = x + ym1 * iWidth;
			int	right = xp1 + y * iWidth;
			int	left = xm1 + y * iWidth;

			float	fPUp = frontCells.fPressure[up];
			float	fPDown = frontCells.fPressure[down];
			float	fPLeft = frontCells.fPressure[left];
			float	fPRight = frontCells.fPressure[right];

			float	d = aFDivergence[iCellIndex];
			float	fNewPressure = (fPUp + fPDown + fPLeft + fPRight - d * fCellDist * fCellDist) * 0.25f;

			backCells.fPressure[iCellIndex] = fNewPressure;
			++iCellIndex;
		}
	}
}

void	DIYFluid::ApplyPressure(float fDt)
{
	float	fInvCellDist = 1.0f / (2.0f * fCellDist);
	for (int y = 0; y < iHeight; ++y)
	{
		int iCellIndex = y * iWidth;
		for (int x = 0; x < iWidth; ++x)
		{
//			int	iCellIndex = x + y * iWidth;
			//	gather the 4 velocities around us
			int	xp1 = glm::clamp(x + 1, 0, iWidth - 1);
			int	xm1 = glm::clamp(x - 1, 0, iWidth - 1);
			int	yp1 = glm::clamp(y + 1, 0, iHeight - 1);
			int	ym1 = glm::clamp(y - 1, 0, iHeight - 1);

			int	up = x + yp1 * iWidth;
			int	down = x + ym1 * iWidth;
			int	right = xp1 + y * iWidth;
			int	left = xm1 + y * iWidth;

			float	fPUp = frontCells.fPressure[up];
			float	fPDown = frontCells.fPressure[down];
			float	fPLeft = frontCells.fPressure[left];
			float	fPRight = frontCells.fPressure[right];

			glm::vec2	fDeltaV = -glm::vec2(fPRight - fPLeft, fPUp - fPDown) * fInvCellDist;

			backCells.vVelocity[iCellIndex] = frontCells.vVelocity[iCellIndex] + fDeltaV;
			++iCellIndex;
		}
	}
}

void	DIYFluid::UpdateBoundary()
{
	float*	p = frontCells.fPressure;
	glm::vec2* v = frontCells.vVelocity;

	for (int x = 0; x < iWidth; ++x)
	{
		int	iFirstRowIndex = x;
		int iSecondRowIndex = x + iWidth;

		p[iFirstRowIndex] = p[iSecondRowIndex];

		v[iFirstRowIndex].x = v[iSecondRowIndex].x;
		v[iFirstRowIndex].y = -v[iSecondRowIndex].y;

		int iLastRowIndex = x + (iHeight - 1) * iWidth;
		int iSecondLastRowIndex = x + (iHeight - 2) * iWidth;

		p[iLastRowIndex] = p[iSecondLastRowIndex];
		v[iLastRowIndex].x = v[iSecondLastRowIndex].x;
		v[iLastRowIndex].y = -v[iSecondLastRowIndex].y;
	}

	for (int y = 0; y < iHeight; ++y)
	{
		int	iFirstColIndex = y * iWidth;
		int	iSecondColIndex = 1 + y * iWidth;

		p[iFirstColIndex] = p[iSecondColIndex];
		v[iFirstColIndex].x = -v[iSecondColIndex].x;
		v[iFirstColIndex].y = v[iSecondColIndex].y;

		int	iLastColIndex = (iWidth - 1) + y * iWidth;
		int	iSecondLastColIndex = (iWidth - 2) + y * iWidth;

		p[iLastColIndex] = p[iSecondLastColIndex];
		v[iLastColIndex].x = -v[iSecondLastColIndex].x;
		v[iLastColIndex].y = v[iSecondLastColIndex].y;
	}
}

void	DIYFluid::RenderFluid(glm::mat4 viewProj)
{
	//	Allocate space for texture data
	unsigned char*	texData = new unsigned char[iWidth * iHeight * 3];
	//	Fill texture with data with dye colours
	int iTimes3 = 0;
	for (int i = 0; i < iWidth * iHeight; ++i)
	{
		texData[iTimes3] =	 (unsigned char)frontCells.vDyeColour[i].r;
		texData[iTimes3 + 1] = (unsigned char)frontCells.vDyeColour[i].g;
		texData[iTimes3 + 2] = (unsigned char)frontCells.vDyeColour[i].b;
		iTimes3 += 3;
	}

	unsigned int textureHandle = CreateGLTextureBasic(texData, iWidth, iHeight, 3);
	unsigned int quadVAO = BuildQuadGLVAO(fQuadHalfSize);

	//	render texture using opengl
	RenderQuad(quadVAO, uiProgram, textureHandle, viewProj);

	//	delete texture
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteTextures(1, &textureHandle);

	delete[]	texData;
}

