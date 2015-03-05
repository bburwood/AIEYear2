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
	glm::vec3 min, max;
};

#endif	//	_AABB_H_
