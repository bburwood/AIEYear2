#include "Agent.h"
#include <iostream>

extern int gScreenWidth;
extern int gScreenHeight;

Agent::Agent()
{
}

Agent::~Agent()
{
}
void Agent::update(float delta)
{
	glm::vec3 testinput = glm::vec3(_position.x, _position.y, 0);
	_clock += delta;
	_memoryClock+= delta;
	float memoryFrequency = 1;
	if (_memoryClock > memoryFrequency)
	{
		_memoryClock -= memoryFrequency;
		glm::vec3 testinput = glm::vec3(_position.x, _position.y,0);
		addToMemory(testinput);
		neuralNetwork->trainNetwork(memory); 
	}
	_position += _velocity * delta;
	if (checkBounds())
	{
		_facingDirection = 44 / 7.0f * ((rand() % 1000) / 1000.0f);
		_velocity.x = _maxSpeed * sin(_facingDirection);
		_velocity.y = _maxSpeed * cos(_facingDirection);
	}
	neuralNetwork->renderDebug(glm::vec2(60, 60), 200, memory);
	_foodClock--;
}

bool Agent::checkBounds()
{
	if ((_position.x < 0 && _velocity.x < 0) || (_position.x >gScreenWidth  && _velocity.x > 0) )
	{ 

		return true;
	}
	if ((_position.y < 0 && _velocity.y < 0) || (_position.y >gScreenHeight  && _velocity.y > 0))
	{
		return true;
	}
	return false;
}

void Agent::draw()
{
}

void Agent::setup(glm::vec2 startPos, float size,glm::vec4 colour,float facingDirection)
{
	_position = startPos;
	_startingPosition = startPos;
	_facingDirection = facingDirection;
	_startingFacingDirection = facingDirection;
	_diameter = size;
	_clock = 0;
	_foodClock = 0;
	_memoryClock = 0;
	_colour = colour;
	_maxSpeed = 500;
	_velocity.x = _maxSpeed * sin(_facingDirection);
	_velocity.y = _maxSpeed * cos(_facingDirection);
	health = StartingHealth;
	if (neuralNetwork == NULL)
	{
		int numberInputs = 2;
		int numberHiddenNeurons = 4;  //the tutorial notes claim that there are three neurons in hte hidden layer but performance is rather poor with only three and four works better. Try three as an experiment
		int numberOutputs = 4; 
		neuralNetwork = new NeuralNetwork(numberInputs, numberHiddenNeurons, numberOutputs);
		initMemory(memory);
		neuralNetwork->trainNetwork(memory);  //mock up for testing purposes
		neuralNetwork->setUpDebugRender(60); //setup our debug 
	}
}

void Agent::initMemory(vector<glm::vec3>& memory)
{
	memory.push_back(glm::vec3(_position.x, _position.y, 0));
}

void Agent::addToMemory(glm::vec3 newMemory)
{
	newMemory.x /= (float)gScreenWidth;
	newMemory.y /= (float)gScreenHeight;
	memory.push_back(newMemory);
}

void Agent::addGizmo()
{
	int segments = 20;
	Gizmos::add2DCircle(_position, _diameter*1.1f / 2, segments, glm::vec4(0, 0, 0, 1));
	Gizmos::add2DCircle(_position, _diameter*.9f / 2, segments, _colour);


}

void Agent::hurtAgent(float damage)
{
	health -= damage;
	if (health < 0)
	{
		addToMemory(glm::vec3(_position.x, _position.y, 1));
		neuralNetwork->trainNetwork(memory);
		_startingPosition.x = (float)(rand() % gScreenWidth);
		_startingPosition.y = (float)(rand() % 2 * gScreenHeight);
		float size = 20;
		float facing = 44 / 7.0f * ((rand() % 1000) / 1000.0f);
		setup(_startingPosition, _diameter, _colour, facing);
	}
}

void Agent::feedAgent(float foodFound)
{
	if (foodFound > 0)
	{
		health += foodFound;
		if (_foodClock <= 0) //simple mechanism to stop too many food discoveries getting added to memory
		{
			addToMemory(glm::vec3(_position.x, _position.y, 2));
			neuralNetwork->trainNetwork(memory);
			_foodClock = 20;
		}
	}
}


glm::vec2 Agent::getPosition()
{
	return _position;
}