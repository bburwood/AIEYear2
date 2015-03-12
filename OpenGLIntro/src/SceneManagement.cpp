#include "SceneManagement.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"
#include "AABB.h"
#include "BoundingSphere.h"
#include "tiny_obj_loader.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

SceneManagement::SceneManagement()
{
}
SceneManagement::~SceneManagement()
{
}

void	RenderPlane(vec4 a_plane)
{
	vec3	up(0, 1, 0);
	if (a_plane.xyz() == vec3(0, 1, 0))
	{
		up = vec3(1, 0, 0);
	}
	vec3	tangent = glm::normalize(glm::cross(a_plane.xyz(), vec3(0, 1, 0)));
	vec3	bitangent = glm::normalize(glm::cross(a_plane.xyz(), tangent));

	vec3	p = a_plane.xyz * a_plane.w;

	vec3	v0 = p + tangent + bitangent;
	vec3	v1 = p + tangent - bitangent;
	vec3	v2 = p - tangent - bitangent;
	vec3	v3 = p - tangent + bitangent;

	Gizmos::addTri(v0, v1, v2, vec4(1, 1, 0, 1));
	Gizmos::addTri(v0, v2, v3, vec4(1, 1, 0, 1));

	Gizmos::addLine(p, p + a_plane.xyz, vec4(0, 1, 1, 1));
}

void	RenderAABB(AABB aabb, mat4 transform)
{
	vec3	centre = ((aabb.min * transform[3].xyz) - (aabb.max * transform[3].xyz)) * 0.5f;
	vec3	extents = ((aabb.max * transform[3].xyz) - (aabb.min * transform[3].xyz)) * 0.5f;

	Gizmos::addAABB(centre, extents, vec4(1));
}
bool	OnPositivePlaneSide(vec4 a_plane, AABB aabb)
{
	vec3	planeTestA, planeTestB;

	if (a_plane.x >= 0)
	{
		planeTestA.x = aabb.min.x;
		planeTestB.x = aabb.max.x;
	}
	else
	{
		planeTestA.x = aabb.max.x;
		planeTestB.x = aabb.min.x;
	}
	if (a_plane.y >= 0)
	{
		planeTestA.y = aabb.min.y;
		planeTestB.y = aabb.max.y;
	}
	else
	{
		planeTestA.y = aabb.max.y;
		planeTestB.y = aabb.min.y;
	}
	if (a_plane.z >= 0)
	{
		planeTestA.z = aabb.min.z;
		planeTestB.z = aabb.max.z;
	}
	else
	{
		planeTestA.z = aabb.max.z;
		planeTestB.z = aabb.min.z;
	}
	float dA = glm::dot(vec3(a_plane), planeTestA) + a_plane.w;
	float dB = glm::dot(vec3(a_plane), planeTestB) + a_plane.w;
	return ();
}
bool	SceneManagement::startup()
{
	if (Application::startup() == false)
	{
		return false;
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

	//	load meshes here
	//m_meshes.push_back(LoadMesh(...));
	//	then go through them and generate the AABB's for them
	for (unsigned int iMeshIndex = 0; iMeshIndex < m_meshes.size(); ++iMeshIndex)
	{

	}

	return true;
}

void	SceneManagement::shutdown()
{
	//	now clean up
	Gizmos::destroy();
	TwDeleteAllBars();
	TwTerminate();
}

bool	SceneManagement::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	//	check if we need to reload the shaders
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		//ReloadShader();
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

	// a plane that faces up
	// offset from (0,0,0) by 1
	vec4 plane(0, 1, 0, -1);
	// ax + by + cz + d = 0
	// 0 + 1 + 0 + -1 = 0

	BoundingSphere sphere;
	sphere.centre = vec3(0, 2 * cosf(m_timer * 1.5f) + 1, 0);
	sphere.radius = 0.5f;

	float d = glm::dot(vec3(plane), sphere.centre) + plane.w;

	vec4 planeColour(1, 1, 0, 1);
	bool	bSphereVisible = true;

	if (d > sphere.radius)
	{
		printf("Front\n");
		planeColour = vec4(0, 1, 0, 1);
	}
	else if (d < -sphere.radius)
	{
		bSphereVisible = false;
		planeColour = vec4(1, 0, 0, 1);
		printf("Back\n");
	}
	else
	{
		//	printf("On the plane\n");
	}
	Gizmos::addTri(vec3(4, 1, 4), vec3(-4, 1, -4), vec3(-4, 1, 4), planeColour);
	Gizmos::addTri(vec3(4, 1, 4), vec3(4, 1, -4), vec3(-4, 1, -4), planeColour);

	vec4 planes[6];
	getFrustumPlanes(m_FlyCamera.GetProjectionView(), planes);
	for (int i = 0; i < 6; i++)
	{
		float d = glm::dot(vec3(planes[i]), sphere.centre) + planes[i].w;
		if (d < -sphere.radius)
		{
			//bSphereVisible = false;
			printf("Behind, don't render it!\n");
			break;
		}
		else if (d < sphere.radius)
		{
			printf("Touching, we still need to render it!\n");
		}
		else
		{
			printf("Front, fully visible so render it!\n");
		}
	}
	if (bSphereVisible)
	{
		Gizmos::addSphere(sphere.centre, sphere.radius, 8, 8, vec4(1, 0, 1, 1));
	}


	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	return true;
}

void	SceneManagement::draw()
{
	Application::draw();

	vec4	planes[6];
	getFrustumPlanes(m_FlyCamera.GetProjectionView, planes);
	for (unsigned int iMeshIndex = 0; iMeshIndex < m_meshes.size(); ++iMeshIndex)
	{
		bool	bIsInFrustum = true;
		for (unsigned int iPlaneIndex = 0; iPlaneIndex < 6; ++iPlaneIndex)
		{
			vec4	transformedPlanes[6];
			mat4	invTransform = glm::inverse(m_meshes[iMeshIndex].transform);

			vec4	n = planes[iPlaneIndex];
			n.w = 0;
			vec4	p = planes[iPlaneIndex] * planes[iPlaneIndex].w;
			p.w = 1;

			n = invTransform * n;
			p = invTransform * p;

			transformedPlanes[iPlaneIndex] = n;
			transformedPlanes[iPlaneIndex].w = glm::dot(n.xyz, p.xyz);
			RenderPlane(transformedPlanes[iPlaneIndex]);
		}

		for (unsigned int iPlaneIndex = 0; iPlaneIndex < 6; ++iPlaneIndex)
		{
			if (OnPositivePlaneSide(planes[iPlaneIndex], m_meshes[iMeshIndex].m_aabb))
			{
				bIsInFrustum = false;
				break;
			}
		}
		if (bIsInFrustum)
		{

			DrawMesh(m_meshes[iMeshIndex]);
		}
		else
		{

		}
	}
	if (m_bDrawGizmos)
	{
		Gizmos::draw(m_FlyCamera.GetProjectionView());
	}


	TwDraw();
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void SceneManagement::getFrustumPlanes(const glm::mat4& transform, glm::vec4* planes)
{
	// right side
	planes[0] = vec4( transform[0][3] - transform[1][0],
		transform[1][3] - transform[1][0],
		transform[2][3] - transform[2][0],
		transform[3][3] - transform[3][0]);
	// left side
	planes[1] = vec4( transform[0][3] + transform[0][0],
		transform[1][3] + transform[1][0],
		transform[2][3] + transform[2][0],
		transform[3][3] + transform[3][0]);
	// top
	planes[2] = vec4( transform[0][3] - transform[0][1],
		transform[1][3] - transform[1][1],
		transform[2][3] - transform[2][1],
		transform[3][3] - transform[3][1]);
	// bottom
	planes[3] = vec4( transform[0][3] + transform[0][1],
		transform[1][3] + transform[1][1],
		transform[2][3] + transform[2][1],
		transform[3][3] + transform[3][1]);
	// far
	planes[4] = vec4( transform[0][3] - transform[0][2],
		transform[1][3] - transform[1][2],
		transform[2][3] - transform[2][2],
		transform[3][3] - transform[3][2]);
	// near
	planes[5] = vec4( transform[0][3] + transform[0][2],
		transform[1][3] + transform[1][2],
		transform[2][3] + transform[2][2],
		transform[3][3] + transform[3][2]);

	for (int i = 0; i < 6; i++)
		planes[i] = glm::normalize(planes[i]);
}

void	SceneManagement::LoadMesh(char* objFilename)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	tinyobj::LoadObj();
}

void	SceneManagement::DrawMesh(MeshObject a_mesh)
{

}


