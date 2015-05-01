#include "FuzzyLogicEngine.h"


rightShoulderMembershipFunction::rightShoulderMembershipFunction(float x0,float x1,string name)
{
	_x0 = x0;
	_x1 = x1;
	_name = name;
}

float rightShoulderMembershipFunction::getMembership(float x)
{
	float result = 0;
	if(x <= _x0)
		result = 0;
	else if(x >= _x1)
		result = 1;
    else
		result = (x/(_x1-_x0))-(_x0/(_x1-_x0));
	return result;
}

float rightShoulderMembershipFunction::getMaxMembership()
{
	return (_x1+1)*.5f;
}

leftShoulderMembershipFunction::leftShoulderMembershipFunction(float x0,float x1,string name)
{
	_x0 = x0;
	_x1 = x1;
	_name = name;
}

float leftShoulderMembershipFunction::getMembership(float x)
{
	float result = 0;
	if(x <= _x0)
		result = 1;
	else if(x >= _x1)
		result = 0;
    else
		result = (-x/(_x1-_x0))+(_x1/(_x1-_x0));
	return result;
}

float leftShoulderMembershipFunction::getMaxMembership()
{
	return (_x0)*.5f;
}


TriangleFunction::TriangleFunction(float x0,float x1,float x2,string name)
{
	_x0 = x0;
	_x1 = x1;
	_x2 = x2;
	_name = name;
}

float TriangleFunction::getMembership(float x)
{
	float result = 0;
	if(x <= _x0 || x >= _x2)
		result = 0;
	else if(x == _x1)
		result = 1;
	else if((x>_x0) && (x<_x1))
		result = (x/(_x1-_x0))-(_x0/(_x1-_x0));
	else
		result = (-x/(_x2-_x1))+(_x2/(_x2-_x1));
	return result;
}

float TriangleFunction::getMaxMembership()
{
	return _x1;
}

TrapezoidFunction::TrapezoidFunction(float x0,float x1,float x2,float x3,string name)
{
	_x0 = x0;
	_x1 = x1;
	_x2 = x2;
	_x3 = x3;
	_name = name;
}

float TrapezoidFunction::getMaxMembership()
{
	return (_x1+_x2)*.5f;
}

float TrapezoidFunction::getMembership(float x)
{
	float result = 0;
	if(x <= _x0 || x >= _x3)
		result = 0;
	else if((x>=_x1) && (x<=_x2))
		result = 1;
	else if((x>_x0) && (x<_x1))
		result = (x/(_x1-_x0))-(_x0/(_x1-_x0));
	else
		result = (-x/(_x3-_x2))+(_x3/(_x3-_x2));
	return result;
}


float Fuzzy::AND(float f1,float f2)
{
	if(f1<f2)
	{
		return f1;
	}
	else
	{
		return f2;
	}
}

float Fuzzy::OR(float f1,float f2)
{
	if(f1>f2)
	{
		return f1;
	}
	else
	{
		return f2;
	}
}

float Fuzzy::NOT(float f1)
{
	return 1 - f1;
}