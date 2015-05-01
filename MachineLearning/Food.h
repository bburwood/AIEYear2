#pragma once

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "Gizmos.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "NeuralNetwork.h"
#include <vector>

class Food
{
private:
	glm::vec2 _centre;
	float _range;
	bool active;
public:
	Food();
	Food(glm::vec2, float range);
	~Food();
	void addGizmo();
	float checkRange(glm::vec2 testPoint);
};