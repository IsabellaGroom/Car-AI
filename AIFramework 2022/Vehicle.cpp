#include "Vehicle.h"

#define NORMAL_MAX_SPEED 200
#define DECCELERATION 5

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
	//acceleration = force / mass
	//velocity += acceleration * deltatime
	//velocity = change in space/ change in time
	//position += velocity * deltatime
	m_deltaTime = deltaTime;
	m_acceleration = Vector2D(0, 0);
	m_vecTo = m_positionTo - m_currentPosition;
	
	StateManager(m_currentState);
	float length = (float)m_vecTo.Length();
	// if the distance to the end point is less than the car would move, then only move that distance. 
	if (length > 0) {
		m_vecTo.Normalize();
		if(length > m_velocity.Length())
			m_vecTo *= m_velocity;
		else
			m_vecTo *= length;

		if (m_isforce)
		{
			m_currentPosition += m_velocity;
		}
		else
		{
			m_currentPosition += m_vecTo;
		}

	
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

void Vehicle::ArriveTo(Vector2D arrivalPoint)
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

void Vehicle::StateManager(State desiredState)
{
	switch (desiredState)
	{
	case SEEK:
		m_currentState = SEEK;
		m_isforce = true;
		Seek();
		break;
	case FLEE:
		m_currentState = FLEE;
		m_isforce = true;
		Flee();
		break;
	case PATH:
		m_currentState = PATH;
		m_isforce = false;
		tempPath();
		break;
	case ARRIVE:
		m_currentState = ARRIVE;
		m_isforce = true;
		Arrive();
		break;
	}
}

void Vehicle::Seek()
{
    m_vecTo = m_positionTo - m_currentPosition;
	Vector2D force;

	m_vecTo.Normalize();
	Vector2D desiredVelocity = m_vecTo * m_maxSpeed;
	force = desiredVelocity - m_velocity;

	m_acceleration = force / 100.0f;
	m_velocity += m_deltaTime * m_acceleration;

}

void Vehicle::Flee()
{

    m_vecTo = m_positionTo - m_currentPosition;
	Vector2D force;

	m_vecTo.Normalize();
	Vector2D desiredVelocity = m_vecTo * m_maxSpeed;
	force = desiredVelocity - m_velocity;

	force = force.GetReverse();

	m_acceleration = force / 100.0f;
	m_velocity += m_deltaTime * m_acceleration;
}

void Vehicle::tempPath()
{
	Vector2D vecTo = m_positionTo - m_currentPosition;
	m_velocity.x += m_deltaTime * m_currentSpeed;
	m_velocity.y += m_deltaTime * m_currentSpeed;
}

void Vehicle::Arrive()
{
	Vector2D force;
	m_vecTo = m_positionTo - m_currentPosition;
	double magnitude = m_vecTo.Length();
    float speed = m_maxSpeed * (magnitude / 10.0f);
	

	m_vecTo.Normalize();
	Vector2D desiredVelocity = m_vecTo * speed;
	desiredVelocity = desiredVelocity / magnitude;

	if (magnitude < 10.0f)
	{
		desiredVelocity = Vector2D(0, 0);
	}
	desiredVelocity.Normalize();
	force = desiredVelocity - m_velocity;

	m_acceleration = force / 50.0f;
	m_acceleration = m_acceleration * 200.0f;
	m_velocity += m_deltaTime * m_acceleration;
	
}