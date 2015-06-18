#include "Checkers.h"
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

//	globals (at least in this file), ick! I know, but this was trying to get the AntTweakBar enums to work, and this was the quickest way to make it happen.
TwType	ATB_PlayerType;
int	P1Type, P1Prev, P2Type, P2Prev;

Checkers::Checkers()
{
}

Checkers::~Checkers()
{
}

bool	Checkers::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	m_fTotalTime = 0.0f;

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(BUFFER_WIDTH, BUFFER_HEIGHT);
	//	setup callbacks to send info to AntTweakBar
	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);

	m_BackgroundColour = vec4(0.1f, 0.1f, 0.3f, 1.0f);
	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	//	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create();
	m_bDrawGizmos = true;
	m_bResetGame = false;
	m_bPieceSelected = false;

	//	now initialise the FlyCamera
	m_FlyCamera = FlyCamera(vec3(-1, 10, 5), vec3(-1, 0, 1), glm::radians(50.0f), (float)BUFFER_WIDTH / (float)BUFFER_HEIGHT, 0.1f, 300.0f);
	m_FlyCamera.SetSpeed(15.0f);

	//	initialise basic AntTweakBar info
	//m_bar = TwNewBar("Stuff you can mess with!!");
	m_bar = TwNewBar("GeneralStuff");	//	must be a single word (no spaces) if you want to be able to resize it
	TwDefine(" GeneralStuff size='320 600' "); // resize bar

	//	set up the Player type AntTweakBar enum
	TwEnumVal	ATB_PlayerTypeEnumVals[] = { { 0, "HUMAN" }, { 1, "MCTS_AI" } };
	//	ATB_PlayerType = TwDefineEnum("PlayerType", ATB_PlayerTypeEnumVals, 2);	//	change the "2" to the number of values in the type if/when changed
	ATB_PlayerType = TwDefineEnumFromString("ATB_PlayerType", "HUMAN, MCTS_AI");
//	P1Type = ATB_PlayerType;
//	P2Type = ATB_PlayerType;
//	P1Prev = ATB_PlayerType;
//	P1Prev = ATB_PlayerType;

	TwAddSeparator(m_bar, "Misc Data", "");
	TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_vLightDir, "label='Light Direction'");
	TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR4F, &m_vLightColour, "");
	TwAddVarRW(m_bar, "Ambient Colour", TW_TYPE_COLOR4F, &m_vAmbientLightColour, "");
	TwAddVarRW(m_bar, "Clear Colour", TW_TYPE_COLOR4F, &m_BackgroundColour, "");
	TwAddVarRW(m_bar, "Draw Gizmos", TW_TYPE_BOOL8, &m_bDrawGizmos, "");
	TwAddVarRW(m_bar, "Draw Debug Gizmos", TW_TYPE_BOOL8, &m_bDebug, "");
	TwAddVarRW(m_bar, "Deferred Lighting", TW_TYPE_BOOL8, &m_bDeferredRendering, "");
	TwAddVarRW(m_bar, "Camera Speed", TW_TYPE_FLOAT, &m_FlyCamera.m_fSpeed, "min=1 max=250 step=1");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fFPS, "");

	TwAddSeparator(m_bar, "Game Data", "");
	TwAddVarRW(m_bar, "Reset Game", TW_TYPE_BOOL8, &m_bResetGame, "");
	TwAddVarRW(m_bar, "Start Player", TW_TYPE_INT32, &m_iPlayerToMoveFirst, "min=1 max=2 step=1");
	TwAddVarRW(m_bar, "Piece Movement Speed", TW_TYPE_FLOAT, &m_Game.m_fMoveSpeed, "min=0.2 max=10 step=0.1");
	TwAddVarRW(m_bar, "No Capture Turn Limiting Moves", TW_TYPE_INT32, &m_Game.m_iNoCaptureTurnsLimit, "min=10 max=100 step=2");
	TwAddVarRO(m_bar, "Current No Capture Move Count", TW_TYPE_INT32, &m_Game.m_iTurnsSinceLastCapture, "");
	TwAddVarRO(m_bar, "Current Player Colour", TW_TYPE_COLOR4F, &m_CurrentPlayerColour, "");
	TwAddVarRW(m_bar, "Emitter Lifespan", TW_TYPE_FLOAT, &m_fEmitterLifespan, "min=0.25 max=25 step=0.25");
	TwAddVarRW(m_bar, "Emitter MaxParticles", TW_TYPE_UINT32, &m_uiEmitterMaxParticles, "min=100 max=50000 step=100");
	TwAddVarRW(m_bar, "Emitter Emit Rate", TW_TYPE_FLOAT, &m_fEmitRate, "min=10 max=20000 step=10");
	TwAddVarRW(m_bar, "Player 1 Colour", TW_TYPE_COLOR4F, &m_Player1Colour, "");
	TwAddVarRW(m_bar, "Player 1 Type", ATB_PlayerType, &P1Type, NULL);
//	TwAddVarRW(m_bar, "Player 1 AI Threads", TW_TYPE_INT32, &m_Game.m_P1.m_iAIThreads, "min=1 max=8 step=1");
	TwAddVarRW(m_bar, "Player 1 AI Max Time Per Move", TW_TYPE_FLOAT, &m_Game.m_P1.m_fMaxTimePerAIMove, "min=0.1 max=25.0 step=0.1");
	TwAddVarRW(m_bar, "Player 1 AI LookAheadMoves", TW_TYPE_INT32, &m_Game.m_P1.m_iAILookAheadMoves, "min=0 max=50 step=1");	//	might change this to 100 later ...
	TwAddVarRW(m_bar, "Player 2 Colour", TW_TYPE_COLOR4F, &m_Player2Colour, "");
	TwAddVarRW(m_bar, "Player 2 Type", ATB_PlayerType, &P2Type, NULL);
//	TwAddVarRW(m_bar, "Player 2 AI Threads", TW_TYPE_INT32, &m_Game.m_P2.m_iAIThreads, "min=1 max=8 step=1");
	TwAddVarRW(m_bar, "Player 2 AI Max Time Per Move", TW_TYPE_FLOAT, &m_Game.m_P2.m_fMaxTimePerAIMove, "min=0.1 max=25.0 step=0.1");
	TwAddVarRW(m_bar, "Player 2 AI LookAheadMoves", TW_TYPE_INT32, &m_Game.m_P2.m_iAILookAheadMoves, "min=0 max=50 step=1");	//	might change this to 100 later ...
	TwAddVarRW(m_bar, "Checkerboard SpecPower", TW_TYPE_FLOAT, &m_fCheckerboardSpecPower, "min=0.0 max=250.0 step=0.5");
	TwAddVarRW(m_bar, "Checker Piece SpecPower", TW_TYPE_FLOAT, &m_fCheckerPieceSpecPower, "min=0.0 max=500.0 step=0.5");
	TwAddVarRW(m_bar, "Deferred SpecPower", TW_TYPE_FLOAT, &m_fDeferredSpecPower, "min=0.0 max=250.0 step=0.5");


	m_Player1Colour = vec4(0.7f, 0.05f, 0.05f, 1.0f);
	m_Player2Colour = vec4(0.05f, 0.05f, 0.7f, 1.0f);
	m_vAmbientLightColour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_vLightColour = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	m_vLightDir = glm::normalize(vec3(-0.10f, -0.85f, 0.5f));

	//	initialise the GPU Particle emitter variables
	m_fFiringTimer = 0.0f;
	m_fFiringInterval = 0.10f;
	m_fEmitterLifespan = 3.0f;
	m_fEmitterParticleLifespan = 1.20f;
	m_uiEmitterMaxParticles = 8000;
	m_fEmitRate = 4000.0f;
	m_iNextEmitterToFire = 0;
	//	initialise the particle emitters, and it's texture
	//	Load the particle texture
	LoadAlphaTexture("./textures/particleTexture.png", m_uiParticleTexture);
	//	and pass the particle texture to all particle emitters
	for (unsigned int i = 0; i < c_iNUM_EMITTERS; ++i)
	{
		m_emitters[i].SetParticleTexture(m_uiParticleTexture);
	}


	m_fCheckerboardSpecPower = 100.0f;
	m_fCheckerPieceSpecPower = 250.0f;
	m_fDeferredSpecPower = 20.0f;
	m_CheckerBoardWorldTransform = glm::translate(vec3(0, -0.248f, 0));
	m_BackBoardWorldTransform = glm::translate(vec3(0, -0.748f, 0));
	m_CheckerWorldTransform = glm::scale(glm::translate(vec3(0.5f, 0.080f, 0.5f)), vec3(0.25f));
	m_KingWorldTransform = glm::scale(glm::translate(vec3(-0.5f, 0.17f, 0.5f)), vec3(0.25f));

	//	now load and create the meshes
	LoadMeshes();
	LoadTexture("./textures/Beveled_Checker_Board_Seamless_by_FantasyStock_2048.jpg", m_uiCheckerBoardTexture);	//	original stuffs up UV's!!
	//	Yup, stuff is probably because the width/height is not a multiple of 4
	//LoadTexture("./textures/Tileable_Checkered_Floor.jpg", m_uiCheckerBoardTexture);
	//LoadTexture("./textures/Metal_02_UV_H_CM_1.jpg", m_uiBackBoardTexture);	//	**
	//LoadTexture("./textures/smooth_metal_plate_texture_seamless_tileable_by_hhh316-d782dao.jpg", m_uiBackBoardTexture);
	LoadTexture("./textures/molten_gold_texture__tileable___2048x2048__by_fabooguy-d705j3c.jpg", m_uiBackBoardTexture);	//	***
	//LoadTexture("./textures/gold_background_texture_69796_2048x2048.jpg", m_uiBackBoardTexture);
	//LoadTexture("./textures/gold-metal-texture-tracery.jpg", m_uiBackBoardTexture);	//	*
	//	Load the particle texture
	LoadAlphaTexture("./textures/particleTexture.png", m_uiParticleTexture);

	ReloadShader();
	m_iPlayerToMoveFirst = 1;
	m_CurrentPlayerColour = m_Player1Colour;
	m_bDebug = true;

	m_Game.ResetGame(m_iPlayerToMoveFirst);
	m_Game.SetCheckersPointer(this);
	m_Game.SetPlayerCheckersPointer(this);
	switch (m_Game.m_P1.m_ePlayerType)
	{
	case PLAYER_HUMAN:
	{
		P1Type = 0;
		P1Prev = 0;
		break;
	}
	case PLAYER_MCTS_AI:
	{
		P1Type = 1;
		P1Prev = 1;
		break;
	}
	default:
	{
		P1Type = 0;
		P1Prev = 0;
		break;
	}
	}
	switch (m_Game.m_P2.m_ePlayerType)
	{
	case PLAYER_HUMAN:
	{
		P2Type = 0;
		P2Prev = 0;
		break;
	}
	case PLAYER_MCTS_AI:
	{
		P2Type = 1;
		P2Prev = 1;
		break;
	}
	default:
	{
		P2Type = 0;
		P2Prev = 0;
		break;
	}
	}

	//	now set up the deferred rendering
	BuildGBuffer();
	BuildLightBuffer();
	BuildScreenSpaceQuad();
	BuildLightCube();
	SetupDeferredLights();	//	actually add the lights
	m_bDeferredRendering = false;

	glEnable(GL_CULL_FACE);
	return true;
}

void	Checkers::shutdown()
{
	//	now clean up
	Gizmos::destroy();
	TwDeleteAllBars();
	TwTerminate();
}

bool	Checkers::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	if (m_bResetGame)
	{
		//	reset the game
		m_Game.ResetGame(m_iPlayerToMoveFirst);
		m_bPieceSelected = false;
		m_bResetGame = false;
	}
	//	check if we need to reload the shaders
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
	}
	if (m_Game.m_oGameState.m_iCurrentPlayer == 1)
	{
		m_CurrentPlayerColour = m_Player1Colour;
	}
	else
	{
		m_CurrentPlayerColour = m_Player2Colour;
	}
	float	dT = (float)glfwGetTime();
	glfwSetTime(0.0f);
	m_fFPS = (float)(1.0f / dT);
	//	now we get to the fun stuff
	m_FlyCamera.update(dT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();
	m_timer += dT;
	m_fTotalTime += dT;
	m_fFiringTimer += dT;

	//	define some basic colours
	vec4	white(1);
	vec4	black(0, 0, 0, 1);
	vec4	blue(0, 0, 1, 1);
	vec4	lightblue(0.25f, 0.25f, 1.0f, 1.0f);
	vec4	yellow(1, 1, 0, 1);
	vec4	gold(1.0f, 1.0f, 0.5f, 1.0f);
	vec4	green(0, 1, 0, 1);
	vec4	red(1, 0, 0, 1);
	//	draw the Gizmos grid
	/*
	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? white : black);
	}
	*/
	//	before getting mouse clicks, check for both player's type being changed ...
	if (P1Type != P1Prev)
	{
		P1Prev = P1Type;
		switch (P1Type)
		{
		default:
		case 0:
		{
			m_Game.m_P1.m_ePlayerType = PLAYER_HUMAN;
			break;
		}
		case 1:
		{
			m_Game.m_P1.m_ePlayerType = PLAYER_MCTS_AI;
			break;
		}
		}
	}
	if (P2Type != P2Prev)
	{
		P2Prev = P2Type;
		switch (P2Type)
		{
		default:
		case 0:
		{
			m_Game.m_P2.m_ePlayerType = PLAYER_HUMAN;
			break;
		}
		case 1:
		{
			m_Game.m_P2.m_ePlayerType = PLAYER_MCTS_AI;
			break;
		}
		}
	}


	double	dXDelta, dYDelta;
	glfwGetCursorPos(m_window, &dXDelta, &dYDelta);
	vec3	vPlanePos = m_FlyCamera.PickAgainstPlane(dXDelta, dYDelta, vec4(0, 1, 0, 0));
//	if (m_timer > 1.0f)
//	{
//		cout << "PlanePos (X/Y/Z): " << vPlanePos.x << " / " << vPlanePos.y << " / " << vPlanePos.z << '\n';
//		m_timer = 0.0f;
//	}
	int	iXMouse, iZMouse;
	if ((vPlanePos.x < -4.0) || (vPlanePos.x > 4.0))
	{
		iXMouse = -1;	//	mouse is outside the board
	}
	else
	{
		//	calculate the x coordinate on the board
		iXMouse = (int)(vPlanePos.x + 4);
	}
	if ((vPlanePos.z < -4.0) || (vPlanePos.z > 4.0))
	{
		iZMouse = -1;	//	mouse is outside the board
	}
	else
	{
		//	calculate the z coordinate on the board
		iZMouse = (int)(vPlanePos.z + 4);
	}
	float	fBoxHeight = 0.35f;	//	Y-coordinate of any squares to draw
	if ((iXMouse != -1) && (iZMouse != -1))
	{
		//	the mouse is on the board so draw a box on the relevant square
		//	first we need to "fix" the x position so only on the black squares are selected
		iXMouse = (2 * (iXMouse / 2)) + (iZMouse + 1) % 2;
		Bitboard	bbSelectedSquare = GenerateBitMaskFromCoords(iXMouse, iZMouse);
		//cout << "Selected move Bitboard: " << bbValidMove << '\n';
		if ((bbSelectedSquare > 0) && ((bbSelectedSquare & m_Game.m_oGameState.m_P1Pieces) || (bbSelectedSquare & m_Game.m_oGameState.m_P2Pieces)))
		{
			//	then we have clicked on a valid piece
			if ((iXMouse != m_iXSelected) || (iZMouse != m_iZSelected))
			{
				//	if this square is not already selected then draw it in blue
				DrawSelectedBox(iXMouse, iZMouse, 1.0f, fBoxHeight, blue);
			}
			if (glfwGetMouseButton(m_window, 0))
			{
				//	user has clicked on the board, so select the appropriate piece
				//	will also need to do checks for correct player here as well
				//m_iXSelected = iXMouse;
				//m_iZSelected = iZMouse;
				m_Game.MouseClickedOnBoardAt(iXMouse, iZMouse);
				//	also create a bitboard for the selected piece here <<<<------************
				m_Game.GetSelectedMoveDetails(m_bPieceSelected, m_iXSelected, m_iZSelected);
				//m_bPieceSelected = true;	//	as soon as a move is selected set this back to false
				if (m_fFiringTimer > m_fFiringInterval)
				{
					//	just as a debug, fire off the next particle emitter ...
					if (m_bPieceSelected)
					{
						//FireEmitterAt(m_iXSelected, m_iZSelected, fBoxHeight);	//	function resets the firing timer
					}
				}
			}
			else
			{
				//	no mouse click, so feed through error coords
				m_Game.MouseClickedOnBoardAt(-1, -1);
			}
		}
		else
		{
			//	we are still on the board, so check for a closing mouse click on an empty square
			if (glfwGetMouseButton(m_window, 0))
			{
				m_Game.MouseClickedOnBoardAt(iXMouse, iZMouse);
			}
		}
	}

	m_Game.update(dT);
	m_Game.GetSelectedMoveDetails(m_bPieceSelected, m_iXSelected, m_iZSelected);
	
	//	debug draw stuff ...
	//m_bDebug = true;
	if (m_bDebug)
	{
		bool	bJumpers = false;
		bbDAvailableMovers = GetCurrentAvailableMovers(m_Game.m_oGameState, bJumpers);
		//	and draw them ...
		DrawBitboardAsBoxes(bbDAvailableMovers, 0.9f, gold);
		DebugDrawMoveList(0.1f, gold);
	}


	if (m_bPieceSelected)
	{
		DrawSelectedBox(m_iXSelected, m_iZSelected, 1.0f, fBoxHeight, yellow);
	}


	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	return true;
}

void	Checkers::DrawBitboardAsBoxes(Bitboard a_bbBoard, float a_fBoxSize, vec4 a_vColour)
{
	for (unsigned int i = 0; i < 32; ++i)
	{
		if ((a_bbBoard & abbSquareMasks[i]) > 0)
		{
			//	then we have a piece here so draw a box for it
			DrawSelectedBox(GetBoardXCoordFromIndex(i), GetBoardYCoordFromIndex(i), a_fBoxSize, 0.15f, a_vColour);
		}
	}
}

void	Checkers::DebugDrawMoveList(float a_fHeight, vec4 a_vColour)
{
	//	simply draws a line on the board, at the height and colour passed in, for every move in the current move list vector
	float	fStartX = -3.5f;
	float	fStartZ = -3.5f;
	int	iStartX;
	int	iStartZ;
	int	iEndX;
	int	iEndZ;
	for (int i = 0; i < m_Game.m_aMoveList.size(); ++i)
	{
		iStartX = GetBoardXCoord(m_Game.m_aMoveList[i].StartPos);
		iStartZ = GetBoardYCoord(m_Game.m_aMoveList[i].StartPos);
		iEndX = GetBoardXCoord(m_Game.m_aMoveList[i].EndPos);
		iEndZ = GetBoardYCoord(m_Game.m_aMoveList[i].EndPos);
		Gizmos::addLine(vec3(fStartX + iStartX, a_fHeight, fStartZ + iStartZ), vec3(fStartX + iEndX, a_fHeight, fStartZ + iEndZ), a_vColour);
	}
}

void	Checkers::draw()
{
	Application::draw();

	if (m_bDeferredRendering)
	{
		DrawModelsDeferred();
		AccumulateLightsDeferred();	//	<<---- This is where the spec calculations need to be done - modify shaders to pass in the G-Buffer to the light accumulation shader
		RenderCompositePass();
	}
	else
	{

		DrawModels(m_vLightDir, m_vLightColour);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		glDepthFunc(GL_LEQUAL);
		
		DrawModels(glm::normalize(vec3(1, -6, 1)), m_vLightColour);
		DrawModels(glm::normalize(vec3(1, -6, -1)), m_vLightColour);
		DrawModels(glm::normalize(vec3(-1, -6, -1)), m_vLightColour);
		DrawModels(glm::normalize(vec3(-1, -6, 1)), m_vLightColour);
		glDepthFunc(GL_LESS);
		glDisable(GL_BLEND);
	}

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

void	Checkers::FireEmitterAt(int a_iXIndex, int a_iZindex, float a_fHeight)
{
	//	fires a smaller emitter with fewer particles, and shorter emitter and particle lifespan, when a piece is clicked on
	vec3	vEmitterPosition = vec3((float)a_iXIndex - 3.5f, a_fHeight, (float)a_iZindex - 3.5f);
	vec4	lightblue(0.25f, 0.25f, 1.0f, 1.0f);
	vec4	gold(1.0f, 1.0f, 0.5f, 1.0f);
	//cout << "Firing Emitter " << m_iNextEmitterToFire << " at location " << vEmitterPosition.x << "/" << vEmitterPosition.y << "/" << vEmitterPosition.z << '\n';
	bool	bHumanPlayer = false;
	int	iCurrentPlayer = m_Game.m_oGameState.m_iCurrentPlayer;
	if (iCurrentPlayer == 1)
	{
		if (m_Game.m_P1.m_ePlayerType == PLAYER_HUMAN)
		{
			bHumanPlayer == true;
		}
	}
	else
	{
		if (m_Game.m_P2.m_ePlayerType == PLAYER_HUMAN)
		{
			bHumanPlayer == true;
		}
	}
	if ((m_fFiringTimer > m_fFiringInterval) && bHumanPlayer)
	{
		m_emitters[m_iNextEmitterToFire].Init(m_uiEmitterMaxParticles * 0.5f, vEmitterPosition, vec3(0.0f, 0.04f, 0.0f), m_fEmitRate * 0.5f,
			m_fEmitterLifespan * 0.75f, 0.1f * m_fEmitterParticleLifespan, 0.75f * m_fEmitterParticleLifespan, 0.15f, 0.35f,
			0.5f, 0.05f, 0.1f, lightblue, gold, m_iNextEmitterToFire);
		m_iNextEmitterToFire = (m_iNextEmitterToFire + 1) % c_iNUM_EMITTERS;
		m_fFiringTimer = 0.0f;
	}
	else
	{
		//	AI player so just fire the emitter ignoring timing!
		m_emitters[m_iNextEmitterToFire].Init(m_uiEmitterMaxParticles * 0.5f, vEmitterPosition, vec3(0.0f, 0.04f, 0.0f), m_fEmitRate * 0.5f,
			m_fEmitterLifespan * 0.75f, 0.1f * m_fEmitterParticleLifespan, 0.75f * m_fEmitterParticleLifespan, 0.15f, 0.35f,
			0.5f, 0.05f, 0.1f, lightblue, gold, m_iNextEmitterToFire);
		m_iNextEmitterToFire = (m_iNextEmitterToFire + 1) % c_iNUM_EMITTERS;
		m_fFiringTimer = 0.0f;
	}
}

void	Checkers::FireCaptureEmitterAt(int a_iXIndex, int a_iZindex, float a_fHeight)
{
	//	fires a larger emitter with the full particle count, when a piece is captured
	//	typically called by the player in the check for if the move made is a capture move
	vec3	vEmitterPosition = vec3((float)a_iXIndex - 3.5f, a_fHeight, (float)a_iZindex - 3.5f);
	vec4	lightblue(0.25f, 0.25f, 1.0f, 1.0f);
	vec4	gold(1.0f, 1.0f, 0.5f, 1.0f);
	//cout << "Firing Emitter " << m_iNextEmitterToFire << " at location " << vEmitterPosition.x << "/" << vEmitterPosition.y << "/" << vEmitterPosition.z << '\n';
	m_emitters[m_iNextEmitterToFire].Init(m_uiEmitterMaxParticles, vEmitterPosition, vec3(0.0f, 0.10f, 0.0f), m_fEmitRate,
		m_fEmitterLifespan, 0.1f * m_fEmitterParticleLifespan, m_fEmitterParticleLifespan, 0.4f, 0.7f,
		0.5f, 0.05f, 0.1f, lightblue, gold, m_iNextEmitterToFire);
	m_iNextEmitterToFire = (m_iNextEmitterToFire + 1) % c_iNUM_EMITTERS;
	m_fFiringTimer = 0.0f;
}

void	Checkers::FireGameOverEmitterAt(int a_iXIndex, int a_iZindex, float a_fHeight, int a_iWinner)
{
	//	fires a larger emitter with the full particle count, when a piece is captured
	//	typically called by the Game when a Game Over condition is met
	//	a_iWinner will either be 0 (for no winner), or 1 or 2 for the winning player.  This will determine the colour used.
	vec3	vEmitterPosition = vec3((float)a_iXIndex - 3.5f, a_fHeight, (float)a_iZindex - 3.5f);
	vec4	gold(1.0f, 1.0f, 0.5f, 1.0f);
	vec4	vStartColour(0.7f, 0.7f, 0.35f, 1.0f);
	vec4	vEndColour;
	switch (a_iWinner)
	{
	case 1:
	{
		vEndColour = m_Player1Colour;
		vStartColour = vEndColour * 5.0f;
		break;
	}
	case 2:
	{
		vEndColour = m_Player2Colour;
		vStartColour = vEndColour * 5.0f;
		break;
	}
	default:
	{
		vEndColour = gold;
		vStartColour = vEndColour * 2.0f;
		break;
	}
	}
	//cout << "Firing Emitter " << m_iNextEmitterToFire << " at location " << vEmitterPosition.x << "/" << vEmitterPosition.y << "/" << vEmitterPosition.z << '\n';
	m_emitters[m_iNextEmitterToFire].Init(m_uiEmitterMaxParticles, vEmitterPosition, vec3(0.0f, 0.5f, 0.0f), m_fEmitRate,
		4.0f * m_fEmitterLifespan, 0.3f * m_fEmitterParticleLifespan, m_fEmitterParticleLifespan, 0.75f, 1.0f,
		0.5f, 0.04f, 0.1f, vStartColour, vEndColour, m_iNextEmitterToFire);
	m_iNextEmitterToFire = (m_iNextEmitterToFire + 1) % c_iNUM_EMITTERS;
}

void	Checkers::BuildBasicCube(OpenGLData& a_cube, float a_fSideLength)
{
	//	Takes in a reference to OpenGLData so that different objects can be passed in.
	//	Also takes in a side length so that a custom sized cube can be made.

	//	This will build a cube scaled to the side length passed in.
	//	This cube is not textured, and is mainly intended for simple purposes like drawing a wireframe or flat coloured cube.

	float	fScale = a_fSideLength * 0.5f;

	float	vertexData[]
	{
		//	X		Y		Z		W		Index
		-fScale, -fScale, fScale,	1,	//	0
		fScale, -fScale, fScale,	1,	//	1
		fScale, -fScale, -fScale,	1,	//	2
		-fScale, -fScale, -fScale,	1,	//	3

		-fScale, fScale, fScale,	1,	//	4
		fScale, fScale, fScale,		1,	//	5
		fScale, fScale, -fScale,	1,	//	6
		-fScale, fScale, -fScale,	1,	//	7
	};

	unsigned int	indexData[]
	{
		0, 5, 4,
		0, 1, 5,
		1, 6, 5,
		1, 2, 6,
		2, 7, 6,
		2, 3, 7,

		3, 4, 7,
		3, 0, 4,
		4, 6, 7,
		4, 5, 6,
		3, 1, 0,
		3, 2, 1,
	};

	a_cube.m_uiIndexCount = 36;

	glGenVertexArrays(1, &a_cube.m_uiVAO);
	glBindVertexArray(a_cube.m_uiVAO);

	glGenBuffers(1, &a_cube.m_uiVBO);
	glGenBuffers(1, &a_cube.m_uiIBO);


	glBindBuffer(GL_ARRAY_BUFFER, a_cube.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, a_cube.m_uiIBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	Checkers::BuildCheckerboard(OpenGLData& a_board, float a_fSideLength, float a_fThickness)
{
	//	Takes in a reference to OpenGLData so that different objects can be passed in.
	//	Also takes in a side length so that a custom sized cube/board can be made.

	//	This will build a checkerboard scaled to the side length and thickness passed in.
	//	Since this is intended to be textured and lit, then 24 vertices are needed instead of 6, so that the normals will be correct for each face.

	//	UV's are also required for the textures ...
	//	For the checkerboard the sides will simply use the edge of the texture, which will still look ok for this purpose.

	float	fScale = a_fSideLength * 0.5f;
	float	fHeight = a_fThickness * 0.5f;
	float	fZero = 0.001f;
	float	fOne = 0.999f;
	
	VertexNormalTexCoord	vertexData[24];
	//	3 versions of each vertex, with different normals for each face
	//	Vertex 0
	vertexData[0].position = vec4(-fScale, -fHeight, fScale, 1.0f);
	vertexData[1].position = vec4(-fScale, -fHeight, fScale, 1.0f);
	vertexData[2].position = vec4(-fScale, -fHeight, fScale, 1.0f);
	vertexData[0].normal = vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertexData[1].normal = vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertexData[2].normal = vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertexData[0].tex_coord = vec2(fZero, fOne);
	vertexData[1].tex_coord = vec2(fZero, fOne);
	vertexData[2].tex_coord = vec2(fZero, fOne);
	//	Vertex 1
	vertexData[3].position = vec4(fScale, -fHeight, fScale, 1.0f);
	vertexData[4].position = vec4(fScale, -fHeight, fScale, 1.0f);
	vertexData[5].position = vec4(fScale, -fHeight, fScale, 1.0f);
	vertexData[3].normal = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vertexData[4].normal = vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertexData[5].normal = vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertexData[3].tex_coord = vec2(fOne, fOne);
	vertexData[4].tex_coord = vec2(fOne, fOne);
	vertexData[5].tex_coord = vec2(fOne, fOne);
	//	Vertex 2
	vertexData[6].position = vec4(fScale, -fHeight, -fScale, 1.0f);
	vertexData[7].position = vec4(fScale, -fHeight, -fScale, 1.0f);
	vertexData[8].position = vec4(fScale, -fHeight, -fScale, 1.0f);
	vertexData[6].normal = vec4(1.0f, .0f, 0.0f, 0.0f);
	vertexData[7].normal = vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertexData[8].normal = vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertexData[6].tex_coord = vec2(fOne, fZero);
	vertexData[7].tex_coord = vec2(fOne, fZero);
	vertexData[8].tex_coord = vec2(fOne, fZero);
	//	Vertex 3
	vertexData[9].position = vec4(-fScale, -fHeight, -fScale, 1.0f);
	vertexData[10].position = vec4(-fScale, -fHeight, -fScale, 1.0f);
	vertexData[11].position = vec4(-fScale, -fHeight, -fScale, 1.0f);
	vertexData[9].normal = vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertexData[10].normal = vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertexData[11].normal = vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertexData[9].tex_coord = vec2(fZero, fZero);
	vertexData[10].tex_coord = vec2(fZero, fZero);
	vertexData[11].tex_coord = vec2(fZero, fZero);
	//	Vertex 4
	vertexData[12].position = vec4(-fScale, fHeight, fScale, 1.0f);
	vertexData[13].position = vec4(-fScale, fHeight, fScale, 1.0f);
	vertexData[14].position = vec4(-fScale, fHeight, fScale, 1.0f);
	vertexData[12].normal = vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertexData[13].normal = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertexData[14].normal = vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertexData[12].tex_coord = vec2(fZero, fOne);
	vertexData[13].tex_coord = vec2(fZero, fOne);
	vertexData[14].tex_coord = vec2(fZero, fOne);
	//	Vertex 5
	vertexData[15].position = vec4(fScale, fHeight, fScale, 1.0f);
	vertexData[16].position = vec4(fScale, fHeight, fScale, 1.0f);
	vertexData[17].position = vec4(fScale, fHeight, fScale, 1.0f);
	vertexData[15].normal = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vertexData[16].normal = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertexData[17].normal = vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertexData[15].tex_coord = vec2(fOne, fOne);
	vertexData[16].tex_coord = vec2(fOne, fOne);
	vertexData[17].tex_coord = vec2(fOne, fOne);
	//	Vertex 6
	vertexData[18].position = vec4(fScale, fHeight, -fScale, 1.0f);
	vertexData[19].position = vec4(fScale, fHeight, -fScale, 1.0f);
	vertexData[20].position = vec4(fScale, fHeight, -fScale, 1.0f);
	vertexData[18].normal = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vertexData[19].normal = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertexData[20].normal = vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertexData[18].tex_coord = vec2(fOne, fZero);
	vertexData[19].tex_coord = vec2(fOne, fZero);
	vertexData[20].tex_coord = vec2(fOne, fZero);
	//	Vertex 7
	vertexData[21].position = vec4(-fScale, fHeight, -fScale, 1.0f);
	vertexData[22].position = vec4(-fScale, fHeight, -fScale, 1.0f);
	vertexData[23].position = vec4(-fScale, fHeight, -fScale, 1.0f);
	vertexData[21].normal = vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertexData[22].normal = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertexData[23].normal = vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertexData[21].tex_coord = vec2(fZero, fZero);
	vertexData[22].tex_coord = vec2(fZero, fZero);
	vertexData[23].tex_coord = vec2(fZero, fZero);

	/*
		//	X		Y		Z		W		Index
		-fScale, -fScale, fScale, 1,	//	0
		fScale, -fScale, fScale, 1,		//	1
		fScale, -fScale, -fScale, 1,	//	2
		-fScale, -fScale, -fScale, 1,	//	3

		-fScale, fScale, fScale, 1,		//	4
		fScale, fScale, fScale, 1,		//	5
		fScale, fScale, -fScale, 1,		//	6
		-fScale, fScale, -fScale, 1,	//	7
	*/

	unsigned int	indexData[]
	{
		2, 17, 14,
		2, 5, 17,
		3, 6, 18,
		3, 18, 15,
		8, 23, 20,
		8, 11, 23,

		9, 12, 21,
		9, 0, 12,
		1, 10, 7,
		1, 7, 4,
		13, 19, 22,
		13, 16, 19,	//
	};

	a_board.m_uiIndexCount = 36;

	glGenVertexArrays(1, &a_board.m_uiVAO);
	glBindVertexArray(a_board.m_uiVAO);

	glGenBuffers(1, &a_board.m_uiVBO);
	glGenBuffers(1, &a_board.m_uiIBO);


	glBindBuffer(GL_ARRAY_BUFFER, a_board.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, a_board.m_uiIBO);

	glBufferData(GL_ARRAY_BUFFER, (sizeof(VertexNormalTexCoord)) * 24, vertexData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(unsigned int)) * 36, indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position
	glEnableVertexAttribArray(1);	//	normal
	glEnableVertexAttribArray(2);	//	texture coordinate

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTexCoord), 0);	//	position
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTexCoord), (void*)(sizeof(vec4)));	//	normal
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTexCoord), (void*)(sizeof(vec4) * 2));	//	texture coordinate

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	Checkers::BuildBackboard(OpenGLData& a_board, float a_fSideLength, float a_fThickness)
{
	//	Takes in a reference to OpenGLData so that different objects can be passed in.
	//	Also takes in a side length so that a custom sized cube/board can be made.

	//	This will build a backboard scaled to the side length and thickness passed in.
	//	Since this is intended to be textured and lit, then 24 vertices are needed instead of 6, so that the normals will be correct for each face.

	//	UV's are also required for the textures ...
	//	For the checkerboard the sides will simply use the edge of the texture, which will still look ok for this purpose.
	//	For the backboard this will eventually be different, using some of the edge of the texture for the edge of the board instead of the face.

	float	fScale = a_fSideLength * 0.5f;
	float	fHeight = a_fThickness * 0.5f;
	float	fZero = 0.001f;
	float	fOne = 0.999f;

	VertexNormalTexCoord	vertexData[24];
	//	3 versions of each vertex, with different normals for each face
	//	Vertex 0
	vertexData[0].position = vec4(-fScale, -fHeight, fScale, 1.0f);
	vertexData[1].position = vec4(-fScale, -fHeight, fScale, 1.0f);
	vertexData[2].position = vec4(-fScale, -fHeight, fScale, 1.0f);
	vertexData[0].normal = vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertexData[1].normal = vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertexData[2].normal = vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertexData[0].tex_coord = vec2(fZero, fOne);
	vertexData[1].tex_coord = vec2(fZero, fOne);
	vertexData[2].tex_coord = vec2(fZero, fOne);
	//	Vertex 1
	vertexData[3].position = vec4(fScale, -fHeight, fScale, 1.0f);
	vertexData[4].position = vec4(fScale, -fHeight, fScale, 1.0f);
	vertexData[5].position = vec4(fScale, -fHeight, fScale, 1.0f);
	vertexData[3].normal = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vertexData[4].normal = vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertexData[5].normal = vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertexData[3].tex_coord = vec2(fOne, fOne);
	vertexData[4].tex_coord = vec2(fOne, fOne);
	vertexData[5].tex_coord = vec2(fOne, fOne);
	//	Vertex 2
	vertexData[6].position = vec4(fScale, -fHeight, -fScale, 1.0f);
	vertexData[7].position = vec4(fScale, -fHeight, -fScale, 1.0f);
	vertexData[8].position = vec4(fScale, -fHeight, -fScale, 1.0f);
	vertexData[6].normal = vec4(1.0f, .0f, 0.0f, 0.0f);
	vertexData[7].normal = vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertexData[8].normal = vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertexData[6].tex_coord = vec2(fOne, fZero);
	vertexData[7].tex_coord = vec2(fOne, fZero);
	vertexData[8].tex_coord = vec2(fOne, fZero);
	//	Vertex 3
	vertexData[9].position = vec4(-fScale, -fHeight, -fScale, 1.0f);
	vertexData[10].position = vec4(-fScale, -fHeight, -fScale, 1.0f);
	vertexData[11].position = vec4(-fScale, -fHeight, -fScale, 1.0f);
	vertexData[9].normal = vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertexData[10].normal = vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertexData[11].normal = vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertexData[9].tex_coord = vec2(fZero, fZero);
	vertexData[10].tex_coord = vec2(fZero, fZero);
	vertexData[11].tex_coord = vec2(fZero, fZero);
	//	Vertex 4
	vertexData[12].position = vec4(-fScale, fHeight, fScale, 1.0f);
	vertexData[13].position = vec4(-fScale, fHeight, fScale, 1.0f);
	vertexData[14].position = vec4(-fScale, fHeight, fScale, 1.0f);
	vertexData[12].normal = vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertexData[13].normal = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertexData[14].normal = vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertexData[12].tex_coord = vec2(fZero, fOne);
	vertexData[13].tex_coord = vec2(fZero, fOne);
	vertexData[14].tex_coord = vec2(fZero, fOne);
	//	Vertex 5
	vertexData[15].position = vec4(fScale, fHeight, fScale, 1.0f);
	vertexData[16].position = vec4(fScale, fHeight, fScale, 1.0f);
	vertexData[17].position = vec4(fScale, fHeight, fScale, 1.0f);
	vertexData[15].normal = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vertexData[16].normal = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertexData[17].normal = vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertexData[15].tex_coord = vec2(fOne, fOne);
	vertexData[16].tex_coord = vec2(fOne, fOne);
	vertexData[17].tex_coord = vec2(fOne, fOne);
	//	Vertex 6
	vertexData[18].position = vec4(fScale, fHeight, -fScale, 1.0f);
	vertexData[19].position = vec4(fScale, fHeight, -fScale, 1.0f);
	vertexData[20].position = vec4(fScale, fHeight, -fScale, 1.0f);
	vertexData[18].normal = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vertexData[19].normal = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertexData[20].normal = vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertexData[18].tex_coord = vec2(fOne, fZero);
	vertexData[19].tex_coord = vec2(fOne, fZero);
	vertexData[20].tex_coord = vec2(fOne, fZero);
	//	Vertex 7
	vertexData[21].position = vec4(-fScale, fHeight, -fScale, 1.0f);
	vertexData[22].position = vec4(-fScale, fHeight, -fScale, 1.0f);
	vertexData[23].position = vec4(-fScale, fHeight, -fScale, 1.0f);
	vertexData[21].normal = vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertexData[22].normal = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertexData[23].normal = vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertexData[21].tex_coord = vec2(fZero, fZero);
	vertexData[22].tex_coord = vec2(fZero, fZero);
	vertexData[23].tex_coord = vec2(fZero, fZero);

	unsigned int	indexData[]
	{
		2, 17, 14,
		2, 5, 17,
		3, 6, 18,
		3, 18, 15,
		8, 23, 20,
		8, 11, 23,

		9, 12, 21,
		9, 0, 12,
		1, 10, 7,
		1, 7, 4,
		13, 19, 22,
		13, 16, 19,
	};

	a_board.m_uiIndexCount = 36;

	glGenVertexArrays(1, &a_board.m_uiVAO);
	glBindVertexArray(a_board.m_uiVAO);

	glGenBuffers(1, &a_board.m_uiVBO);
	glGenBuffers(1, &a_board.m_uiIBO);


	glBindBuffer(GL_ARRAY_BUFFER, a_board.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, a_board.m_uiIBO);

	glBufferData(GL_ARRAY_BUFFER, (sizeof(VertexNormalTexCoord)) * 24, vertexData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(unsigned int)) * 36, indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position
	glEnableVertexAttribArray(1);	//	normal
	glEnableVertexAttribArray(2);	//	texture coordinate

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTexCoord), 0);	//	position
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTexCoord), (void*)(sizeof(vec4)));	//	normal
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTexCoord), (void*)(sizeof(vec4) * 2));	//	texture coordinate

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	Checkers::LoadMeshes()
{
	m_CheckerMesh = LoadOBJ("./models/Checker.obj");
	m_KingMesh = LoadOBJ("./models/Checker_King.obj");

	//	now generate the board and backboard meshes
	BuildCheckerboard(m_BoardMesh, 8.0f, 0.5f);
	BuildBackboard(m_BackBoardMesh, 10.0f, 0.5f);

}

void	Checkers::DrawModels(vec3 a_vLightDir, vec4 a_vLightColour)
{
	//	draw the checkerboard
	glUseProgram(m_uiProgramID);
	int	iViewProjUniform = glGetUniformLocation(m_uiProgramID, "projection_view");
	glUniformMatrix4fv(iViewProjUniform, 1, GL_FALSE, (float*)&m_FlyCamera.m_projectionViewTransform);
	int	iWorldUniform = glGetUniformLocation(m_uiProgramID, "worldTransform");
	glUniformMatrix4fv(iWorldUniform, 1, GL_FALSE, (float*)&m_CheckerBoardWorldTransform);
	int iTexUniform = glGetUniformLocation(m_uiProgramID, "albedoTexture");
	glUniform1i(iTexUniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiCheckerBoardTexture);
	int	eye_pos_uniform = glGetUniformLocation(m_uiProgramID, "eye_pos");
	glUniform3fv(eye_pos_uniform, 1, (float*)&m_FlyCamera.GetPosition());
	int	iLightDirUniform = glGetUniformLocation(m_uiProgramID, "light_dir");
	glUniform3fv(iLightDirUniform, 1, (float*)&a_vLightDir);
	int	iLightColourUniform = glGetUniformLocation(m_uiProgramID, "light_colour");
	glUniform3fv(iLightColourUniform, 1, (float*)&a_vLightColour.rgb);
	int	iAmbientUniform = glGetUniformLocation(m_uiProgramID, "ambient_light");
	glUniform3fv(iAmbientUniform, 1, (float*)&m_vAmbientLightColour.rgb);
	int	iSpecPowerUniform = glGetUniformLocation(m_uiProgramID, "specular_power");
	glUniform1f(iSpecPowerUniform, m_fCheckerboardSpecPower);

	glBindVertexArray(m_BoardMesh.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_BoardMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);

	//	now draw the Backboard
	//	uses the same shader program, so no need to load change here yet
	iWorldUniform = glGetUniformLocation(m_uiProgramID, "worldTransform");
	glUniformMatrix4fv(iWorldUniform, 1, GL_FALSE, (float*)&m_BackBoardWorldTransform);
	iTexUniform = glGetUniformLocation(m_uiProgramID, "albedoTexture");
	glUniform1i(iTexUniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiBackBoardTexture);

	glBindVertexArray(m_BackBoardMesh.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_BackBoardMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);


	//	now draw the checkers
	//	these use their own version of the lighting shader that takes in a uniform for the piece colour
	glUseProgram(m_uiModelProgramID);
	iViewProjUniform = glGetUniformLocation(m_uiModelProgramID, "projection_view");
	glUniformMatrix4fv(iViewProjUniform, 1, GL_FALSE, (float*)&m_FlyCamera.m_projectionViewTransform);
	eye_pos_uniform = glGetUniformLocation(m_uiModelProgramID, "eye_pos");
	glUniform3fv(eye_pos_uniform, 1, (float*)&m_FlyCamera.GetPosition());
	iLightDirUniform = glGetUniformLocation(m_uiModelProgramID, "light_dir");
	glUniform3fv(iLightDirUniform, 1, (float*)&a_vLightDir);
	iLightColourUniform = glGetUniformLocation(m_uiModelProgramID, "light_colour");
	glUniform3fv(iLightColourUniform, 1, (float*)&a_vLightColour.rgb);
	iAmbientUniform = glGetUniformLocation(m_uiModelProgramID, "ambient_light");
	glUniform3fv(iAmbientUniform, 1, (float*)&m_vAmbientLightColour.rgb);
	iSpecPowerUniform = glGetUniformLocation(m_uiModelProgramID, "specular_power");
	glUniform1f(iSpecPowerUniform, m_fCheckerPieceSpecPower);

	vec3	vPieceColour = m_Player1Colour.rgb;	//	this changes for each player ...
	int	iCheckerColourUniform = glGetUniformLocation(m_uiModelProgramID, "material_colour");
	glUniform3fv(iCheckerColourUniform, 1, (float*)&vPieceColour.rgb);

	//	get a copy of the bitboards
	Bitboard	bbP1Pieces = m_Game.m_oGameState.m_P1Pieces;
	Bitboard	bbP2Pieces = m_Game.m_oGameState.m_P2Pieces;
	Bitboard	bbKings = m_Game.m_oGameState.m_Kings;
	for (int i = 0; i < 32; ++i)
	{
		if ((bbP1Pieces & bbKings & (0x00000001 << i)) > 0)
		{
			//	there is a P1 King in this position
			//	this needs to be different for each checker
			m_CheckerWorldTransform = glm::scale(glm::translate(aKingCoords[i]), vec3(0.25f, 0.25f, 0.25f));
			iWorldUniform = glGetUniformLocation(m_uiModelProgramID, "worldTransform");
			glUniformMatrix4fv(iWorldUniform, 1, GL_FALSE, (float*)&m_CheckerWorldTransform);
			glBindVertexArray(m_KingMesh.m_uiVAO);
			glDrawElements(GL_TRIANGLES, m_KingMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);
		}
		else if ((bbP1Pieces & (0x00000001 << i)) > 0)
		{
			//	There is a regular piece in this position
			//	this needs to be different for each checker
			m_CheckerWorldTransform = glm::scale(glm::translate(aPieceCoords[i]), vec3(0.25f, 0.25f, 0.25f));
			iWorldUniform = glGetUniformLocation(m_uiModelProgramID, "worldTransform");
			glUniformMatrix4fv(iWorldUniform, 1, GL_FALSE, (float*)&m_CheckerWorldTransform);
			glBindVertexArray(m_CheckerMesh.m_uiVAO);
			glDrawElements(GL_TRIANGLES, m_CheckerMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);
		}
	}
	
	vPieceColour = m_Player2Colour.rgb;	//	this changes for each player ...
	iCheckerColourUniform = glGetUniformLocation(m_uiModelProgramID, "material_colour");
	glUniform3fv(iCheckerColourUniform, 1, (float*)&vPieceColour.rgb);

	for (int i = 0; i < 32; ++i)
	{
		if ((bbP2Pieces & bbKings & (0x00000001 << i)) > 0)
		{
			//	there is a P2 King in this position
			//	this needs to be different for each checker
			m_CheckerWorldTransform = glm::scale(glm::translate(aKingCoords[i]), vec3(0.25f, 0.25f, 0.25f));
			iWorldUniform = glGetUniformLocation(m_uiModelProgramID, "worldTransform");
			glUniformMatrix4fv(iWorldUniform, 1, GL_FALSE, (float*)&m_CheckerWorldTransform);
			glBindVertexArray(m_KingMesh.m_uiVAO);
			glDrawElements(GL_TRIANGLES, m_KingMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);
		}
		else if ((bbP2Pieces & (0x00000001 << i)) > 0)
		{
			//	There is a regular piece in this position
			//	this needs to be different for each checker
			m_CheckerWorldTransform = glm::scale(glm::translate(aPieceCoords[i]), vec3(0.25f, 0.25f, 0.25f));
			iWorldUniform = glGetUniformLocation(m_uiModelProgramID, "worldTransform");
			glUniformMatrix4fv(iWorldUniform, 1, GL_FALSE, (float*)&m_CheckerWorldTransform);
			glBindVertexArray(m_CheckerMesh.m_uiVAO);
			glDrawElements(GL_TRIANGLES, m_CheckerMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);
		}
	}
}

void	Checkers::DrawSelectedBox(int a_iXIndex, int a_iZIndex, float a_fSideLength, float a_fYCoord, vec4 a_Colour)
{
	//	draws a box around the passed in checkerboard square, of the side length passed in at the Y coordinate height and colour passed in
	float	fYBottom = 0.01f;
	float	fLengthOffset = 0.5f * (1.0f - a_fSideLength);
	//	draw the bottom
	Gizmos::addLine(vec3(-4.0f + a_iXIndex + fLengthOffset, fYBottom, -4.0f + a_iZIndex + fLengthOffset), vec3(-3.0f + a_iXIndex - fLengthOffset, fYBottom, -4.0f + a_iZIndex + fLengthOffset), a_Colour);
	Gizmos::addLine(vec3(-3.0f + a_iXIndex - fLengthOffset, fYBottom, -4.0f + a_iZIndex + fLengthOffset), vec3(-3.0f + a_iXIndex - fLengthOffset, fYBottom, -3.0f + a_iZIndex - fLengthOffset), a_Colour);
	Gizmos::addLine(vec3(-3.0f + a_iXIndex - fLengthOffset, fYBottom, -3.0f + a_iZIndex - fLengthOffset), vec3(-4.0f + a_iXIndex + fLengthOffset, fYBottom, -3.0f + a_iZIndex - fLengthOffset), a_Colour);
	Gizmos::addLine(vec3(-4.0f + a_iXIndex + fLengthOffset, fYBottom, -3.0f + a_iZIndex - fLengthOffset), vec3(-4.0f + a_iXIndex + fLengthOffset, fYBottom, -4.0f + a_iZIndex + fLengthOffset), a_Colour);
	//	draw the top
	Gizmos::addLine(vec3(-4.0f + a_iXIndex + fLengthOffset, a_fYCoord, -4.0f + a_iZIndex + fLengthOffset), vec3(-3.0f + a_iXIndex - fLengthOffset, a_fYCoord, -4.0f + a_iZIndex + fLengthOffset), a_Colour);
	Gizmos::addLine(vec3(-3.0f + a_iXIndex - fLengthOffset, a_fYCoord, -4.0f + a_iZIndex + fLengthOffset), vec3(-3.0f + a_iXIndex - fLengthOffset, a_fYCoord, -3.0f + a_iZIndex - fLengthOffset), a_Colour);
	Gizmos::addLine(vec3(-3.0f + a_iXIndex - fLengthOffset, a_fYCoord, -3.0f + a_iZIndex - fLengthOffset), vec3(-4.0f + a_iXIndex + fLengthOffset, a_fYCoord, -3.0f + a_iZIndex - fLengthOffset), a_Colour);
	Gizmos::addLine(vec3(-4.0f + a_iXIndex + fLengthOffset, a_fYCoord, -3.0f + a_iZIndex - fLengthOffset), vec3(-4.0f + a_iXIndex + fLengthOffset, a_fYCoord, -4.0f + a_iZIndex + fLengthOffset), a_Colour);
	//	draw the verticals
	Gizmos::addLine(vec3(-4.0f + a_iXIndex + fLengthOffset, fYBottom, -4.0f + a_iZIndex + fLengthOffset), vec3(-4.0f + a_iXIndex + fLengthOffset, a_fYCoord, -4.0f + a_iZIndex + fLengthOffset), a_Colour);
	Gizmos::addLine(vec3(-3.0f + a_iXIndex - fLengthOffset, fYBottom, -4.0f + a_iZIndex + fLengthOffset), vec3(-3.0f + a_iXIndex - fLengthOffset, a_fYCoord, -4.0f + a_iZIndex + fLengthOffset), a_Colour);
	Gizmos::addLine(vec3(-3.0f + a_iXIndex - fLengthOffset, fYBottom, -3.0f + a_iZIndex - fLengthOffset), vec3(-3.0f + a_iXIndex - fLengthOffset, a_fYCoord, -3.0f + a_iZIndex - fLengthOffset), a_Colour);
	Gizmos::addLine(vec3(-4.0f + a_iXIndex + fLengthOffset, fYBottom, -3.0f + a_iZIndex - fLengthOffset), vec3(-4.0f + a_iXIndex + fLengthOffset, a_fYCoord, -3.0f + a_iZIndex - fLengthOffset), a_Colour);
}

void	Checkers::ReloadShader()
{
	//	may need to add a check for not loaded programs here, given I'm using this to do the initial shader loading as well
	glDeleteProgram(m_uiProgramID);
	glDeleteProgram(m_uiModelProgramID);
	//LoadShader("shaders/perlin_vertex.glsl", 0, "shaders/perlin_fragment.glsl", &m_uiProgramID);
	LoadShader("./shaders/lighting_vertex.glsl", nullptr, "./shaders/lighting_fragment.glsl", &m_uiProgramID);
	LoadShader("./shaders/lighting_colour_vertex.glsl", nullptr, "./shaders/lighting_colour_fragment.glsl", &m_uiModelProgramID);

	LoadShader("shaders/gbuffer_vertex.glsl", 0, "shaders/gbuffer_fragment.glsl", &m_uiGBufferProgram);
	LoadShader("shaders/composite_vertex.glsl", 0, "shaders/composite_fragment.glsl", &m_uiCompositeProgram);
	LoadShader("shaders/composite_vertex.glsl", 0, "shaders/directional_light_fragment.glsl", &m_uiDirectionalLightProgram);
	LoadShader("shaders/point_light_vertex.glsl", 0, "shaders/point_light_fragment.glsl", &m_uiPointLightProgram);

}


///////////////////////////////////////////////////////////////////////////////
//	Deferred Rendering Functions
///////////////////////////////////////////////////////////////////////////////
void	Checkers::BuildLightCube()
{
	//	this will build a unit cube with REVERSE winding order so that the back face automatically renders for the light
	//	instead of the front faces, without disabling backface culling
	//	it is a copy / paste from the build cube
	//	no UV's are required
	float	vertexData[]
	{
		-1, -1, 1, 1,
			1, -1, 1, 1,
			1, -1, -1, 1,
			-1, -1, -1, 1,

			-1, 1, 1, 1,
			1, 1, 1, 1,
			1, 1, -1, 1,
			-1, 1, -1, 1,
	};

	//	the index data is in REVERSE winding order so the back face is rendered rather than the front face
	unsigned int	indexData[]
	{
		4, 5, 0,
			5, 1, 0,
			5, 6, 1,
			6, 2, 1,
			6, 7, 2,
			7, 3, 2,

			7, 4, 3,
			4, 0, 3,
			7, 6, 4,
			6, 5, 4,
			0, 1, 3,
			1, 2, 3,
	};

	m_LightCube.m_uiIndexCount = 36;

	glGenVertexArrays(1, &m_LightCube.m_uiVAO);
	glBindVertexArray(m_LightCube.m_uiVAO);

	glGenBuffers(1, &m_LightCube.m_uiVBO);
	glGenBuffers(1, &m_LightCube.m_uiIBO);


	glBindBuffer(GL_ARRAY_BUFFER, m_LightCube.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_LightCube.m_uiIBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	Checkers::BuildScreenSpaceQuad()
{
	vec2	halfTexel = 1.0f / (vec2((float)BUFFER_WIDTH, (float)BUFFER_HEIGHT) * 0.5f);
	//	vec2	halfTexel = 1.0f / vec2(1280.0f / 720.0f);
	//	this is so that the UV's are sampled from the correct texel.
	float	vertexData[]
	{
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
			-1, 1, 0, 1, halfTexel.x, 1.0f - halfTexel.y,
			1, 1, 0, 1, 1.0f - halfTexel.x, 1.0f - halfTexel.y,
			1, -1, 0, 1, 1.0f - halfTexel.x, halfTexel.y,
	};

	unsigned int	indexData[]
	{
		2, 1, 0,
			3, 2, 0
	};

	m_ScreenSpaceQuad.m_uiIndexCount = 6;

	glGenVertexArrays(1, &m_ScreenSpaceQuad.m_uiVAO);
	glBindVertexArray(m_ScreenSpaceQuad.m_uiVAO);

	glGenBuffers(1, &m_ScreenSpaceQuad.m_uiVBO);
	glGenBuffers(1, &m_ScreenSpaceQuad.m_uiIBO);


	glBindBuffer(GL_ARRAY_BUFFER, m_ScreenSpaceQuad.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ScreenSpaceQuad.m_uiIBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position
	glEnableVertexAttribArray(1);	//	tex coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	Checkers::RenderPointLights()
{
	mat4	mCamViewTransform = m_FlyCamera.m_viewTransform;

	int	iPosUniform = glGetUniformLocation(m_uiPointLightProgram, "light_position");
	int	iViewPosUniform = glGetUniformLocation(m_uiPointLightProgram, "light_view_position");
	int	iLightDiffuseUniform = glGetUniformLocation(m_uiPointLightProgram, "light_diffuse");
	int	iLightRadiusUniform = glGetUniformLocation(m_uiPointLightProgram, "light_radius");
	vec3	vLightPos(0, 0, 0);
	vec3	vLightDiffuse(0, 0, 0);
	vec4	viewspacePos;

	for (int i = 0; i < aPointLightsX.size(); ++i)
	{
		//	now go through the point lights and render them
		vLightPos.x = aPointLightsX[i];
		vLightPos.y = aPointLightsY[i];
		vLightPos.z = aPointLightsZ[i];
		vLightDiffuse.r = aPointLightsColourR[i];
		vLightDiffuse.g = aPointLightsColourG[i];
		vLightDiffuse.b = aPointLightsColourB[i];
		viewspacePos = mCamViewTransform * vec4(vLightPos, 1.0f);
		glUniform3fv(iPosUniform, 1, (float*)&vLightPos);
		glUniform3fv(iViewPosUniform, 1, (float*)&viewspacePos.xyz);
		glUniform3fv(iLightDiffuseUniform, 1, (float*)&vLightDiffuse);
		glUniform1f(iLightRadiusUniform, aPointLightsRange[i]);

		glBindVertexArray(m_LightCube.m_uiVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
}

void	Checkers::AddPointLight(float a_fX, float a_fY, float a_fZ, float a_fR, float a_fG, float a_fB, float a_fRange)
{
	aPointLightsX.push_back(a_fX);
	aPointLightsY.push_back(a_fY);
	aPointLightsZ.push_back(a_fZ);
	aPointLightsColourR.push_back(a_fR);
	aPointLightsColourG.push_back(a_fG);
	aPointLightsColourB.push_back(a_fB);
	aPointLightsRange.push_back(a_fRange);
}

void	Checkers::RenderDirectionalLight(vec3 light_dir, vec3 light_colour)
{
	vec4	viewspace_light_dir = m_FlyCamera.m_viewTransform * vec4(glm::normalize(light_dir), 0);
	int iLightDirUniform = glGetUniformLocation(m_uiDirectionalLightProgram, "light_dir");
	int iLightColourUniform = glGetUniformLocation(m_uiDirectionalLightProgram, "light_colour");

	glUniform3fv(iLightDirUniform, 1, (float*)&viewspace_light_dir);
	glUniform3fv(iLightColourUniform, 1, (float*)&light_colour);

	glBindVertexArray(m_ScreenSpaceQuad.m_uiVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void	Checkers::BuildGBuffer()
{
	//	create the framebuffer
	glGenFramebuffers(1, &m_uiGBufferFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiGBufferFBO);

	//	generate all our textures
	//	albedo, position, normal, depthI(depth is render buffer)
	glGenTextures(1, &m_uiAlbedoTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiAlbedoTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, BUFFER_WIDTH, BUFFER_HEIGHT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &m_uiPositionsTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiPositionsTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, BUFFER_WIDTH, BUFFER_HEIGHT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &m_uiNormalsTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiNormalsTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, BUFFER_WIDTH, BUFFER_HEIGHT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &m_uiGBufferDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_uiGBufferDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, BUFFER_WIDTH, BUFFER_HEIGHT);

	//	attach our textures to the framebuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_uiAlbedoTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_uiPositionsTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_uiNormalsTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_uiGBufferDepth);

	GLenum	targets[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	glDrawBuffers(3, targets);

	//	check that it worked
	GLenum	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Aahhhh!!  It broke!!  G-Buffer creation FAIL!!\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void	Checkers::BuildLightBuffer()
{
	//	create the fbo
	glGenFramebuffers(1, &m_uiLightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiLightFBO);

	//	create textures
	//	just the light texture this time
	glGenTextures(1, &m_uiLightTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiLightTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, BUFFER_WIDTH, BUFFER_HEIGHT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//	attach textures
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_uiLightTexture, 0);
	GLenum	lightTarget = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &lightTarget);

	//	check framebuffer for success
	GLenum	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Aahhhh!!  It broke!!  Light Buffer creation FAIL!!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void	Checkers::DrawModelsDeferred()
{
	////////////////////////
	//	G-BUFFER generation
	////////////////////////
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, m_uiGBufferFBO);
	glClearColor(0, 0, 0, 0);	//	so we know where nothing was rendered

	glClear(GL_DEPTH_BUFFER_BIT);

	vec4	clearColour = vec4(0, 0, 0, 0);
	vec4	clearNormal = vec4(0.5f, 0.5f, 0.5f, 0.0f);

	glClearBufferfv(GL_COLOR, 0, (float*)&clearColour);
	glClearBufferfv(GL_COLOR, 1, (float*)&clearColour);
	glClearBufferfv(GL_COLOR, 2, (float*)&clearNormal);

	glUseProgram(m_uiGBufferProgram);

	int	iViewUniform = glGetUniformLocation(m_uiGBufferProgram, "view");
	int	iViewProjUniform = glGetUniformLocation(m_uiGBufferProgram, "view_proj");

	glUniformMatrix4fv(iViewUniform, 1, GL_FALSE, (float*)&m_FlyCamera.m_viewTransform);
	glUniformMatrix4fv(iViewProjUniform, 1, GL_FALSE, (float*)&m_FlyCamera.m_projectionViewTransform);

	//glBindVertexArray(m_Bunny.m_uiVAO);
	//glDrawElements(GL_TRIANGLES, m_Bunny.m_uiIndexCount, GL_UNSIGNED_INT, 0);

	//	now draw the checkers
	//	these use their own version of the lighting shader that takes in a uniform for the piece colour
	//glUseProgram(m_uiModelProgramID);
	//int	iViewProjUniform = glGetUniformLocation(m_uiModelProgramID, "projection_view");
	//glUniformMatrix4fv(iViewProjUniform, 1, GL_FALSE, (float*)&m_FlyCamera.m_projectionViewTransform);
	//int	eye_pos_uniform = glGetUniformLocation(m_uiModelProgramID, "eye_pos");
	//glUniform3fv(eye_pos_uniform, 1, (float*)&m_FlyCamera.GetPosition());
	//int	iLightDirUniform = glGetUniformLocation(m_uiModelProgramID, "light_dir");
	//glUniform3fv(iLightDirUniform, 1, (float*)&m_vLightDir);
	//int	iLightColourUniform = glGetUniformLocation(m_uiModelProgramID, "light_colour");
	//glUniform3fv(iLightColourUniform, 1, (float*)&m_vLightColour.rgb);
	//int	iAmbientUniform = glGetUniformLocation(m_uiModelProgramID, "ambient_light");
	//glUniform3fv(iAmbientUniform, 1, (float*)&m_vAmbientLightColour.rgb);
	//int	iSpecPowerUniform = glGetUniformLocation(m_uiModelProgramID, "specular_power");
	//glUniform1f(iSpecPowerUniform, m_fCheckerPieceSpecPower);
	int	iWorldUniform = glGetUniformLocation(m_uiGBufferProgram, "worldTransform");

	vec3	vPieceColour = m_Player1Colour.rgb;	//	this changes for each player ...
	int	iCheckerColourUniform = glGetUniformLocation(m_uiGBufferProgram, "material_colour");
	glUniform3fv(iCheckerColourUniform, 1, (float*)&vPieceColour);

	//	get a copy of the bitboards
	Bitboard	bbP1Pieces = m_Game.m_oGameState.m_P1Pieces;
	Bitboard	bbP2Pieces = m_Game.m_oGameState.m_P2Pieces;
	Bitboard	bbKings = m_Game.m_oGameState.m_Kings;

	vec3	vCheckerScale(0.25f, 0.25f, 0.25f);
	for (int i = 0; i < 32; ++i)
	{
		if ((bbP1Pieces & bbKings & (0x00000001 << i)) > 0)
		{
			//	there is a P1 King in this position
			//	this needs to be different for each checker
			m_CheckerWorldTransform = glm::scale(glm::translate(aKingCoords[i]), vCheckerScale);
			glUniformMatrix4fv(iWorldUniform, 1, GL_FALSE, (float*)&m_CheckerWorldTransform);
			glBindVertexArray(m_KingMesh.m_uiVAO);
			glDrawElements(GL_TRIANGLES, m_KingMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);
		}
		else if ((bbP1Pieces & (0x00000001 << i)) > 0)
		{
			//	There is a regular piece in this position
			//	this needs to be different for each checker
			m_CheckerWorldTransform = glm::scale(glm::translate(aPieceCoords[i]), vCheckerScale);
			//iWorldUniform = glGetUniformLocation(m_uiModelProgramID, "worldTransform");
			glUniformMatrix4fv(iWorldUniform, 1, GL_FALSE, (float*)&m_CheckerWorldTransform);
			glBindVertexArray(m_CheckerMesh.m_uiVAO);
			glDrawElements(GL_TRIANGLES, m_CheckerMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);
		}
	}

	vPieceColour = m_Player2Colour.rgb;	//	this changes for each player ...
	iCheckerColourUniform = glGetUniformLocation(m_uiGBufferProgram, "material_colour");
	glUniform3fv(iCheckerColourUniform, 1, (float*)&vPieceColour);
	for (int i = 0; i < 32; ++i)
	{
		if ((bbP2Pieces & bbKings & (0x00000001 << i)) > 0)
		{
			//	there is a P1 King in this position
			//	this needs to be different for each checker
			m_CheckerWorldTransform = glm::scale(glm::translate(aKingCoords[i]), vCheckerScale);
			glUniformMatrix4fv(iWorldUniform, 1, GL_FALSE, (float*)&m_CheckerWorldTransform);
			glBindVertexArray(m_KingMesh.m_uiVAO);
			glDrawElements(GL_TRIANGLES, m_KingMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);
		}
		else if ((bbP2Pieces & (0x00000001 << i)) > 0)
		{
			//	There is a regular piece in this position
			//	this needs to be different for each checker
			m_CheckerWorldTransform = glm::scale(glm::translate(aPieceCoords[i]), vCheckerScale);
			//iWorldUniform = glGetUniformLocation(m_uiModelProgramID, "worldTransform");
			glUniformMatrix4fv(iWorldUniform, 1, GL_FALSE, (float*)&m_CheckerWorldTransform);
			glBindVertexArray(m_CheckerMesh.m_uiVAO);
			glDrawElements(GL_TRIANGLES, m_CheckerMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);
		}
	}


	//	draw the checkerboard
	glUseProgram(m_uiGBufferProgram);
	iViewProjUniform = glGetUniformLocation(m_uiGBufferProgram, "view_proj");
	glUniformMatrix4fv(iViewProjUniform, 1, GL_FALSE, (float*)&m_FlyCamera.m_projectionViewTransform);
	iWorldUniform = glGetUniformLocation(m_uiGBufferProgram, "worldTransform");
	glUniformMatrix4fv(iWorldUniform, 1, GL_FALSE, (float*)&m_CheckerBoardWorldTransform);
//	int iTexUniform = glGetUniformLocation(m_uiGBufferProgram, "albedoTexture");
//	glUniform1i(iTexUniform, 0);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, m_uiCheckerBoardTexture);
//	eye_pos_uniform = glGetUniformLocation(m_uiGBufferProgram, "eye_pos");
//	glUniform3fv(eye_pos_uniform, 1, (float*)&m_FlyCamera.GetPosition());
//	iLightDirUniform = glGetUniformLocation(m_uiGBufferProgram, "light_dir");
//	glUniform3fv(iLightDirUniform, 1, (float*)&m_vLightDir);
//	iLightColourUniform = glGetUniformLocation(m_uiGBufferProgram, "light_colour");
//	glUniform3fv(iLightColourUniform, 1, (float*)&m_vLightColour.rgb);
//	iAmbientUniform = glGetUniformLocation(m_uiGBufferProgram, "ambient_light");
//	glUniform3fv(iAmbientUniform, 1, (float*)&m_vAmbientLightColour.rgb);
//	iSpecPowerUniform = glGetUniformLocation(m_uiGBufferProgram, "specular_power");
//	glUniform1f(iSpecPowerUniform, m_fCheckerboardSpecPower);

	//	this is temporary until I get the texture working
	vPieceColour = vec3(0.25f);	//	make the board temporarily dark
	iCheckerColourUniform = glGetUniformLocation(m_uiGBufferProgram, "material_colour");
	glUniform3fv(iCheckerColourUniform, 1, (float*)&vPieceColour);

	glBindVertexArray(m_BoardMesh.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_BoardMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);
/*
	//	now draw the Backboard
	//	uses the same shader program, so no need to load change here yet
	iWorldUniform = glGetUniformLocation(m_uiProgramID, "worldTransform");
	glUniformMatrix4fv(iWorldUniform, 1, GL_FALSE, (float*)&m_BackBoardWorldTransform);
	iTexUniform = glGetUniformLocation(m_uiProgramID, "albedoTexture");
	glUniform1i(iTexUniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiBackBoardTexture);

	glBindVertexArray(m_BackBoardMesh.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_BackBoardMesh.m_uiIndexCount, GL_UNSIGNED_INT, 0);
*/

}

void	Checkers::AccumulateLightsDeferred()
{
	////////////////////////
	//	Light accumulation
	////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiLightFBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(m_uiDirectionalLightProgram);
	int	iPositionTexUniform = glGetUniformLocation(m_uiDirectionalLightProgram, "position_tex");
	int	iNormalsTexUniform = glGetUniformLocation(m_uiDirectionalLightProgram, "normals_tex");
	glUniform1i(iPositionTexUniform, 0);
	glUniform1i(iNormalsTexUniform, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiPositionsTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_uiNormalsTexture);

	vec3	light_dir = m_vLightDir;
	////	vec3	light_colour = vec3(0.2f, 0.4f, 0.8f);
	vec3	light_colour = m_vLightColour.rgb;
	RenderDirectionalLight(light_dir, light_colour);
	//	RenderDirectionalLight(vec3(0, 1, 0), vec3(0.5f, 0.25f, 0.0f));
	//	RenderDirectionalLight(vec3(1, 1, 0), vec3(0.6f, 0.0f, 0.0f));
	//	RenderDirectionalLight(vec3(1, 0, 0), vec3(0.0f, 0.6f, 0.0f));
	//	RenderDirectionalLight(vec3(0, 0, 1), vec3(0.0f, 0.0f, 0.75f));
	//	RenderDirectionalLight(vec3(0.0f, 1.0f, 1.0f), vec3(0.4f, 0.4f, 0.4f));
	//	RenderDirectionalLight(vec3(-1.0f, -0.10f, -1.0f), vec3(0.5f, 0.5f, 0.0f));

	//	now render the point lights
	glUseProgram(m_uiPointLightProgram);
	int	iViewProjUniform = glGetUniformLocation(m_uiPointLightProgram, "proj_view");
	iPositionTexUniform = glGetUniformLocation(m_uiPointLightProgram, "position_texture");
	iNormalsTexUniform = glGetUniformLocation(m_uiPointLightProgram, "normal_texture");

	glUniformMatrix4fv(iViewProjUniform, 1, GL_FALSE, (float*)&m_FlyCamera.m_projectionViewTransform);
	glUniform1i(iPositionTexUniform, 0);
	glUniform1i(iNormalsTexUniform, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiPositionsTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_uiNormalsTexture);

	//	draw the point lights
	RenderPointLights();

	glDisable(GL_BLEND);
}

void	Checkers::RenderCompositePass()
{
	////////////////////////
	//	Composite pass
	////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_uiCompositeProgram);

	//	these need to be passed in to the composite pass as well:
	//	m_uiAlbedoTexture
	//	m_uiPositionsTexture
	//	m_uiNormalsTexture

	int	iAlbedoTexUniform = glGetUniformLocation(m_uiCompositeProgram, "albedo_tex");
	int	iPositionsTexUniform = glGetUniformLocation(m_uiCompositeProgram, "position_tex");
	int	iNormalsTexUniform = glGetUniformLocation(m_uiCompositeProgram, "normals_tex");
	int	iLightTexUniform = glGetUniformLocation(m_uiCompositeProgram, "light_tex");
	int	iSpecUniform = glGetUniformLocation(m_uiCompositeProgram, "fSpecPower");
	int	iEyePosUniform = glGetUniformLocation(m_uiCompositeProgram, "eye_pos");
	int	iAmbientUniform = glGetUniformLocation(m_uiCompositeProgram, "ambient_light");

	glUniform1i(iAlbedoTexUniform, 0);
	glUniform1i(iPositionsTexUniform, 1);
	glUniform1i(iNormalsTexUniform, 2);
	glUniform1i(iLightTexUniform, 3);
	glUniform1f(iSpecUniform, m_fDeferredSpecPower);
	glUniform3fv(iEyePosUniform, 1, (float*)&m_FlyCamera.GetPosition());
	glUniform3fv(iAmbientUniform, 1, (float*)&m_vAmbientLightColour.rgb);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiAlbedoTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_uiPositionsTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_uiNormalsTexture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_uiLightTexture);

	/*
	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	*/

	glBindVertexArray(m_ScreenSpaceQuad.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_ScreenSpaceQuad.m_uiIndexCount, GL_UNSIGNED_INT, 0);
}

void	Checkers::SetupDeferredLights()
{
	//	Add lights to indicate Player 1's side
	for (int i = 0; i < 5; ++i)
	{
		AddPointLight(i * 2 - 4.0f, 2.0f, -4.0f, 0.5f, 0.1f, 0.1f, 3.5f);
	}
	//	Add lights to indicate Player 2's side
	for (int i = 0; i < 5; ++i)
	{
		AddPointLight(i * 2 - 4.0f, 2.0f, 4.0f, 0.1f, 0.1f, 0.5f, 3.5f);
	}
	//	Add lights above the board
	for (int i = 0; i < 13; ++i)
	{
		for (int j = 0; j < 13; ++j)
		{
			AddPointLight(i - 6.0f, 0.5f, j - 6.0f, 0.25f, 0.25f, 0.25f, 1.0f);
		}
	}
	//	Add lights below the board
	for (int i = 0; i < 7; ++i)
	{
		for (int j = 0; j < 7; ++j)
		{
			AddPointLight(i*2 - 6.0f, -1.5f, j*2 - 6.0f, 0.25f, 0.25f, 0.25f, 2.5f);
		}
	}

//	AddPointLight(0, 2, 0, 0.5f, 0.5f, 0.5f, 2.75f);	//	initial test light at (0, 5, 0) with a range of 10
//	AddPointLight(-3, 1, 0, 0.25f, 0.25f, 0.5f, 2.5f);	//	initial test light at (0, 5, 0) with a range of 10
//	AddPointLight(3, 1, -3, 0.25f, 0.5f, 0.25f, 2.5f);	//	initial test light at (0, 5, 0) with a range of 10
//	AddPointLight(-3, 1, -3, 0.5f, 0.25f, 0.25f, 2.5f);	//	initial test light at (0, 5, 0) with a range of 10
}