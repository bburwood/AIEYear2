#ifndef UTILITY_MATH_H_
#define UTILITY_MATH_H_

#include <algorithm>

namespace UtilitySystem
{
	namespace UtilityMath
	{

		static float LinearNormalise(float fMin, float fMax, float fValue)
		{
			float fClampedValue = std::max(std::min(fMax, fValue), fMin);
			float fRange = fMax - fMin;
			return (fClampedValue - fMin) / (fRange);
		}

		static float QuadraticNormalise(float fMin, float fMax, float fValue, float fPower)
		{
			float fClampedValue = std::max(std::min(fMax, fValue), fMin);

			return pow(fClampedValue, fPower) / pow(fMax, fPower);
		}

	}
}

#endif