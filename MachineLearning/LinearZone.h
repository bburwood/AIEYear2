#pragma once

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "Gizmos.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "NeuralNetwork.h"
#include <vector>

class LinearZone
{
private:
	glm::vec2 _centre;
	float _rotation;
	bool active;
public:
	LinearZone();
	LinearZone(glm::vec2, float range);
	~LinearZone();
	void addGizmo();
	float checkRange(glm::vec2 testPoint);
};