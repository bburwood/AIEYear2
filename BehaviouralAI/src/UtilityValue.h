#ifndef UTILITYVALUE_
#define UTILITYVALUE_

namespace UtilitySystem
{

	class UtilityValue
	{

	public:
		enum NormalizationType
		{
			LINEAR,
			INVERSE_LINEAR,
			QUADRATIC,
			INVERSE_QUADRATIC
		};

		UtilityValue();
		UtilityValue(NormalizationType a_eType, float a_fMin, float a_fMax);
		~UtilityValue();

		void setMinMaxValues(float a_fMin, float a_fMax);

		void setNormalizationType(NormalizationType a_eType);
		void setQuadraticPower(float a_fPower);

		void setValue(float a_fValue);

		float evaluate();
	private:
		float m_fMin;
		float m_fMax;
		float m_fPower; //Only used when using Quadratic Normalization

		float m_fValue;
		float m_fNormalizedValue;

		NormalizationType m_eNormalizationType;
	};

}

#endif