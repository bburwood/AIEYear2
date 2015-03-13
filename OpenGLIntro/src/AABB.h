#ifndef _AABB_H_
#define _AABB_H_

#include "GLMHeader.h"
#include <vector>

class AABB
{
public:
	AABB()
	{
		reset();
	}
	~AABB() {}
	void reset()
	{
		min.x = min.y = min.z = 1e37f;
		max.x = max.y = max.z = -1e37f;
	}
	void fit(const std::vector<glm::vec3>& points)
	{
		for (auto& p : points)
		{
			if (p.x < min.x)
				min.x = p.x;
			if (p.y < min.y)
				min.y = p.y;
			if (p.z < min.z)
				min.z = p.z;
			if (p.x > max.x)
				max.x = p.x;
			if (p.y > max.y)
				max.y = p.y;
			if (p.z > max.z)
				max.z = p.z;
		}
	}
	void	GenerateAABB(vec3* a_positions, unsigned int a_count, unsigned int a_stride = 0)
	{
		//	more general AABB generation to allow for vertex buffer data to be used as well
		reset();
		if (a_stride == 0)
		{
			a_stride = sizeof(vec3);
		}
		for (unsigned int i = 0; i < a_count; ++i)
		{
			if (a_positions->x < min.x)	min.x = a_positions->x;
			if (a_positions->y < min.y)	min.y = a_positions->y;
			if (a_positions->z < min.z)	min.z = a_positions->z;

			if (a_positions->x > max.x)	max.x = a_positions->x;
			if (a_positions->y > max.y)	max.y = a_positions->y;
			if (a_positions->z > max.z)	max.z = a_positions->z;

//			a_positions = (vec3)((char*)a_positions + a_stride);
		}
	}
	glm::vec3 min, max;
};

#endif	//	_AABB_H_
