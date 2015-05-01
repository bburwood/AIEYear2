#include <vector>
#include <string>
using namespace std;

class MembershipFunction
{
public:
	float virtual getMembership(float value) = 0;
	float virtual getMaxMembership() = 0;
};

class rightShoulderMembershipFunction:public MembershipFunction
{
	float _x0,_x1;
	string _name;
public:
	rightShoulderMembershipFunction(float x0,float x1,string name);
	float virtual getMembership(float value) ;
	float virtual getMaxMembership();
};

class leftShoulderMembershipFunction:public MembershipFunction
{
	float _x0,_x1;
	string _name;
public:
	leftShoulderMembershipFunction(float x0,float x1,string name);
	float virtual getMembership(float value) ;
	float virtual getMaxMembership();
};

class TriangleFunction:public MembershipFunction
{
	float _x0,_x1,_x2;
	string _name;
public:
	TriangleFunction(float x0,float x1,float x2,string name);
	float virtual getMembership(float value) ;
	float virtual getMaxMembership();
};

class TrapezoidFunction:public MembershipFunction
{
	float _x0,_x1,_x2,_x3;
	string _name;
public:
	TrapezoidFunction(float x0,float x1,float x2,float x3,string name);
	float virtual getMembership(float value) ;
	float virtual getMaxMembership();
};


class Fuzzy
{
public:
	Fuzzy(){};
	//membership functions

	leftShoulderMembershipFunction* tired;
	TrapezoidFunction* awake;
	rightShoulderMembershipFunction* superActive;


	leftShoulderMembershipFunction* veryHungry;
	TrapezoidFunction* hungry;
	rightShoulderMembershipFunction* full;

	leftShoulderMembershipFunction* WeekFromThirsty;
	TriangleFunction* veryThirsty;
	TriangleFunction* thirsty;
	rightShoulderMembershipFunction* notThirsty;

	leftShoulderMembershipFunction* veryNear;
	TrapezoidFunction* mediumRange;
	rightShoulderMembershipFunction* farAway;

	leftShoulderMembershipFunction* undesirable;
	TriangleFunction* desirable;
	rightShoulderMembershipFunction* veryDesirable;

	//fuzzy operators
	static float AND(float f1,float f2);
	static float OR(float f1,float f2);
	static float NOT(float f1);
};