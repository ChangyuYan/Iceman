#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

//**********************************************************************************
// Actor Class
//**********************************************************************************
class Actor: public GraphObject
{
public:
	Actor(StudentWorld* sw, std::string name, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth = 0):GraphObject(imageID, startX, startY, dir, size, depth)
	{
		m_className = name;
		m_alive = true;
		m_sw = sw;
		m_annoyed = false;
		m_leaveTheOilField = false;
	}
	virtual void doSomething() = 0;
	virtual ~Actor(){}
	
	bool isAlive();
	void setDead();
	void setHitPoints(int points);
	void decreaseHitPoints(int byHowMany);
	int  getHitPoints();
	void zeroHitPoints();
	void ifMoveAhead(int&x, int&y);
	bool moreThanXUnitsAway(Actor* a1, Actor* a2, int x_units);
	bool lessThanOrEqualToXUnitsAway(Actor* a1, Actor* a2, int x_units);
	void moveForwardOneStep();
	bool hasIceInHumanDirection(Direction dir);
	bool hasBlouderSpaceInHumanDirection(Direction dir);
	bool isEdgeInHumanDirection(Direction dir);
	bool isAnnoyed();
	void makeAnnoyed();
	void resetAnnoyed();
	void makeLeave();
	bool isLeaving();
	std::string getname();
	StudentWorld* getWorld();

private:
	int m_alive;
	int m_hitPoints;
	bool m_annoyed;
	std::string m_className;
	StudentWorld* m_sw;
	bool m_leaveTheOilField;
};

//**********************************************************************************
// Ice Class
//**********************************************************************************
class Ice: public Actor
{
public:
	Ice(StudentWorld* sw, int x, int y):Actor(sw, "Ice", IID_ICE, x, y, Actor::right, 0.25, 3)
	{
		GraphObject::setVisible(true);
	}
	void doSomething(){}
	~Ice(){}
private:

};

//**********************************************************************************
// Iceman Class
//**********************************************************************************
class Iceman: public Actor
{
public:
	Iceman(StudentWorld* sw):Actor(sw, "Iceman", IID_PLAYER, 30, 60, Actor::right, 1, 0)
	{
		GraphObject::setVisible(true);
		m_water = 5; // CHANGE BACK
		m_sonar = 1;
		m_gold = 0;
		m_score = 0;
		setHitPoints(10); 
	}
	void doSomething();
	void fireWater();
	void addWater(int amount);
	void addGold(); 
	int  getGold();
	int  getWater();
	void addScore(int amount);
	void addSonar();
	int  getSonar();
	int  getScore();
	int  SquirtX();
	int  SquirtY();
	bool notWithinBoulderRadius();
	~Iceman(){}
	
private:
	int m_water;
	int m_sonar;
	int m_gold;
	int m_score;

};

//**********************************************************************************
// Squirt Class
//**********************************************************************************

class Squirt: public Actor
{
public:
	Squirt(Iceman* player):Actor(player->getWorld(), "Squirt", IID_WATER_SPURT, player->SquirtX(), player->SquirtY(), player->getDirection(), 1, 1)
	{
		m_distance = 4;
		GraphObject::setVisible(true);
	}
	void doSomething();
	~Squirt(){}
private:
	int m_distance;
};

//**********************************************************************************
// Sonar Class
//**********************************************************************************

class Sonar: public Actor
{
public:
	Sonar(int lifeTime, StudentWorld* sw):Actor(sw, "Sonar", IID_SONAR, 0, 60, Actor::right, 1, 2)
	{
		GraphObject::setVisible(true);
		m_lifeTime = lifeTime; 
	}
	void doSomething();
	~Sonar(){}
	
private:
	int m_lifeTime;
};

//**********************************************************************************
// Boulder Class
//**********************************************************************************

class Boulder: public Actor
{
public:
	Boulder(StudentWorld* sw, int x, int y):Actor(sw, "Boulder", IID_BOULDER, x, y, Actor::down, 1, 1)
	{
		GraphObject::setVisible(true);
		m_state = "STABLE";
		m_waitingTicks = 0;
	
	}
	void doSomething();
	~Boulder(){}
	
	bool checkIfStable();
	bool hasBoulderBelow();
	bool hitStuff();
	bool hitHuman(); 
	
private:
	std::string m_state;
	int  m_waitingTicks;
	
};

//**********************************************************************************
// Oil Class
//**********************************************************************************
class Oil: public Actor
{
public:
	Oil(StudentWorld* sw, int x, int y):Actor(sw, "Oil", IID_BARREL, x, y, Actor::right, 1, 2)
	{
		GraphObject::setVisible(false);
	}
	void doSomething();
	~Oil(){}
};

//**********************************************************************************
// Gold Class
//**********************************************************************************
class Gold: public Actor
{
public:
	Gold(StudentWorld* sw, int x, int y, char state, bool visible, bool icemanPickupable, bool protesterPickupable): Actor(sw, "Gold", IID_GOLD, x, y, Actor::right, 1, 2)
	{
		GraphObject::setVisible(visible);
		m_state = state; // when initialized, should give the parameter either 'P' or 'T'
		m_pickableByIceman = icemanPickupable;
		m_pickableByProtecter = protesterPickupable;
		if (state == 'T')
		{
			m_lifetime = 100; 
		}
		
	}
	void doSomething();
	~Gold(){}
	
private:
	char m_state;
	bool m_pickableByIceman;
	bool m_pickableByProtecter;
	int  m_lifetime;
};

//**********************************************************************************
// Pool Class
//**********************************************************************************
class Pool: public Actor
{
public:
	Pool(StudentWorld* sw, int x, int y, int lifetime):Actor(sw, "Pool", IID_WATER_POOL, x, y, Actor::right, 1, 2)
	{
		GraphObject::setVisible(true);
		m_lifetime = lifetime;
	}
	
	void doSomething();
	~Pool(){}
	
private:
	int m_lifetime;
};

//**********************************************************************************
// Protester Base Class
//**********************************************************************************
class Protester: public Actor
{
public:
	Protester(StudentWorld* sw, std::string type, int image, int tickPeriod):Actor(sw, type, image, 60, 60, Actor::left, 1, 0)
	{ // CHANGE THE LOCATION BACK! 
		m_numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
		setVisible(true);
		m_stateCount = tickPeriod;
		m_noShoutTicks = 0;
		m_noPerpendicularTurnTicks = 0;
		AnnoyedTicks = 0; 
	}
	virtual void doSomething() = 0;
	virtual   ~Protester(){}
	int       getStateCount();
	void      updateStateCount();
	void      resetStateCount(); 
	void      moveOneStepToExit();
	bool      isFacingIceman();
	int       getNoShoutTicks();
	void      updateNoShoutTicks();
	int		  getNoPerpendicularTurnTicks();
	void      updateNoPerpendicularTurnTicks(); 
	bool      onTheSameLineAsIceman();
	bool      noBlocksOnPathToIceman();
	void      ZeroNumSquaresToMoveInCurrentDirection();
	void      DecrementNumSquaresToMoveInCurrentDirection();
	void      PickANewValueForNumSquaresToMoveInCurrentDirection();
	int       getNumSquaresToMoveInCurrentDirection();
	void      randomlyChangeDirection();
	bool      cannotMoveAhead();
	bool      isAtIntersection();
	bool	  pickUpGoldIfThereIsOne();
	Direction pickAPerpendicularDirection(); 
	Direction DirectionToIceman();
	int       AnnoyedTicks;
	void      moveSouth();
	void	  moveNorth();
	void	  moveEast();
	void      moveWest(); 
	
private:
	int  m_stateCount;
	int  m_numSquaresToMoveInCurrentDirection;
	int  m_noShoutTicks; // Decide whether to shout
	int  m_noPerpendicularTurnTicks; // Decide whether to turn penpendicularly
};

//**********************************************************************************
// Regular Protester Class
//**********************************************************************************
class RegularProtester: public Protester
{
public:
	RegularProtester(StudentWorld* sw, int tickPeriod):Protester(sw, "Regular", IID_PROTESTER, tickPeriod)
	{
		setHitPoints(5);
	}
	~RegularProtester(){}
	void doSomething();
};

//**********************************************************************************
// Hardcore Protester Class
//**********************************************************************************
class HardcoreProtester: public Protester
{
public:
	HardcoreProtester(StudentWorld* sw, int tickPeriod):Protester(sw, "Hardcore", IID_HARD_CORE_PROTESTER, tickPeriod)
	{
		setHitPoints(10);
	}
	~HardcoreProtester(){}
	void doSomething(); 
};

#endif // ACTOR_H_
