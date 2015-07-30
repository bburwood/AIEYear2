#ifndef _MYCONTROLLER_H_
#define _MYCONTROLLER_H_

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>
using namespace physx;

class MyControllerHitReport : public PxUserControllerHitReport
{
public:
	MyControllerHitReport() : PxUserControllerHitReport(){};
	~MyControllerHitReport();
	//	overload the onShapeHit function
	virtual void	onShapeHit(const PxControllerShapeHit &hit);
	//	other collision functions which must be overloaded
	//	these handle collision with other controllers and hitting obstacles
	virtual void	onControllerHit(const PxControllersHit &hit){};
	//	called when current controller hits another controller.  More ...
	virtual void	onObstacleHit(const PxControllerObstacleHit &hit){};
	//	called when current controller hits a user-defined obstacle

	PxVec3	getPlayerContactNormal(){ return _playerContactNormal; };
	void	clearPlayerContactNormal(){ _playerContactNormal = PxVec3(0, 0, 0); };
	PxVec3	_playerContactNormal;

private:

};

#endif	//	_MYCONTROLLER_H_
