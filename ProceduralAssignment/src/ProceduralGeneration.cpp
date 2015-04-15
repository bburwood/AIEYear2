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
	m_fTotalTime = 0.0f;

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);
	//	setup callbacks to send info to AntTweakBar
	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);

	m_BackgroundColour = vec4(0.3f, 0.3f, 0.8f, 1.0f);
	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	//	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create();
	m_bDrawGizmos = true;

	//	now initialise the FlyCamera
	m_FlyCamera = FlyCamera(vec3(10, 10, 10), vec3(0, 0, 0), glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 3000.0f);
	m_FlyCamera.SetSpeed(250.0f);

	//	initialise basic AntTweakBar info
	m_bar = TwNewBar("Stuff you can mess with!!");
	//TwAddSeparator(m_bar, "Light Data", "");
	//	TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_light_dir, "label='Group Light'");
	//	TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR3F, &m_light_colour, "label='Group Light'");
	//	TwAddVarRW(m_bar, "Spec Power", TW_TYPE_FLOAT, &m_fSpecular_power, "label='Group Light' min=0.05 max=100 step=0.05");
	TwAddSeparator(m_bar, "Misc Data", "");
	TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_vLightDir, "label='Light Direction'");
	TwAddVarRW(m_bar, "Clear Colour", TW_TYPE_COLOR4F, &m_BackgroundColour, "");
	TwAddVarRW(m_bar, "Draw Gizmos", TW_TYPE_BOOL8, &m_bDrawGizmos, "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fFPS, "");
	TwAddVarRW(m_bar, "Terrain Height", TW_TYPE_FLOAT, &m_fTerrainHeight, "min=1 max=250 step=0.5");
	TwAddVarRW(m_bar, "Flying Speed", TW_TYPE_FLOAT, &m_FlyCamera.m_fSpeed, "min=1 max=250 step=0.5");
	TwAddSeparator(m_bar, "Perlin Data", "");
	TwAddVarRW(m_bar, "Perlin Scale", TW_TYPE_FLOAT, &m_fPerlinScale, "min=0.02 max=15 step=0.02");
	TwAddVarRW(m_bar, "Perlin Octaves", TW_TYPE_UINT32, &m_uiOctaves, "min=1 max=50 step=1");
	TwAddVarRW(m_bar, "Perlin Real Width", TW_TYPE_FLOAT, &m_fRealWidth, "min=1 max=1500 step=0.5");
	TwAddVarRW(m_bar, "Perlin Real Height", TW_TYPE_FLOAT, &m_fRealHeight, "min=1 max=1500 step=0.5");
	TwAddVarRW(m_bar, "Perlin Mesh Width", TW_TYPE_UINT32, &m_iMeshWidth, "min=4 max=200 step=1");
	TwAddVarRW(m_bar, "Perlin Mesh Height", TW_TYPE_UINT32, &m_iMeshHeight, "min=4 max=200 step=1");
	TwAddSeparator(m_bar, "Other Data", "");
	TwAddVarRW(m_bar, "Emitter Lifespan", TW_TYPE_FLOAT, &m_fEmitterLifespan, "min=0.25 max=25 step=0.25");
	TwAddVarRW(m_bar, "Emitter MaxParticles", TW_TYPE_UINT32, &m_uiEmitterMaxParticles, "min=1000 max=25000 step=100");
	TwAddVarRW(m_bar, "Emitter Emit Rate", TW_TYPE_FLOAT, &m_fEmitRate, "min=100 max=10000 step=100");

	m_fRealWidth = 1000.0f;
	m_fRealHeight = 1000.0f;
	m_iMeshWidth = 150;
	m_iMeshHeight = 150;
	m_fTerrainHeight = 150.0f;

	m_GridDimensions = vec2(m_fRealWidth, m_fRealHeight);
	m_MeshDimensions = glm::ivec2(m_iMeshWidth, m_iMeshHeight);

	BuildGrid(m_GridDimensions, m_MeshDimensions);
	m_fPerlinScale = 0.5f;
	m_uiOctaves = 6;
	BuildPerlinTexture(m_MeshDimensions, m_uiOctaves, m_fPerlinScale);
	//	the following line sets the initial camera position to be above the highest point on the terrain
	//m_FlyCamera.SetLookAt(vec3(m_fHighestX, m_fTerrainHeight * 1.1f, m_fHighestZ), vec3(0, m_fTerrainHeight * 1.1f, 0), vec3(0, 1, 0));
	//	the following line sets the initial camera position to be above the lowest point on the terrain
	m_FlyCamera.SetLookAt(vec3(m_fLowestX, m_fTerrainHeight * 1.1f, m_fLowestZ), vec3(0, m_fTerrainHeight * 1.1f, 0), vec3(0, 1, 0));
	m_F16CopyTransform = glm::scale(glm::translate(vec3(m_fHighestX, m_fHighest * m_fTerrainHeight, m_fHighestZ)), vec3(4.0f, 4.0f, 4.0f));

	//	Load the terrain textures
	LoadTexture("./textures/water_01_512.jpg", m_uiWaterTexture);
	LoadTexture("./textures/SnowTexture_512.png", m_uiSnowTexture);
	LoadTexture("./textures/grass_texture_512.jpg", m_uiGrassTexture);
	//	Load the particle texture
	LoadAlphaTexture("./textures/particleTexture.png", m_uiParticleTexture);
	//	Load the F16 texture
	LoadTexture("./models/f16/f16s.bmp", m_uiF16Texture);
	//	and pass the particle texture to all particle emitters
	for (unsigned int i = 0; i < c_iNUM_EMITTERS; ++i)
	{
		m_emitters[i].SetParticleTexture(m_uiParticleTexture);
	}

	cout << "PerlinTexture: " << m_uiPerlinTexture << " WaterTexture: " << m_uiWaterTexture << " SnowTexture: " << m_uiSnowTexture
		<< " GrassTexture: " << m_uiGrassTexture << " ParticleTexture: " << m_uiParticleTexture << '\n';

	LoadShader("shaders/perlin_vertex.glsl", 0, "shaders/perlin_fragment.glsl", &m_uiProgramID);

	//	initialise the GPU Particle emitter variables
	m_fFiringTimer = 0.0f;
	m_fFiringInterval = 1.0f;
	m_fEmitterLifespan = 10.0f;
	m_fEmitterParticleLifespan = 4.0f;
	m_uiEmitterMaxParticles = 12000;
	m_fEmitRate = 4000.0f;
	m_iNextEmitterToFire = 0;
	m_vLightDir = glm::normalize(vec3(-0.50f, -0.5f, -0.50f));

	//	now load the meshes
	m_F16Mesh = LoadTexturedOBJ("./models/f16/f16.obj");
	m_F16CopyMesh = LoadTexturedOBJ("./models/f16/f16.obj");
	//OpenGLData	testMesh = LoadTexturedOBJ("./models/f16/f16.obj");
	//m_F16Mesh = LoadOBJ("./models/stanford/bunny.obj");

//	LoadShader("./shaders/normal_mapped_vertex.glsl", nullptr, "./shaders/normal_mapped_fragment.glsl", &m_uiModelProgramID);
	LoadShader("./shaders/lighting_vertex.glsl", nullptr, "./shaders/lighting_fragment.glsl", &m_uiModelProgramID);

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
	m_fFPS = (float)(1.0f / dT);
	//	now we get to the fun stuff
	m_FlyCamera.update(dT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();
	Gizmos::addTransform(mat4(1));	//	adds the red green blue unit vectors to the drawn grid of lines
	vec4	white(1);
	vec4	black(0, 0, 0, 1);
	vec4	blue(0, 0, 1, 1);
	vec4	yellow(1, 1, 0, 1);
	vec4	green(0, 1, 0, 1);
	vec4	red(1, 0, 0, 1);

	m_timer += dT;
	m_fTotalTime += dT;
	m_fFiringTimer += dT;
	vec3	vCamForward = m_FlyCamera.GetForwardDirection();
	vCamForward = glm::normalize(vCamForward);
	vec3	vCamUp = m_FlyCamera.GetUpDirection();
	vCamUp = glm::normalize(vCamUp);
	vec3	vCamRight = m_FlyCamera.GetRightDirection();
	vCamRight = glm::normalize(vCamRight);
//	if (m_FlyCamera.m_bRotatedThisFrame)
//	{
//		m_F16Transform = m_FlyCamera.m_LastRotation * glm::translate(m_FlyCamera.GetPosition() + (vCamForward * 6.0f));
//	}
//	else
//	{
//		m_F16Transform = glm::translate(m_FlyCamera.GetPosition() + (vCamForward * 6.0f));
//	}
//	m_F16Transform = glm::inverse(m_FlyCamera.m_LastRotation) * m_F16Transform;
	//m_F16Transform[0] = m_FlyCamera.m_LastRotation * m_F16Transform[0];
	//m_F16Transform[1] = m_FlyCamera.m_LastRotation * m_F16Transform[1];
	//m_F16Transform[2] = m_FlyCamera.m_LastRotation * m_F16Transform[2];

	m_F16Transform = glm::translate(m_FlyCamera.GetPosition() + (vCamForward * 6.0f) - (vCamUp * 1.5f));

	if ((glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) && (m_fFiringTimer > m_fFiringInterval))
	{
		//	fire another particle effect
		cout << "Firing emitter: " << m_iNextEmitterToFire << " m_fFiringTimer:" << m_fFiringTimer << '\n';
		m_emitters[m_iNextEmitterToFire].Init(m_uiEmitterMaxParticles,
			m_FlyCamera.GetPosition() + (vCamForward * 0.15f * m_FlyCamera.m_fSpeed) - (vCamUp * 0.15f * m_FlyCamera.m_fSpeed) + (vCamRight * ((float)m_iNextEmitterToFire - (0.5f * c_iNUM_EMITTERS) + 0.5f) * 10.0f),
			vCamForward * m_FlyCamera.m_fSpeed * 0.75f,
			m_fEmitRate, m_fEmitterLifespan, 0.1f * m_fEmitterParticleLifespan, m_fEmitterParticleLifespan,
			0.02f * m_FlyCamera.m_fSpeed, 0.04f * m_FlyCamera.m_fSpeed, 2.5f,
			0.003f * m_fTerrainHeight, 0.01f * m_fTerrainHeight,
			vec4(0.2f, 0.4f, 1.0f, 1), vec4(1, 1, 0.5f, 0.3), m_iNextEmitterToFire);
		m_iNextEmitterToFire = (m_iNextEmitterToFire + 1) % c_iNUM_EMITTERS;
		m_fFiringTimer = 0.0f;
	}

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
	static float	s_fLastPerlinScale = m_fPerlinScale;

	if ((s_uiLastOctave != m_uiOctaves) || (s_fLastPerlinScale != m_fPerlinScale))
	{
		BuildPerlinTexture(m_MeshDimensions, m_uiOctaves, m_fPerlinScale);
		s_uiLastOctave = m_uiOctaves;
		s_fLastPerlinScale = m_fPerlinScale;
		cout << "Perlin Parameters changed: Octaves: " << m_uiOctaves << "  Scale: " << m_fPerlinScale << '\n';
	}
	else if ((s_uiLastMeshWidth != m_iMeshWidth) || (s_uiLastMeshHeight != m_iMeshHeight)
		|| (s_fLastRealWidth != m_fRealWidth) || (s_fLastRealHeight != m_fRealHeight))
	{
		//	update the Perlin Noise dimensions
		m_GridDimensions = vec2(m_fRealWidth, m_fRealHeight);
		m_MeshDimensions = glm::ivec2(m_iMeshWidth, m_iMeshHeight);

		//BuildPerlinTexture(m_MeshDimensions, m_uiOctaves, m_fPerlinScale);
		BuildGrid(m_GridDimensions, m_MeshDimensions);

		s_uiLastMeshWidth = m_iMeshWidth;
		s_uiLastMeshHeight = m_iMeshHeight;
		s_fLastRealWidth = m_fRealWidth;
		s_fLastRealHeight = m_fRealHeight;
		cout << "Mesh Parameters changed: MeshWidth: " << m_iMeshWidth << " MeshHeight: " << m_iMeshHeight << "  RealWidth: " << m_fRealWidth << " RealHeight: " << m_fRealHeight << '\n';
		cout << "Terrain Height: " << m_fTerrainHeight << '\n';
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

	static unsigned int s_uiFrameCounter = 0;
	++s_uiFrameCounter;

	if ((glfwGetKey(m_window, GLFW_KEY_T) == GLFW_PRESS) && (s_uiFrameCounter > 200))
	{
		//	it's next to the R key so ...
		cout << "Perlin details (draw): Highest: " << m_fHighest << " Lowest: " << m_fLowest << " Scale: " << m_fPerlinScale << '\n';
		s_uiFrameCounter = 0;
	}

	int iTexUniform = glGetUniformLocation(m_uiProgramID, "perlin_texture");
	glUniform1i(iTexUniform, 0);

	int iWaterTexUniform = glGetUniformLocation(m_uiProgramID, "water_texture");
	glUniform1i(iWaterTexUniform, 1);

	int iSnowTexUniform = glGetUniformLocation(m_uiProgramID, "snow_texture");
	glUniform1i(iSnowTexUniform, 2);

	int iGrassTexUniform = glGetUniformLocation(m_uiProgramID, "grass_texture");
	glUniform1i(iGrassTexUniform, 3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiPerlinTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_uiWaterTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_uiSnowTexture);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_uiGrassTexture);


	int iScaleUniform = glGetUniformLocation(m_uiProgramID, "fPerlinScale");
	glUniform1f(iScaleUniform, m_fPerlinScale);
	int iHighestUniform = glGetUniformLocation(m_uiProgramID, "fHighest");
	glUniform1f(iHighestUniform, m_fHighest);
	int iLowestUniform = glGetUniformLocation(m_uiProgramID, "fLowest");
	glUniform1f(iLowestUniform, m_fLowest);
	int iTerrainHeightUniform = glGetUniformLocation(m_uiProgramID, "fTerrainHeight");
	glUniform1f(iTerrainHeightUniform, m_fTerrainHeight);
	int iTerrainGridDimensionsUniform = glGetUniformLocation(m_uiProgramID, "vMeshDims");
	glUniform2iv(iTerrainGridDimensionsUniform, 1, (int*)&m_MeshDimensions);

	int	eye_pos_uniform = glGetUniformLocation(m_uiProgramID, "eye_pos");
	glUniform3fv(eye_pos_uniform, 1, (float*)&m_FlyCamera.GetPosition());
	int	iLightDirUniform = glGetUniformLocation(m_uiProgramID, "light_dir");
	glUniform3fv(iLightDirUniform, 1, (float*)&m_vLightDir);

	glBindVertexArray(m_PlaneMesh.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_PlaneMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);

	//	just draws the terrain mesh ... delete this!!
//	glBindVertexArray(m_PlaneMesh.m_uiVAO);
//	glDrawElements(GL_LINES, m_PlaneMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);

	//	now draw the loaded meshes
	DrawModels();

	//	now draw any particle emitters
	for (unsigned int i = 0; i < c_iNUM_EMITTERS; ++i)
	{
		m_emitters[i].Draw(1.0f / m_fFPS, m_FlyCamera.m_worldTransform, m_FlyCamera.GetProjectionView());
	}

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
				//	remember the highest point x and z coordinates
				m_fHighestX = (((float)x - (0.5f * (float)a_Dims.x)) / (float)a_Dims.x) * m_fRealWidth;
				m_fHighestZ = (((float)y - (0.5f * (float)a_Dims.y)) / (float)a_Dims.y) * m_fRealHeight;
				//cout << "Perlin: HighestX: " << m_fHighestX << " X: " << (int)x - (a_Dims.x / 2) << " HighestZ: " << m_fHighestZ << " Z: " << (int)y - (a_Dims.y / 2) << '\n';
			}
			if (m_fPerlinData[uiCurrentIndex] < m_fLowest)
			{
				m_fLowest = m_fPerlinData[uiCurrentIndex];
				//	remember the lowest point x and z coordinates
				m_fLowestX = (((float)x - (0.5f * (float)a_Dims.x)) / (float)a_Dims.x) * m_fRealWidth;
				m_fLowestZ = (((float)y - (0.5f * (float)a_Dims.y)) / (float)a_Dims.y) * m_fRealHeight;
			}
		}
	}

	cout << "Perlin details (pre-zero): Highest: " << m_fHighest << " Lowest: " << m_fLowest << '\n';
	//	now subtract the lowest noise value from every value so that zero should always the lowest value in the texture
	float	fTempLowest = 999999999999.0f;
	m_fHighest = 0.0f;
	for (unsigned int y = 0; y < a_Dims.y; ++y)
	{
		for (unsigned int x = 0; x < a_Dims.x; ++x)
		{
			unsigned int uiCurrentIndex = y * a_Dims.x + x;
			m_fPerlinData[uiCurrentIndex] -= m_fLowest;
			if (m_fPerlinData[uiCurrentIndex] > m_fHighest)
			{
				m_fHighest = m_fPerlinData[uiCurrentIndex];
			}
			if (m_fPerlinData[uiCurrentIndex] < fTempLowest)
			{
				fTempLowest = m_fPerlinData[uiCurrentIndex];	//	get the actual lowest value stored after the subtraction
			}
		}
	}
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
	glDeleteProgram(m_uiModelProgramID);
	LoadShader("shaders/perlin_vertex.glsl", 0, "shaders/perlin_fragment.glsl", &m_uiProgramID);
//	LoadShader("./shaders/normal_mapped_vertex.glsl", nullptr, "./shaders/normal_mapped_fragment.glsl", &m_uiModelProgramID);
	LoadShader("./shaders/lighting_vertex.glsl", nullptr, "./shaders/lighting_fragment.glsl", &m_uiModelProgramID);
}

void	ProceduralGeneration::DrawModels()
{
	glUseProgram(m_uiModelProgramID);

	int	iViewProjUniform = glGetUniformLocation(m_uiModelProgramID, "projection_view");
	glUniformMatrix4fv(iViewProjUniform, 1, GL_FALSE, (float*)&m_FlyCamera.GetProjectionView());
	int	iWordPosUniform = glGetUniformLocation(m_uiModelProgramID, "worldTransform");
	glUniformMatrix4fv(iWordPosUniform, 1, GL_FALSE, (float*)&m_F16Transform);

	int	ambient_uniform = glGetUniformLocation(m_uiModelProgramID, "ambient_light");
	int	light_colour_uniform = glGetUniformLocation(m_uiModelProgramID, "light_colour");
	int	light_dir_uniform = glGetUniformLocation(m_uiModelProgramID, "light_dir");
	//int	material_colour_uniform = glGetUniformLocation(m_uiModelProgramID, "material_colour");
	int	eye_pos_uniform = glGetUniformLocation(m_uiModelProgramID, "eye_pos");
	int	specular_uniform = glGetUniformLocation(m_uiModelProgramID, "specular_power");
	//int	timer_uniform = glGetUniformLocation(m_uiModelProgramID, "timer");

	int iTexUniform = glGetUniformLocation(m_uiModelProgramID, "albedoTexture");
	glUniform1i(iTexUniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiF16Texture);

	glUniform3fv(ambient_uniform, 1, (float*)&vec3(0.1f, 0.1f, 0.1f));
	glUniform3fv(light_colour_uniform, 1, (float*)&vec3(1.0f, 1.0f, 1.0f));
	glUniform3fv(light_dir_uniform, 1, (float*)&m_vLightDir);
	//glUniform3fv(light_dir_uniform, 1, (float*)&vec3(0.9f, 1.5f, 0.8f));
	//glUniform3fv(material_colour_uniform, 1, (float*)&vec3(0.1f, 0.2f, 0.5f));

	vec3	camera_pos = m_FlyCamera.GetPosition();
	glUniform3fv(eye_pos_uniform, 1, (float*)&camera_pos);
	glUniform1f(specular_uniform, 12.0f);
	//glUniform1f(timer_uniform, m_timer);

	glBindVertexArray(m_F16Mesh.m_uiVAO);

	if (glfwGetKey(m_window, GLFW_KEY_M) == GLFW_PRESS)
	{
		//	just draw the mesh if the M key is pressed
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glDrawElements(GL_TRIANGLES, m_F16Mesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);

	//	now draw the second plane
	iWordPosUniform = glGetUniformLocation(m_uiModelProgramID, "worldTransform");
	glUniformMatrix4fv(iWordPosUniform, 1, GL_FALSE, (float*)&m_F16CopyTransform);
	glBindVertexArray(m_F16CopyMesh.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_F16CopyMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);


}
