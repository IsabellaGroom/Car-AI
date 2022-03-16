#include "Vehicle.h"

#define NORMAL_MAX_SPEED 200
#define DECCELERATION 10

HRESULT	Vehicle::initMesh(ID3D11Device* pd3dDevice, carColour colour)
{
	m_scale = XMFLOAT3(30, 20, 1);

	if (colour == carColour::redCar)
	{
		setTextureName(L"Resources\\car_red.dds");
	}
	else if (colour == carColour::blueCar)
	{
		setTextureName(L"Resources\\car_blue.dds");
	}

	HRESULT hr = DrawableGameObject::initMesh(pd3dDevice);

	m_maxSpeed = NORMAL_MAX_SPEED;
	setMaxSpeed(MAX_SPEED);
	m_currentSpeed = m_maxSpeed;
	setVehiclePosition(Vector2D(0, 0));

	m_lastPosition = Vector2D(0, 0);
	m_deltaTime = 0.0f;
	return hr;
}

void Vehicle::update(const float deltaTime)
{
	m_deltaTime = deltaTime;
	// consider replacing with force based acceleration / velocity calculations
	Vector2D vecTo = m_positionTo - m_currentPosition;
	m_velocity = deltaTime * m_currentSpeed;

	float length = (float)vecTo.Length();
	// if the distance to the end point is less than the car would move, then only move that distance. 
	if (length > 0) {
		vecTo.Normalize();
		if(length > m_velocity)
			vecTo *= m_velocity;
		else
			vecTo *= length;

		m_currentPosition += vecTo;
	}

	// rotate the object based on its last & current position
	Vector2D diff = m_currentPosition - m_lastPosition;
	if (diff.Length() > 0) { // if zero then don't update rotation
		diff.Normalize();
		m_radianRotation = atan2f((float)diff.y, (float)diff.x); // this is used by DrawableGameObject to set the rotation
	}
	m_lastPosition = m_currentPosition;

	// set the current poistion for the drawablegameobject
	setPosition(Vector2D(m_currentPosition));
	DrawableGameObject::update(deltaTime);

	if (!m_path.empty())
	{
		seekPath();
	}

}


// a ratio: a value between 0 and 1 (1 being max speed)
void Vehicle::setCurrentSpeed(const float speed)
{
	m_currentSpeed = m_maxSpeed * speed;
	m_currentSpeed = max(0, m_currentSpeed);
	m_currentSpeed = min(m_maxSpeed, m_currentSpeed);
}

// set a position to move to
void Vehicle::setPositionTo(Vector2D position)
{
	m_startPosition = m_currentPosition;
	
	m_positionTo = position;
}

// set the current position
void Vehicle::setVehiclePosition(Vector2D position)
{
	m_currentPosition = position;
	m_positionTo = position;
	m_startPosition = position;
	setPosition(position);
}

void Vehicle::setWaypointManager(WaypointManager* wpm)
{
	m_waypointManager = wpm;
}

void Vehicle::arriveTo(Vector2D arrivalPoint)
{
	//TODO: slow down upon arrival
	m_startPosition = m_currentPosition;

	double length = arrivalPoint.Length();
	float speed = length * DECCELERATION;
	m_velocity = m_velocity * speed;
	//Vector2D vector = ((arrivalPoint * speed) / length)
	m_positionTo = arrivalPoint;
}

void Vehicle::seekPath()
{
		setPositionTo(m_path.back()->getPosition());
		if (m_positionTo.Distance(m_currentPosition) < 2.0)
		{
			m_path.pop_back();
		}
}

void Vehicle::setPath(std::list<Waypoint*> path)
{
	m_path = path;
}
