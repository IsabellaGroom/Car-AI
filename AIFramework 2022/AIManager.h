#pragma once

#include "WaypointManager.h"

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

protected:
	bool	checkForCollisions();
	void	setRandomPickupPosition(PickupItem* pickup);

private:
	vecPickups              m_pickups;
	Vehicle*				m_pRedCar = nullptr;
	Vehicle*                m_pBlueCar = nullptr;
	WaypointManager			m_waypointManager;
	float time;
	Vector2D                m_RedCarPos;
	Vector2D                m_BlueCarPos;
	//TODO: add acceleration = velocity * time
};

