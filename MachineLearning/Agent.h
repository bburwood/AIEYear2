#pragma once

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "Gizmos.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "NeuralNetwork.h"
#include <vector>

using namespace std;

const int StartingHealth = 1;

class Agent
{
	float _memoryClock;
	NeuralNetwork* neuralNetwork = NULL;
	glm::vec2 _position;
	float _facingDirection;
	glm::vec2 _startingPosition;
	float _startingFacingDirection;

	float _diameter;
	float _clock;
	int _foodClock;
	glm::vec2 _velocity;

	bool _active;
	glm::vec4 _colour;
	float _maxSpeed;
	bool checkBounds();
	void fakeMemory(vector<glm::vec3>&);
	void initMemory(vector<glm::vec3>&);
	float health;
	vector<glm::vec3> memory;
	void addToMemory(glm::vec3);
public:
	Agent();
	~Agent();
	void setup(glm::vec2 startPos, float size, glm::vec4 colour,float facingDirection);
	void update(float delta);
	void draw();
	void addGizmo();
	void hurtAgent(float damage);
	void feedAgent(float food);
	glm::vec2 getPosition();
};