#include "LinearZone.h"

LinearZone::LinearZone(glm::vec2 centre, float rotation)
{
	_centre = centre;
	_rotation = rotation;
	active = true;
}
LinearZone::~LinearZone()
{
}

LinearZone::LinearZone()
{
	active = false;
}

void LinearZone::addGizmo()
{
	float length = 1000;
	glm::vec2 offset(length * cos(_rotation),length * sin(_rotation));
	glm::vec2 centre;
	glm::mat4 rotationMatrix;
	rotationMatrix = glm::rotate(rotationMatrix, _rotation, glm::vec3(0, 0, 1));
	Gizmos::add2DAABBFilled(_centre + offset, glm::vec2(length, length), glm::vec4(.5, 0, 0, .3), &rotationMatrix);
}

float LinearZone::checkRange(glm::vec2 testPoint)
{
	if (active)
	{
		glm::vec2 normal(-cos(_rotation), -sin(_rotation));
		glm::vec2 displacement = testPoint - _centre;
		float distance = glm::dot(normal, displacement);
		if (distance < 0)
			return 1;
		else
			return 0;

	}
	return 0;
}