#include "ProceduralGeneration.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"
#include <iostream>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using namespace std;

ProceduralGeneration::ProceduralGeneration()
{
}
ProceduralGeneration::~ProceduralGeneration()
{
	delete[] m_fPerlinData;
}

bool	ProceduralGeneration::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	//	check if we need to reload the shaders
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
	}

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);
	//	setup callbacks to send info to AntTweakBar
	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);

	m_BackgroundColour = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	//	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create();
	m_bDrawGizmos = true;

	//	now initialise the FlyCamera
	m_FlyCamera = FlyCamera(vec3(10, 10, 10), vec3(0, 0, 0), glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

	//	initialise basic AntTweakBar info
	m_bar = TwNewBar("Amazing new AntTweakBar!!");
	TwAddSeparator(m_bar, "Light Data", "");
	//	TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_light_dir, "label='Group Light'");
	//	TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR3F, &m_light_colour, "label='Group Light'");
	//	TwAddVarRW(m_bar, "Spec Power", TW_TYPE_FLOAT, &m_fSpecular_power, "label='Group Light' min=0.05 max=100 step=0.05");
	TwAddSeparator(m_bar, "Misc Data", "");
	TwAddVarRW(m_bar, "Clear Colour", TW_TYPE_COLOR4F, &m_BackgroundColour, "");
	TwAddVarRW(m_bar, "Draw Gizmos", TW_TYPE_BOOL8, &m_bDrawGizmos, "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fFPS, "");
	TwAddSeparator(m_bar, "Perlin Data", "");
	TwAddVarRW(m_bar, "Perlin Scale", TW_TYPE_FLOAT, &m_fScale, "min=0.02 max=15 step=0.02");
	TwAddVarRW(m_bar, "Perlin Octaves", TW_TYPE_UINT32, &m_uiOctaves, "min=1 max=50 step=1");
	TwAddVarRW(m_bar, "Perlin Real Width", TW_TYPE_FLOAT, &m_fRealWidth, "min=1 step=0.5");
	TwAddVarRW(m_bar, "Perlin Real Height", TW_TYPE_FLOAT, &m_fRealHeight, "min=1 step=0.5");
	TwAddVarRW(m_bar, "Perlin Mesh Width", TW_TYPE_UINT32, &m_iMeshWidth, "min=4 max=200 step=1");
	TwAddVarRW(m_bar, "Perlin Mesh Height", TW_TYPE_UINT32, &m_iMeshHeight, "min=4 max=200 step=1");

	m_fRealWidth = 150.0f;
	m_fRealHeight = 150.0f;
	m_iMeshWidth = 150;
	m_iMeshHeight = 150;

	m_GridDimensions = vec2(m_fRealWidth, m_fRealHeight);
	m_MeshDimensions = glm::ivec2(m_iMeshWidth, m_iMeshHeight);

	BuildGrid(m_GridDimensions, m_MeshDimensions);
	m_fScale = 0.7f;
	m_uiOctaves = 6;
	BuildPerlinTexture(m_MeshDimensions, m_uiOctaves, m_fScale);

	LoadShader("shaders/perlin_vertex.glsl", 0, "shaders/perlin_fragment.glsl", &m_uiProgramID);

	return true;
}

void	ProceduralGeneration::shutdown()
{
	//	now clean up
	Gizmos::destroy();
	TwDeleteAllBars();
	TwTerminate();
}

bool	ProceduralGeneration::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	//	check if we need to reload the shaders
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
	}

	float	dT = (float)glfwGetTime();
	glfwSetTime(0.0f);
	m_fFPS = (float)(1.0 / dT);
	//	now we get to the fun stuff
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();
	Gizmos::addTransform(mat4(1));

	m_timer += dT;
	m_FlyCamera.update(dT);

	vec4	white(1);
	vec4	black(0, 0, 0, 1);
	vec4	blue(0, 0, 1, 1);
	vec4	yellow(1, 1, 0, 1);
	vec4	green(0, 1, 0, 1);
	vec4	red(1, 0, 0, 1);
	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? white : black);
	}
	static unsigned int s_uiLastOctave = m_uiOctaves;
	static unsigned int s_uiLastMeshWidth = m_iMeshWidth;
	static unsigned int s_uiLastMeshHeight = m_iMeshHeight;
	static float	s_fLastRealWidth = m_fRealWidth;
	static float	s_fLastRealHeight = m_fRealHeight;
	static float	s_fLastPerlinScale = m_fScale;

	if ((s_uiLastOctave != m_uiOctaves) || (s_fLastPerlinScale != m_fScale))
	{
		BuildPerlinTexture(m_MeshDimensions, m_uiOctaves, m_fScale);
		s_uiLastOctave = m_uiOctaves;
		s_fLastPerlinScale = m_fScale;
		cout << "Perlin Parameters changed: Octaves: " << m_uiOctaves << "  Scale: " << m_fScale << '\n';
	}
	else if ((s_uiLastMeshWidth != m_iMeshWidth) || (s_uiLastMeshHeight != m_iMeshHeight)
		|| (s_fLastRealWidth != m_fRealWidth) || (s_fLastRealHeight != m_fRealHeight))
	{
		//	update the Perlin Noise dimensions
		m_GridDimensions = vec2(m_fRealWidth, m_fRealHeight);
		m_MeshDimensions = glm::ivec2(m_iMeshWidth, m_iMeshHeight);

		BuildPerlinTexture(m_MeshDimensions, m_uiOctaves, m_fScale);
		BuildGrid(m_GridDimensions, m_MeshDimensions);

		s_uiLastMeshWidth = m_iMeshWidth;
		s_uiLastMeshHeight = m_iMeshHeight;
		s_fLastRealWidth = m_fRealWidth;
		s_fLastRealHeight = m_fRealHeight;
		cout << "Mesh Parameters changed: MeshWidth: " << m_iMeshWidth << " MeshHeight: " << m_iMeshHeight << "  RealWidth: " << m_fRealWidth <<" RealHeight: " << m_fRealHeight << '\n';
	}



	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	return true;
}

void	ProceduralGeneration::draw()
{
	Application::draw();

	glUseProgram(m_uiProgramID);
	int	iViewProjUniform = glGetUniformLocation(m_uiProgramID, "view_proj");
	glUniformMatrix4fv(iViewProjUniform, 1, GL_FALSE, (float*)&m_FlyCamera.m_projectionViewTransform);

	glBindVertexArray(m_PlaneMesh.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_PlaneMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);

	int iTexUniform = glGetUniformLocation(m_uiProgramID, "perlin_texture");
	glUniform1i(iTexUniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiPerlinTexture);

	static unsigned int s_uiFrameCounter = 0;
	++s_uiFrameCounter;

	if ((glfwGetKey(m_window, GLFW_KEY_T) == GLFW_PRESS) && (s_uiFrameCounter > 200))
	{
		//	it's next to the R key so ...
		cout << "Perlin details (draw): Highest: " << m_fHighest << " Lowest: " << m_fLowest << " Scale: " << m_fScale << '\n';
		s_uiFrameCounter = 0;
	}

	int iScaleUniform = glGetUniformLocation(m_uiProgramID, "fPerlinScale");
	glUniform1f(iScaleUniform, m_fScale);

	int iHighestUniform = glGetUniformLocation(m_uiProgramID, "fHighest");
	glUniform1f(iHighestUniform, m_fHighest);

	int iLowestUniform = glGetUniformLocation(m_uiProgramID, "fLowest");
	glUniform1f(iLowestUniform, m_fLowest);

	glBindVertexArray(m_PlaneMesh.m_uiVAO);
	glDrawElements(GL_LINES, m_PlaneMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);

	if (m_bDrawGizmos)
	{
		Gizmos::draw(m_FlyCamera.GetProjectionView());
	}
	TwDraw();
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void	ProceduralGeneration::BuildGrid(vec2 a_RealDims, glm::ivec2 a_Dims)
{
	//	This function generates the grid we will use for the height map
	//	a_RealDims is the real world dimensions of the grid
	//	a_Dims is the number of rows and columns

	if (m_PlaneMesh.m_uiIndexCount > 0)
	{
		//	first delete any previous buffers if they exist
		glDeleteVertexArrays(1, &m_PlaneMesh.m_uiVAO);
		glDeleteBuffers(1, &m_PlaneMesh.m_uiVBO);
		glDeleteBuffers(1, &m_PlaneMesh.m_uiIBO);
	}
	//	compute how many vertices we need
	unsigned int	iVertexCount = (a_Dims.x + 1) * (a_Dims.y + 1);
	//	allocate vertex data
	VertexTexCoord*	vertexData = new VertexTexCoord[iVertexCount];

	//	compute how many indices we need
	unsigned int	iIndexCount = a_Dims.x * a_Dims.y * 6;
	//	allocate index data
	unsigned int*	indexData = new unsigned int[iIndexCount];

	//	two nested for loops to generate vertex data
	float	fCurrY = -a_RealDims.y * 0.5f;
	for (unsigned int y = 0; y < a_Dims.y + 1; ++y)
	{
		float	fCurrX = -a_RealDims.x * 0.5f;
		for (unsigned int x = 0; x < a_Dims.x + 1; ++x)
		{
			//	inside we create our points, with the grid centred at (0, 0)
			vertexData[y * (a_Dims.x + 1) + x].position = vec4(fCurrX, 0, fCurrY, 1);
			vertexData[y * (a_Dims.x + 1) + x].tex_coord = vec2((float)x / (float)a_Dims.x, (float)y / (float)a_Dims.y);
			fCurrX += a_RealDims.x / (float)a_Dims.x;
		}
		fCurrY += a_RealDims.y / (float)a_Dims.y;
	}

	//	two nested for loops to generate index data
	int	iCurrIndex = 0;
	for (unsigned int y = 0; y < a_Dims.y; ++y)
	{
		for (unsigned int x = 0; x < a_Dims.x; ++x)
		{
			//	create our 6 indices here!!
			indexData[iCurrIndex++] = y * (a_Dims.x + 1) + x;
			indexData[iCurrIndex++] = (y + 1) * (a_Dims.x + 1) + x;
			indexData[iCurrIndex++] = (y + 1) * (a_Dims.x + 1) + x + 1;

			indexData[iCurrIndex++] = (y + 1) * (a_Dims.x + 1) + x + 1;
			indexData[iCurrIndex++] = y * (a_Dims.x + 1) + x + 1;
			indexData[iCurrIndex++] = y * (a_Dims.x + 1) + x;
		}
	}

	m_PlaneMesh.m_uiIndexCount = iIndexCount;
	//	create VertexArrayObject, buffers, etc
	glGenVertexArrays(1, &m_PlaneMesh.m_uiVAO);
	glGenBuffers(1, &m_PlaneMesh.m_uiVBO);
	glGenBuffers(1, &m_PlaneMesh.m_uiIBO);

	//	bind and fill buffers
	glBindVertexArray(m_PlaneMesh.m_uiVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_PlaneMesh.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_PlaneMesh.m_uiIBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexCoord)* iVertexCount, vertexData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* iIndexCount, indexData, GL_STATIC_DRAW);

	//	tell OpenGL about our vertex structure
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), (void*)sizeof(vec4));

	//	unbind stuff!!
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//	free vertex and index data
	delete[]	vertexData;
	delete[]	indexData;
}

void	ProceduralGeneration::BuildPerlinTexture(glm::ivec2 a_Dims, unsigned int a_uiOctaves, float a_fPersistence)
{
	//	set scale
	float	fScaleX = (1.0f / a_Dims.x) * 6.14159265358979f;
	float	fScaleY = (1.0f / a_Dims.y) * 6.14159265358979f;

	//	first delete any previously allocated Perlin data
	delete[] m_fPerlinData;
	//	allocate memory for Perlin data
	m_fPerlinData = new float[a_Dims.x * a_Dims.y];
	m_fHighest = 0.0f;
	m_fLowest = 999999999999.0f;
	//	loop through all the pixels
	for (unsigned int y = 0; y < a_Dims.y; ++y)
	{
		for (unsigned int x = 0; x < a_Dims.x; ++x)
		{
			float fAmplitude = 1.0f;
			float fFrequency = 1.0f;
			unsigned int uiCurrentIndex = y * a_Dims.x + x;
			m_fPerlinData[uiCurrentIndex] = 0.0f;
			//	loop over a number of octaves - however many got passed in
			for (unsigned int o = 0; o < a_uiOctaves; ++o)
			{
				//	call glm::perlin function to generate our perlin noise, including scaling to put it in the range 0-1
				float fPerlinSample = glm::perlin(vec2((float)x * fScaleX, (float)y * fScaleY) * fFrequency) * 0.5f + 0.5f;
				fPerlinSample *= fAmplitude;
				m_fPerlinData[uiCurrentIndex] += fPerlinSample;

				fAmplitude *= a_fPersistence;
				fFrequency *= 2.0f;
			}
			//	now check the highest and lowest samples ...
			if (m_fPerlinData[uiCurrentIndex] > m_fHighest)
			{
				m_fHighest = m_fPerlinData[uiCurrentIndex];
			}
			if (m_fPerlinData[uiCurrentIndex] < m_fLowest)
			{
				m_fLowest = m_fPerlinData[uiCurrentIndex];
			}
		}
	}

	cout << "Perlin details (pre-zero): Highest: " << m_fHighest << " Lowest: " << m_fLowest << '\n';
	//	now subtract the lowest noise value from every value so that zero should always the lowest value in the texture
	float	fTempLowest = 999999999999.0f;
	for (unsigned int y = 0; y < a_Dims.y; ++y)
	{
		for (unsigned int x = 0; x < a_Dims.x; ++x)
		{
			unsigned int uiCurrentIndex = y * a_Dims.x + x;
			m_fPerlinData[uiCurrentIndex] -= m_fLowest;
			if (m_fPerlinData[uiCurrentIndex] < fTempLowest)
			{
				fTempLowest = m_fPerlinData[uiCurrentIndex];	//	get the actual lowest value stored after the subtraction
			}
		}
	}
	m_fHighest -= m_fLowest;
	m_fLowest = fTempLowest;
	cout << "Perlin details (post-zero): Highest: " << m_fHighest << " Lowest: " << m_fLowest << '\n';
	//	hmm, given I am already taking care of these in the shader then I don't need to rescale on the cpu here

	//	generate OpenGL texture handles
	glGenTextures(1, &m_uiPerlinTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiPerlinTexture);

	//	pass perlin data to the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, a_Dims.x, a_Dims.y, 0, GL_RED, GL_FLOAT, m_fPerlinData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


void	ProceduralGeneration::ReloadShader()
{
	glDeleteProgram(m_uiProgramID);
	LoadShader("shaders/perlin_vertex.glsl", 0, "shaders/perlin_fragment.glsl", &m_uiProgramID);
}
