#include "AIManager.h"
#include "main.h"
#include "Vehicle.h"
#include "DrawableGameObject.h"
#include "PickupItem.h"
#include "Waypoint.h"
#include "constants.h"
#include <queue>
#include "t_Array.h"
#include <functional>
#include <vector>
#include <utility>
#include <string>

AIManager::AIManager()
{
	m_pRedCar = nullptr;
    m_pBlueCar = nullptr;
}

AIManager::~AIManager()
{
	release();
}

void AIManager::release()
{
	clearDrawList();

	for (PickupItem* pu : m_pickups)
	{
		delete pu;
	}
	m_pickups.clear();

	delete m_pRedCar;
    delete m_pBlueCar;
	m_pRedCar = nullptr;
    m_pBlueCar = nullptr;
}

HRESULT AIManager::initialise(ID3D11Device* pd3dDevice)
{
    // create the vehicle 
    float xPos = -500; // an abtrirary start point
    float yPos = 300;

    m_pRedCar = new Vehicle();
    HRESULT hr = m_pRedCar->initMesh(pd3dDevice, carColour::redCar);
    m_pRedCar->setVehiclePosition(Vector2D(xPos, yPos));
    if (FAILED(hr))
        return hr;

    xPos = 500;
    yPos = -300;
    m_pBlueCar = new Vehicle();
    hr = m_pBlueCar->initMesh(pd3dDevice, carColour::blueCar);
    m_pRedCar->setVehiclePosition(Vector2D(xPos, yPos));
    if (FAILED(hr))
        return hr;

    speed = m_pBlueCar->getMaxSpeed();

    // setup the waypoints
    m_waypointManager.createWaypoints(pd3dDevice);
    m_pRedCar->setWaypointManager(&m_waypointManager);
    m_pBlueCar->setWaypointManager(&m_waypointManager);

    // create a passenger pickup item
    PickupItem* pPickupPassenger = new PickupItem();
    hr = pPickupPassenger->initMesh(pd3dDevice, pickuptype::Passenger);
    m_pickups.push_back(pPickupPassenger);

    // NOTE!! for fuel and speedboost - you will need to create these here yourself

    // (needs to be done after waypoint setup)
    setRandomPickupPosition(pPickupPassenger);

    return hr;
}


void AIManager::update(const float fDeltaTime)
{
    m_RedCarPos = m_pRedCar->getPosition();
    m_BlueCarPos = m_pBlueCar->getPosition();


    for (unsigned int i = 0; i < m_waypointManager.getWaypointCount(); i++) {
        m_waypointManager.getWaypoint(i)->update(fDeltaTime);
        //AddItemToDrawList(m_waypointManager.getWaypoint(i)); // if you uncomment this, it will display the waypoints
    }

    for (int i = 0; i < m_waypointManager.getQuadpointCount(); i++)
    {
        Waypoint* qp = m_waypointManager.getQuadpoint(i);
        qp->update(fDeltaTime);
        //AddItemToDrawList(qp); // if you uncomment this, it will display the quad waypoints
    }

    // update and display the pickups
    for (unsigned int i = 0; i < m_pickups.size(); i++) {
        m_pickups[i]->update(fDeltaTime);
        AddItemToDrawList(m_pickups[i]);
    }


	// draw the waypoints nearest to the car
	/*
    Waypoint* wp = m_waypointManager.getNearestWaypoint(m_pCar->getPosition());
	if (wp != nullptr)
	{
		vecWaypoints vwps = m_waypointManager.getNeighbouringWaypoints(wp);
		for (Waypoint* wp : vwps)
		{
			AddItemToDrawList(wp);
		}
	}
    */
    if (isArrive)
    {
        speed -= 1.0f;
        m_pBlueCar->setCurrentSpeed(speed);

        //add check to see if car has arrived
        //if so isArrive = false;
    }


    // update and draw the car (and check for pickup collisions)
	if (m_pRedCar != nullptr)
	{
		m_pRedCar->update(fDeltaTime);
		checkForCollisions();
		AddItemToDrawList(m_pRedCar);
	}

    if (m_pBlueCar != nullptr)
    {
        m_pBlueCar->update(fDeltaTime);
        checkForCollisions();
        AddItemToDrawList(m_pBlueCar);
    }
}

void AIManager::mouseUp(int x, int y)
{
	// get a waypoint near the mouse click, then set the car to move to the this waypoint
	Waypoint* wp = m_waypointManager.getNearestWaypoint(Vector2D(x, y));
	if (wp == nullptr)
		return;

 //   unordered_map<Waypoint*, Waypoint*> path = pathFinding(wp);
    // steering mode
   
    m_pRedCar->arriveTo(wp->getPosition());
}

void AIManager::keyUp(WPARAM param)
{
    const WPARAM key_a = 65;
    switch (param)
    {
        case key_a:
        {
            OutputDebugStringA("a Up \n");
            break;
        }
    }
}

void AIManager::keyDown(WPARAM param)
{
	// hint 65-90 are a-z 
	const WPARAM key_a = 65;
	const WPARAM key_s = 83;
    const WPARAM key_p = 80;
    const WPARAM key_f = 70;

    switch (param)
    {
        case VK_NUMPAD0:
        {
            OutputDebugStringA("0 pressed \n");
            break;
        }
        case VK_NUMPAD1:
        {
            OutputDebugStringA("1 pressed \n");
            break;
        }
        case key_s:
        {

            Waypoint* waypoint = m_waypointManager.getWaypoint(unsigned int(rand() % m_waypointManager.getWaypointCount() + 1));

            if (waypoint != nullptr /*&& hasArrived == true*/)
            {
                AddItemToDrawList(waypoint);
                m_pBlueCar->setPositionTo(waypoint->getPosition());
            }

            break;
        }
        case key_a:
        {
            Waypoint* waypoint = m_waypointManager.getWaypoint(unsigned int(rand() % m_waypointManager.getWaypointCount() + 1));

            if (waypoint != nullptr /*&& hasArrived == true*/)
            {
                AddItemToDrawList(waypoint);
                m_pBlueCar->setPositionTo(waypoint->getPosition());
                isArrive = true;
            }            
            break;
        }
		case key_p:
		{ 
            /*
            //Distance between two vectors.
            Vector2D v1(1, 0);
            Vector2D v2(0, 1);
            Vector2D v3 = v1 + v2;
            */
            //Pursuit
           /* attempt at force based seeking
             Vector2D vector = m_RedCarPos + m_BlueCarPos;
             m_pRedCar->setPositionTo(vector * m_pRedCar->getMaxSpeed());
           */
           
            std::unordered_map<Waypoint*, Waypoint*> came_from;
            std::list<Waypoint*> path;
            came_from = pathFinding(m_waypointManager.getNearestWaypoint(m_BlueCarPos));
            //list put goal in first
            Waypoint* traversal = came_from.at(m_waypointManager.getNearestWaypoint(m_BlueCarPos));
            while (came_from.at(traversal) != nullptr)
            {
                path.push_back(traversal);
                traversal = came_from.at(traversal);
            }
            
            while (true)
            {
                m_pRedCar->setPositionTo(path.back()->getPosition());
                path.pop_back();
            }


          //  m_pRedCar->setPositionTo(m_BlueCarPos);
            break;
		}
        case key_f:
		{
            //flee
            m_pRedCar->setPositionTo(m_RedCarPos - m_BlueCarPos);

            break;
        }
        case VK_SPACE:
        {
            m_pRedCar->setPositionTo(Vector2D(7, -8));
            break;
        }
        // etc
        default:
            break;
    }
}

void AIManager::setRandomPickupPosition(PickupItem* pickup)
{
    if (pickup == nullptr)
        return;

    int x = (rand() % SCREEN_WIDTH) - (SCREEN_WIDTH / 2);
    int y = (rand() % SCREEN_HEIGHT) - (SCREEN_HEIGHT / 2);

    Waypoint* wp = m_waypointManager.getNearestWaypoint(Vector2D(x, y));
    if (wp) {
        pickup->setPosition(wp->getPosition());
    }
}

/*
// IMPORTANT
// hello. This is hopefully the only time you are exposed to directx code 
// you shouldn't need to edit this, but marked in the code below is a section where you may need to add code to handle pickup collisions (speed boost / fuel)
// the code below does the following:
// gets the *first* pickup item "m_pickups[0]"
// does a collision test with it and the car
// creates a new random pickup position for that pickup

// the relevant #includes are already in place, but if you create your own collision class (or use this code anywhere else) 
// make sure you have the following:
#include <d3d11_1.h> // this has the appropriate directx structures / objects
#include <DirectXCollision.h> // this is the dx collision class helper
using namespace DirectX; // this means you don't need to put DirectX:: in front of objects like XMVECTOR and so on. 
*/

bool AIManager::checkForCollisions()
{
    if (m_pickups.size() == 0)
        return false;

    XMVECTOR dummy;

    // get the position and scale of the car and store in dx friendly xmvectors
    XMVECTOR carPos;
    XMVECTOR carScale;
    XMMatrixDecompose(
        &carScale,
        &dummy,
        &carPos,
        XMLoadFloat4x4(m_pRedCar->getTransform())
    );

    // create a bounding sphere for the car
    XMFLOAT3 scale;
    XMStoreFloat3(&scale, carScale);
    BoundingSphere boundingSphereCar;
    XMStoreFloat3(&boundingSphereCar.Center, carPos);
    boundingSphereCar.Radius = scale.x;

    // do the same for a pickup item
    // a pickup - !! NOTE it is only referring the first one in the list !!
    // to get the passenger, fuel or speedboost specifically you will need to iterate the pickups and test their type (getType()) - see the pickup class
    XMVECTOR puPos;
    XMVECTOR puScale;
    XMMatrixDecompose(
        &puScale,
        &dummy,
        &puPos,
        XMLoadFloat4x4(m_pickups[0]->getTransform())
    );

    // bounding sphere for pickup item
    XMStoreFloat3(&scale, puScale);
    BoundingSphere boundingSpherePU;
    XMStoreFloat3(&boundingSpherePU.Center, puPos);
    boundingSpherePU.Radius = scale.x;

	// THIS IS generally where you enter code to test each type of pickup
    // does the car bounding sphere collide with the pickup bounding sphere?
    if (boundingSphereCar.Intersects(boundingSpherePU))
    {
        OutputDebugStringA("Pickup collision\n");
        m_pickups[0]->hasCollided();
        setRandomPickupPosition(m_pickups[0]);

        // you will need to test the type of the pickup to decide on the behaviour
        // m_pCar->dosomething(); ...

        return true;
    }

    return false;
}

std::unordered_map<Waypoint*, Waypoint*> AIManager::pathFinding(Waypoint* goal)
{

    std::priority_queue<costing, std::vector<costing>, std::greater<costing>> frontier;
    Waypoint* start = m_waypointManager.getNearestWaypoint(m_RedCarPos);
    //Waypoint* start = start;
    frontier.push(std::make_pair(0, start));

    unordered_map<Waypoint*, Waypoint*> came_from;
    unordered_map<Waypoint*, float> costSoFar;


    while (!frontier.empty())
    {
        std::pair<int, Waypoint*> current = frontier.top();
        //frontier.pop();

        if (current.second == goal)
        {
            break;
        }


        for (Waypoint* neighbours : m_waypointManager.getNeighbouringWaypoints(current.second))
        {
            for (Waypoint* neighbours : m_waypointManager.getNeighbouringWaypoints(current.second))
            {
                float costToNeighbour = costSoFar.at(current.second);//+ costOfEdge
                if (came_from.find(neighbours) == came_from.end() || costToNeighbour < costSoFar.at(neighbours))
                {

                    double priority = costToNeighbour + heuristic(neighbours, goal);
                    costSoFar.insert(std::make_pair(neighbours, costToNeighbour));
                    came_from.insert(std::make_pair(neighbours, current.second));
                    frontier.push(std::make_pair(priority, neighbours));
                }
            }
        }
    }

  
    return came_from;
}

float AIManager::heuristic(Waypoint* w1, Waypoint* w2)
{
    return  abs(w1->getPosition().x - w2->getPosition().x) + abs(w1->getPosition().y - w2->getPosition().y);
}





