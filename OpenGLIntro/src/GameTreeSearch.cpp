#include "GameTreeSearch.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

GameTreeSearch::GameTreeSearch()
{
}
GameTreeSearch::~GameTreeSearch()
{
}

bool	GameTreeSearch::startup()
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
	m_FlyCamera = FlyCamera(vec3(-1, 15, 0), vec3(0, 0, 0), glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

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

	//	create a game of Connect Four
	m_game = new ConnectFour();

	//	create an AI, just the random one for now
//	m_ai = new RandomAI();
	
	m_ai = new MCTS(1000);

	return true;
}

void	GameTreeSearch::shutdown()
{
	//	now clean up
	delete m_ai;
	delete m_game;

	Gizmos::destroy();
	TwDeleteAllBars();
	TwTerminate();
}

bool	GameTreeSearch::update()
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
	//for (int i = 0; i <= 20; ++i)
	//{
	//	Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
	//		i == 10 ? white : black);
	//	Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
	//		i == 10 ? white : black);
	//}

	//	now add the code for the Connect Four game update logic
	// keep track of if the mouse has been clicked
	static bool moveMade = false;
	// if the game isn't over...
	if (m_game->getCurrentGameState() == Game::UNKNOWN)
	{
		// if it is the user's turn (player 1)
		if (m_game->getCurrentPlayer() == Game::PLAYER_ONE)
		{
			// if the mouse button is down...
			if (glfwGetMouseButton(m_window, 0) == GLFW_PRESS)
			{
				// if the mouse JUST went down then make a choice
				if (moveMade == false)
				{
					moveMade = true;
					// get the mouse position within the game grid
					double x = 0, y = 0;
					glfwGetCursorPos(m_window, &x, &y);
					m_pickPosition = m_FlyCamera.PickAgainstPlane((float)x, (float)y, glm::vec4(0, 1, 0, 0));
					// determine which column was chosen
					int column = (int)((m_pickPosition.z + ConnectFour::COLUMNS) / 2);
					// if we clicked in the area for a valid column then perform that action
					if (m_game->isActionValid(column))
						m_game->performAction(column);
				}
			}
			else
				moveMade = false;
		}
		else
		{
			// it is the opponent's turn (player 2)
			// use the A.I. to make a decision
			m_game->performAction( m_ai->makeDecision(*m_game) );
		}
	}



	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	return true;
}

void	GameTreeSearch::draw()
{
	Application::draw();

	m_game->draw();

	if (m_bDrawGizmos)
	{
		Gizmos::draw(m_FlyCamera.GetProjectionView());
	}




	TwDraw();
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}


void	GameTreeSearch::ReloadShader()
{
	glDeleteProgram(m_uiProgramID);
	LoadShader("./shaders/lighting_vertex.glsl", nullptr, "./shaders/lighting_fragment.glsl", &m_uiProgramID);
}
