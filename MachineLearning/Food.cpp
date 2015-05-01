#include "Food.h"

Food::Food(glm::vec2 centre, float range)
{
	_centre = centre;
	_range = range;
	active = true;
}
Food::~Food()
{
}

Food::Food()
{
	active = false;
}

void Food::addGizmo()
{
	if (active)
	{
		Gizmos::add2DCircle(_centre, _range, 20, glm::vec4(0, 1, 0, .25f));
	}
}

float Food::checkRange(glm::vec2 testPoint)
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