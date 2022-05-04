#pragma once

#include "WaypointManager.h"
#include <unordered_map>

using namespace std;

class Vehicle;
class PickupItem;
typedef vector<PickupItem*> vecPickups;

class AIManager
{
public:
	AIManager();
	virtual  ~AIManager();
	void	release();
	HRESULT initialise(ID3D11Device* pd3dDevice);
	void	update(const float fDeltaTime);
	void	mouseUp(int x, int y);
	void	keyDown(WPARAM param);
	void	keyUp(WPARAM param);
	std::unordered_map<Waypoint*, Waypoint*> pathFinding(Waypoint* goal);
	float heuristic(Waypoint* w1, Waypoint* w2);
	float time;
protected:
	bool	checkForCollisions(Vehicle* car);
	void	setRandomPickupPosition(PickupItem* pickup);

private:
	vecPickups              m_pickups;
	Vehicle*				m_pRedCar = nullptr;
	Vehicle*                m_pBlueCar = nullptr;
	WaypointManager			m_waypointManager;
	float speed;
	bool isWander; 
	bool isFlee;
	bool isStrategy;
	bool isPath;
	Vector2D                m_RedCarPos;
	Vector2D                m_BlueCarPos;
	//TODO: add acceleration = velocity * time

	typedef std::pair<float, Waypoint*> costing;
};

