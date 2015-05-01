#include "Turret.h"

Turret::Turret(glm::vec2 centre, float range)
{
	_centre = centre;
	_range = range;
	active = true;
}
Turret::~Turret()
{
}

Turret::Turret()
{
	active = false;
}

void Turret::addGizmo()
{
	if (active)
	{
		Gizmos::add2DCircle(_centre, _range, 20, glm::vec4(.5, 0, 0, .3));
	}
}

float Turret::checkRange(glm::vec2 testPoint)
{
	if (active)
	{
		glm::vec2 displacment = testPoint - _centre;
		float distance = glm::length(displacment);
		if (distance < _range)
		{
			return 1;
		}
	}
	return 0;
}