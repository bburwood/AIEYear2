#include "SimulationObjects.h"
#include "FuzzyLogicEngine.h"
#include <iostream>

extern Fuzzy fuzzyEngine;

void WorldObject::draw()
{
	int segments = 20;
	Gizmos::add2DCircle(position, radius,segments, colour);
}

BaseAgent::BaseAgent(glm::vec2 position, glm::vec4 colour, float radius)
{
	this->position = position;
	this->colour = colour;
	this->radius = radius;
}


BaseResource::BaseResource(glm::vec2 position,glm::vec4 colour,float radius)
{
	this->position = position;
	this->colour = colour;
	this->radius = radius;
}

Water::Water(glm::vec2 position):BaseResource(position,glm::vec4(0,0,1,1),20)
{
	type = WATER;
}

Food::Food(glm::vec2 position):BaseResource(position,glm::vec4(0,0.8f,0,1),20)
{
	type = FOOD;
}

Cave::Cave(glm::vec2 position):BaseResource(position,glm::vec4(0,0,0,1),20)
{
	type = CAVE;
}

Agent::Agent(glm::vec2 position) : BaseAgent(position, glm::vec4(1, 1, 0, 1), 22)
{
	tiredness = 1;
	food = 1;
	water = 1;
	type = SIMPLE_AI;
	maxSpeed = 450;
};

void Agent::drawBar(float value, int index)
{
	glm::vec4 colours[] = {glm::vec4(1,0,0,1),glm::vec4(0,1,0,1),glm::vec4(0,0,1,1)};
	glm::vec2 leftPos(10,700);
	float barLength = 90;
	float barHeight = 10;
	glm::vec4 white(1,1,1,1);
	glm::vec4 back(1, 1, 1, .25f);
	glm::vec4 colour(.3,.3,.3,1);
	glm::vec2 extents(barLength,barHeight);
	glm::vec2 centrePos = leftPos;
	centrePos.y -= barHeight * 3 * index;
	centrePos.x += barLength;
	Gizmos::add2DAABBFilled(centrePos, extents, back);
	barLength*=value;
	extents = glm::vec2(barLength,barHeight);
	centrePos = leftPos;
	centrePos.y -= barHeight * 3 * index;
	centrePos.x += barLength;
	colour = colours[index];
	Gizmos::add2DAABBFilled(centrePos, extents, colour);
}

float BaseAgent::findNearestResource(WorldObjectType type)
{
	float minDistance = 1000000;
	for(auto object:*worldObjects)
	{
		if(object->type == type)
		{
			float distance = glm::length(object->position - position);
			if(distance < minDistance)
			{
				minDistance = distance;
			}
		}
	}
	return minDistance;
}

glm::vec2 BaseAgent::findResourceVector(WorldObjectType type)
{
	float minDistance = 1000000;
	glm::vec2 target;
	for(auto object:*worldObjects)
	{
		if(object->type == type)
		{
			float distance = glm::length(object->position - position);
			if(distance < minDistance)
			{
				minDistance = distance;
				target = object->position;
			}
		}
	}
	glm::vec2 vector;
	if(minDistance<1)
	{
		vector = glm::vec2(0,0);
	}
	else
	{
		vector = (target - position)/minDistance;
	}
	return vector;
}

void Agent::draw()
{
	BaseAgent::draw();
	drawBar(tiredness,0);
	drawBar(food,1);
	drawBar(water,2);
}



float Agent::checkEatingDesirable()
{
	float desire = 0; //this is the return value which will be how much
	//our AI desires food
	float foodRange = findNearestResource(FOOD);
	//how far is the nearest food source
	//how hungry are we...
	float hungry = fuzzyEngine.hungry->getMembership(food);	
	//how full are we...
	float full = fuzzyEngine.full->getMembership(food);
	//are we very hungry?
	float veryHungry  = fuzzyEngine.veryHungry->getMembership(food);
	//how close are we to food...
	float foodRangeClose = fuzzyEngine.veryNear->getMembership(foodRange);
	float foodRangeMedium = fuzzyEngine.mediumRange->getMembership(foodRange);
	float foodRangeFar = fuzzyEngine.farAway->getMembership(foodRange);
	//is this a very desirable action?
	float veryDesirableValue = Fuzzy::OR(Fuzzy::AND(foodRangeClose,hungry),veryHungry);
	//is it a desirable action?
	float desirableValue = Fuzzy::AND(Fuzzy::NOT(foodRangeFar),hungry);
	//is it undesirable?  In this case if we are full it's undesirable
	float undesirableValue = full;
	//set up our maximum values ready to defuzzify
	float maxVeryDesirable = fuzzyEngine.veryDesirable->getMaxMembership();
	float maxDesirable = fuzzyEngine.desirable->getMaxMembership();
	float maxUndesirable = fuzzyEngine.undesirable->getMaxMembership();
	//defuzzify
	desire = maxVeryDesirable*veryDesirableValue + maxDesirable* desirableValue + maxUndesirable*undesirableValue;
	desire /= (.1f + veryDesirableValue + desirableValue + undesirableValue);
	//return our final desire
	//cout << desire << endl;
	return desire;
}

float Agent::checkSleepDesirable()
{
	float desire = 0; //this is the return value which will be how much
	//our AI desires food
	float caveRange = findNearestResource(CAVE);
	//how far is the nearest food source
	//how hungry are we...
	float tired = fuzzyEngine.tired->getMembership(tiredness);	
	//how full are we...
	float active = fuzzyEngine.superActive->getMembership(tiredness);
	//are we very hungry?
	float awake  = fuzzyEngine.awake->getMembership(tiredness);
	//how close are we to food...
	float caveClose = fuzzyEngine.veryNear->getMembership(caveRange);
	float caveMedium = fuzzyEngine.mediumRange->getMembership(caveRange);
	float caveFar = fuzzyEngine.farAway->getMembership(caveRange);
	//is this a very desirable action?
	float veryDesirableValue = Fuzzy::OR(Fuzzy::AND(caveClose,awake),tired);
	//is it a desirable action?
	float desirableValue = Fuzzy::AND(Fuzzy::NOT(caveFar),tired);
	//is in undesirable?  In this case if we are full it's undesirable
	float undesirableValue = active;
	//set up our maximum values readt to defuzzify
	float maxVeryDesirable = fuzzyEngine.veryDesirable->getMaxMembership();
	float maxDesirable = fuzzyEngine.desirable->getMaxMembership();
	float maxUndesirable = fuzzyEngine.undesirable->getMaxMembership();
	//defuzzify
	desire = maxVeryDesirable*veryDesirableValue + maxDesirable* desirableValue + maxUndesirable*undesirableValue;
	desire /= (.1f + veryDesirableValue + desirableValue + undesirableValue);
	//return our final desire
	//cout << desire << endl;
	return desire;
}

float Agent::checkDrinkingDesirable()
{
	float desire = 0; //this is the return value which will be how much
	//our AI desires food
	float waterRange = findNearestResource(WATER);
	//how far is the nearest food source
	//how hungry are we...
	float thirsty = fuzzyEngine.thirsty->getMembership(water);	
	//how full are we...
	float notThirsty = fuzzyEngine.notThirsty->getMembership(water);
	//are we very hungry?
	float veryThirsty = fuzzyEngine.veryThirsty->getMembership(water);
	float weekFromThirst = fuzzyEngine.WeekFromThirsty->getMembership(water);
	//how close are we to food...
	float waterRangeClose = fuzzyEngine.veryNear->getMembership(waterRange);
	float waterRangeMedium = fuzzyEngine.mediumRange->getMembership(waterRange);
	float waterRangeFar = fuzzyEngine.farAway->getMembership(waterRange);
	//is this a very desirable action?
	float veryDesirableValue = Fuzzy::OR(Fuzzy::AND(waterRangeClose,thirsty),veryThirsty);
	veryDesirableValue= Fuzzy::OR(veryDesirableValue,weekFromThirst);
	//is it a desirable action?
	float desirableValue = Fuzzy::AND(Fuzzy::NOT(waterRangeClose),thirsty);
	//is in undesirable?  In this case if we are full it's undesirable
	float undesirableValue = notThirsty;
	//set up our maximum values readt to defuzzify
	float maxVeryDesirable = fuzzyEngine.veryDesirable->getMaxMembership();
	float maxDesirable = fuzzyEngine.desirable->getMaxMembership();
	float maxUndesirable = fuzzyEngine.undesirable->getMaxMembership();
	//defuzzify
	desire = maxVeryDesirable*veryDesirableValue + maxDesirable* desirableValue + maxUndesirable*undesirableValue;
	desire /= (.1f + veryDesirableValue + desirableValue + undesirableValue);
	//return our final desire
	return desire;

}

glm::vec2 Agent::gotoFood(float desirability, float delta)
{
	glm::vec2 velocity = findResourceVector(FOOD) * delta * (1+desirability)*maxSpeed;
	return velocity;
}

glm::vec2 Agent::gotoCave(float desirability, float delta)
{
	glm::vec2 velocity = findResourceVector(CAVE) * delta * (1+desirability)*maxSpeed;
	return velocity;
}

glm::vec2 Agent::gotoWater(float desirability, float delta)
{
	glm::vec2 velocity = findResourceVector(WATER) * delta * (1+desirability) * maxSpeed;
	return velocity;
}

void Agent::update(float delta)
{
	float eatDesirability = checkEatingDesirable();
	float sleepDesirability = checkSleepDesirable();
	float drinkDesirability = checkDrinkingDesirable();
	glm::vec2 velocity;
	if(eatDesirability>sleepDesirability && eatDesirability>drinkDesirability)
	{
		velocity= gotoFood(eatDesirability,delta);
	}
	else if(sleepDesirability>drinkDesirability)
	{
		velocity = gotoCave(sleepDesirability,delta);
	}
	else
	{
		velocity = gotoWater(drinkDesirability,delta);
	}
	position += velocity;
	//if we are near water then drink
	if(findNearestResource(WATER) <2)
	{
		water  += delta * 0.5f;
	}
	//else we are using water
	else
	{
		water -= delta *.05f;
	}
	//clamp water
	if(water <0)
	{
		water = 0;
	}
	if(water >1)
	{
		water = 1;
	}
	//if we are near food then eat
	if(findNearestResource(FOOD) <2)
	{
		food += delta * .2f;
	}
	//else we get hungrier
	else
	{
		food -= delta * .03f;
	}
	//clamp food
	if(food <0)
	{
		food = 0;
	}
	if(food > 1)
	{
		food = 1;
	}
	//if we are near cave then sleep
	if(findNearestResource(CAVE) <2)
	{
		tiredness += delta * .3f;
	}
	//else we get sleepier
	else
	{
		tiredness -= delta * .02f;
	}
	//clamp tiredness
	if(tiredness <0)
	{
		tiredness = 0;
	}
	if(tiredness >1)
	{
		tiredness = 1;
	}
}

void WorldController::update(float delta)
{
	for(auto worldObject:worldObjects)
	{
		worldObject->update(delta);
	}
}

void WorldController::draw()
{
	for(auto worldObject:worldObjects)
	{
		worldObject->draw();
	}
}

void WorldController::addObject(WorldObject* newObject)
{
	newObject->worldObjects = &worldObjects;
	worldObjects.push_back(newObject);
}