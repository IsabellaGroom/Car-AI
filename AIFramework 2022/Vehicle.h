#pragma once

#include "DrawableGameObject.h"
#include "WaypointManager.h"
#include "Waypoint.h"
#include "Vector2D.h"
#include "Collidable.h"
#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#define MAX_SPEED 200

enum class carColour
{
	redCar,
	blueCar,
};

enum State
{
	SEEK,
	FLEE,
	PATH,
	ARRIVE,
	STATIC,
};

class Vehicle : public DrawableGameObject, public Collidable
{
public:
	virtual HRESULT initMesh(ID3D11Device* pd3dDevice, carColour colour);
	virtual void update(const float deltaTime);

	void setMaxSpeed(const float maxSpeed) { m_maxSpeed = maxSpeed; }
	const float getMaxSpeed() { return m_maxSpeed; }
	void setCurrentSpeed(const float speed); // a ratio: a value between 0 and 1 (1 being max speed)
	void setPositionTo(Vector2D positionTo); // a position to move to
	float getCurrentFuel() { return m_fuel; }
	void setVehiclePosition(Vector2D position); // the current position - this resets positionTo
	void setWaypointManager(WaypointManager* wpm);
	void hasCollided() {}
	void seekPath();
	void setPath(std::list<Waypoint*> path);
	void IncreaseFuel() { m_fuel++; }
	void SpeedBoost() { m_currentSpeed += 100; }

	//states
	void Seek();
	void Flee();
	void Arrive();
	void Static();
	void Path();
	void StateManager(State desiredState);
	State GetCurrentState() { return m_currentState; };

	bool pathCompleted;

protected: // protected methods


protected: // preotected properties
	float m_maxSpeed;
	float m_currentSpeed;
	float m_fuel;
	float m_time;
	Vector2D m_velocity;
	float m_deltaTime;
	Vector2D m_acceleration;
	State m_currentState;
	bool m_isforce = true;
	Vector2D m_vecTo;
	
	Vector2D m_currentPosition;
	Vector2D m_startPosition;
	Vector2D m_positionTo;
	Vector2D m_lastPosition;
	WaypointManager* m_waypointManager;
	std::list<Waypoint*> m_path;

};

