#include "BBPhysics.h"


BBPhysics::BBPhysics()
{
}

BBPhysics::~BBPhysics()
{
}



///////////////////////////////////////////////////////////////////////////////////////
//	BBSphere class functions
///////////////////////////////////////////////////////////////////////////////////////
BBPhysicsScene::BBPhysicsScene()
{
	m_vGravity = vec2(0.0f,	-0.05f);	//	set the y gravity to -9.8 for standard gravity
	m_fTimeStep = 0.001f;	//	just so it has a non-zero value
	m_fMaxTimeStep = 0.05f;	//	make the physics at least 20fps
}

BBPhysicsScene::~BBPhysicsScene()
{
}

void	BBPhysicsScene::AddActor(BBPhysicsObject* a_pActor)
{
	if (a_pActor != nullptr)
	{
		m_aActors.push_back(a_pActor);
	}
}

void	BBPhysicsScene::RemoveActor(BBPhysicsObject* a_pActor)
{
	if (a_pActor != nullptr)
	{
		for (std::vector<BBPhysicsObject*>::iterator i = m_aActors.begin(); i != m_aActors.end(); ++i)
		{
			if (*i == a_pActor)
			{
				m_aActors.erase(i);
			}
		}
	}
}

void	BBPhysicsScene::update(float dT)
{
	if (dT > m_fMaxTimeStep)
	{
		m_fTimeStep = m_fMaxTimeStep;
	}
	else
	{
		m_fTimeStep = dT;
	}
	for (unsigned int i = 0; i < m_aActors.size(); ++i)
	{
		m_aActors[i]->update(m_vGravity, m_fTimeStep);
	}
}

void	BBPhysicsScene::DebugScene()
{
}

void	BBPhysicsScene::AddGizmos()
{
	for (unsigned int i = 0; i < m_aActors.size(); ++i)
	{
		m_aActors[i]->MakeGizmo();
	}
}


