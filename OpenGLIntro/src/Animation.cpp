#include "Animation.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

Animation::Animation()
{
}
Animation::~Animation()
{
}

bool	Animation::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create();
	m_bDrawGizmos = true;

	//	now initialise the FlyCamera
	m_FlyCamera = FlyCamera(vec3(15, 15, 25), vec3(0, 10, 0), glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
	m_BackgroundColour = vec4(0.3f, 0.3f, 0.3f, 1.0f);

	m_file = new FBXFile();
	m_file->load("./models/characters/Pyro/pyro.fbx");
	m_file->initialiseOpenGLTextures();
	GenerateGLMeshes(m_file);
	LoadShader("./shaders/skinned_vertex.glsl", nullptr, "./shaders/skinned_fragment.glsl", &m_uiProgramID);

	return true;
}

void	Animation::shutdown()
{
	//	now clean up
	m_file->unload();
	Gizmos::destroy();
}

bool	Animation::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float	dT = (float)glfwGetTime();
	glfwSetTime(0.0f);
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

	FBXSkeleton*	skele = m_file->getSkeletonByIndex(0);
	FBXAnimation*	anim = m_file->getAnimationByIndex(0);

//	skele->evaluate(anim, m_timer * 0.15f);
	EvaluateSkeleton(anim, skele, m_timer * 5.15f);

	for (unsigned int i = 0; i < skele->m_boneCount; ++i)
	{
		skele->m_nodes[i]->updateGlobalTransform();
		mat4	nodeGlobal = skele->m_nodes[i]->m_globalTransform;
		vec3	nodePos = nodeGlobal[3].xyz;

		Gizmos::addAABBFilled(nodePos, vec3(2.0f), vec4(1, 0, 0, 1), &nodeGlobal);

		if (skele->m_nodes[i]->m_parent != nullptr)
		{
			vec3	parentPos = skele->m_nodes[i]->m_parent->m_globalTransform[3].xyz;
			Gizmos::addLine(nodePos, parentPos, vec4(0, 1, 0, 1));
		}
	}
	
	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	return true;
}

void	Animation::draw()
{
	Application::draw();
	if (m_bDrawGizmos)
	{
		Gizmos::draw(m_FlyCamera.GetProjectionView());
	}
	glUseProgram(m_uiProgramID);
	int	viewProjUniform = glGetUniformLocation(m_uiProgramID, "projection_view");
	glUniformMatrix4fv(viewProjUniform, 1, GL_FALSE, (float*)&m_FlyCamera.GetProjectionView());
	int	diffuseUniform = glGetUniformLocation(m_uiProgramID, "diffuse");
	glUniform1i(diffuseUniform, 0);

	FBXSkeleton*	skeleton = m_file->getSkeletonByIndex(0);
	//skeleton->updateBones();
	UpdateBones(skeleton);
	int	bonesUniform = glGetUniformLocation(m_uiProgramID, "bones");
	glUniformMatrix4fv(bonesUniform, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	for (unsigned int i = 0; i < m_meshes.size(); ++i)
	{
		FBXMeshNode*	currMesh = m_file->getMeshByIndex(i);
		FBXMaterial*	meshMaterial = currMesh->m_material;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, meshMaterial->textures[FBXMaterial::DiffuseTexture]->handle);

		mat4	worldTransform = currMesh->m_globalTransform;
		int worldUniform = glGetUniformLocation(m_uiProgramID, "world");
		glUniformMatrix4fv(worldUniform, 1, GL_FALSE, (float*)&worldTransform);
		glBindVertexArray(m_meshes[i].m_uiVAO);
		glDrawElements(GL_TRIANGLES, m_meshes[i].m_uiIndexCount, GL_UNSIGNED_INT, 0);
	}

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void	Animation::GenerateGLMeshes(FBXFile* fbx)
{
	unsigned int	meshCount = fbx->getMeshCount();
	m_meshes.resize(meshCount);
	for (unsigned int meshIndex = 0; meshIndex < meshCount; ++meshIndex)
	{
		FBXMeshNode*	currMesh = fbx->getMeshByIndex(meshIndex);
		m_meshes[meshIndex].m_uiIndexCount = currMesh->m_indices.size();
		glGenBuffers(1, &m_meshes[meshIndex].m_uiVBO);
		glGenBuffers(1, &m_meshes[meshIndex].m_uiIBO);
		glGenVertexArrays(1, &m_meshes[meshIndex].m_uiVAO);
		glBindVertexArray(m_meshes[meshIndex].m_uiVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_meshes[meshIndex].m_uiVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex)* currMesh->m_vertices.size(), currMesh->m_vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshes[meshIndex].m_uiIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * currMesh->m_indices.size(), currMesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);	//	pos
		glEnableVertexAttribArray(1);	//	tex coord
		glEnableVertexAttribArray(2);	//	bone indices
		glEnableVertexAttribArray(3);	//	bone weights

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void	Animation::EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton* skeleton, float timer)
{
	float	fps = 24.0f;
	int	currentFrame = (int)(timer * fps);
	//	loop through all the tracks
	for (unsigned int trackIndex = 0; trackIndex < anim->m_trackCount; ++trackIndex)
	{
		//	wrap back to the start of the track if we have gone too far
		int	trackFrameCount = anim->m_tracks[trackIndex].m_keyframeCount;
		int	trackFrame = currentFrame % trackFrameCount;

		//	find what keyframes are currently affecting the bones
		FBXKeyFrame	currFrame = anim->m_tracks[trackIndex].m_keyframes[trackFrame];
		FBXKeyFrame	nextFrame = anim->m_tracks[trackIndex].m_keyframes[trackFrame + 1];

		//	interpolate between those keyframes to generate the matrix for the current pose
		float	timeSinceFrameFlip = timer - ((int)currentFrame / fps);
		float	t = timeSinceFrameFlip * fps;
		vec3	newPos = glm::mix(currFrame.m_translation, nextFrame.m_translation, t);
		vec3	newScale = glm::mix(currFrame.m_scale, nextFrame.m_scale, t);
		//newScale = ((float)trackFrame / (float)trackFrameCount) * newScale;
		glm::quat	newRot = glm::slerp(currFrame.m_rotation, nextFrame.m_rotation, t);
		mat4	localTransform = glm::translate(newPos) * glm::toMat4(newRot) * glm::scale(newScale);

		//	get the right bone for the given track
		int	boneIndex = anim->m_tracks[trackIndex].m_boneIndex;

		//	set the FBXNode's local transforms to match
		if (boneIndex < skeleton->m_boneCount)
		{
			skeleton->m_nodes[boneIndex]->m_localTransform = localTransform;
		}
	}

}

void	Animation::UpdateBones(FBXSkeleton* skeleton)
{
	//	loop through the nodes in the skeleton
	for (unsigned int boneIndex = 0; boneIndex < skeleton->m_boneCount; ++boneIndex)
	{
		//	generate their global transforms
		int	parentIndex = skeleton->m_parentIndex[boneIndex];
		if (parentIndex == -1)
		{
			//	no parent
			skeleton->m_bones[boneIndex] = skeleton->m_nodes[boneIndex]->m_localTransform;
		}
		else
		{
			skeleton->m_bones[boneIndex] = skeleton->m_bones[parentIndex] * skeleton->m_nodes[boneIndex]->m_localTransform;
		}
	}
	for (unsigned int boneIndex = 0; boneIndex < skeleton->m_boneCount; ++boneIndex)
	{
		//	multiply the global transform by the inverse bind pose
		skeleton->m_bones[boneIndex] = skeleton->m_bones[boneIndex] * skeleton->m_bindPoses[boneIndex];
	}
}


