#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

using namespace std;

//**********************************************************************************
// Actor Implementation
//**********************************************************************************
bool Actor::isAlive()
{
	return m_alive;
}

void Actor::setDead()
{
	m_alive = false;
}

void Actor::setHitPoints(int points)
{
	m_hitPoints = points;
}

int Actor::getHitPoints()
{
	return m_hitPoints;
}

void Actor::zeroHitPoints()
{
	m_hitPoints = 0;
}
StudentWorld* Actor::getWorld()
{
	return m_sw;
}

string Actor::getname()
{
	return m_className;
}

void Actor::ifMoveAhead(int& x, int& y)
{
	if (getDirection() == left)
		x--;
	else if (getDirection() == right)
		x++;
	else if (getDirection() == up)
		y++;
	else if (getDirection() == down)
		y--;
}

void Actor::decreaseHitPoints(int byHowMany)
{
	m_hitPoints -= byHowMany; 
}

bool Actor::moreThanXUnitsAway(Actor* a1, Actor* a2, int x_units)
{
	return getWorld()->radius(a1, a2) >  x_units;
}

bool Actor::lessThanOrEqualToXUnitsAway(Actor* a1, Actor* a2, int x_units)
{
	return getWorld()->radius(a1, a2) <= x_units;
}

void Actor::moveForwardOneStep()
{
	if (getDirection() == left)
		moveTo(max(0, getX()-1), getY());
	else if (getDirection() == right)
		moveTo(min(60, getX()+1), getY());
	else if (getDirection() == down)
		moveTo(getX(), max(0, getY()-1));
	else if (getDirection() == up)
		moveTo(getX(), min(60, getY()+1));
}

bool Actor::hasIceInHumanDirection(Direction dir)
{
	bool result = false;
	
	if (dir == right)
	{
		for (int i = 0; i < 4; i++)
			if (getWorld()->hasIce(getX() + 4, getY() + i))
				result = true;
	}
	else if (dir == left)
	{
		for (int i = 0; i < 4; i++)
			if (getWorld()->hasIce(getX() - 1, getY() + i))
				result = true;
	}
	else if (dir == up)
	{
		for (int i = 0; i < 4; i++)
			if (getWorld()->hasIce(getX() + i, getY() + 4))
				result = true;
	}
	else if (dir == down)
	{
		for (int i = 0; i < 4; i++)
			if (getWorld()->hasIce(getX() + i, getY() - 1))
				result = true;
	}
	
	return result;
}

bool Actor::hasBlouderSpaceInHumanDirection(Direction dir)
{
	bool result = false;
	
	if (dir == right)
	{
		for (int i = 0; i < 4; i++)
			if (getWorld()->boulderSpace[getX() + 4][getY() + i])
				result = true;
	}
	else if (dir == left)
	{
		for (int i = 0; i < 4; i++)
			if (getWorld()->boulderSpace[getX() - 1][getY() + i])
				result = true;
	}
	else if (dir == up)
	{
		for (int i = 0; i < 4; i++)
			if (getWorld()->boulderSpace[getX() + i][getY() + 4])
				result = true;
	}
	else if (dir == down)
	{
		for (int i = 0; i < 4; i++)
			if (getWorld()->boulderSpace[getX() + i][getY() - 1])
				result = true;
	}
	return result;
}

bool Actor::isEdgeInHumanDirection(Direction dir)
{
	if (dir == right)
	{
		return (getX() + 4 > 63);
	}
	else if (dir == left)
	{
		return (getX() - 1 < 0);
	}
	else if (dir == up)
	{
		return (getY() + 4 > 63);
	}
	else if (dir == down)
	{
		return (getY() - 1 < 0);
	}
	return false;
}

bool Actor::isAnnoyed()
{
	return m_annoyed;
}

void Actor::makeAnnoyed()
{
	m_annoyed = true;
}
void Actor::resetAnnoyed()
{
	m_annoyed = false;
}
void Actor::makeLeave()
{
	m_leaveTheOilField = true;
}

bool Actor::isLeaving()
{
	return m_leaveTheOilField;
}


//**********************************************************************************
// Iceman Implementation
//**********************************************************************************
void Iceman::doSomething()
{
	if (!isAlive())
		return;
	if (getHitPoints() <= 0)
	{
		setDead();
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
	}
	
	
	getWorld()->destoryIce(getX(), getY());
	
	int ch;
	if (getWorld()->getKey(ch) == true)
	{
		// User hit a key this tick!
		switch(ch)
		{
			case KEY_PRESS_LEFT:
				if (getDirection() != left)
					setDirection(left);
				else
					if (notWithinBoulderRadius())
						moveForwardOneStep();
				break;
			case KEY_PRESS_RIGHT:
				if (getDirection() != right)
					setDirection(right);
				else
					if (notWithinBoulderRadius())
						moveForwardOneStep();
				break;
			case KEY_PRESS_DOWN:
				if (getDirection() != down)
					setDirection(down);
				else
					if (notWithinBoulderRadius())
						moveForwardOneStep();
				break;
			case KEY_PRESS_UP:
				if (getDirection() != up)
					setDirection(up);
				else
					if (notWithinBoulderRadius())
						moveForwardOneStep();
				break;
			case KEY_PRESS_ESCAPE:
				setDead();
				break;
			case KEY_PRESS_SPACE:
				if (getWater() > 0)
					fireWater();
				break;
			case 'Z':
			case 'z':
				if (m_sonar > 0)
				{
					getWorld()->playSound(SOUND_SONAR);
					for(int i = 0; i < getWorld()->getVectorSize(); i++)
					{
						if (getWorld()->radius(this, getWorld()->accessObject(i)) <= 12)
						{
							getWorld()->accessObject(i)->setVisible(true);
						}
					}
					m_sonar--;
				}
				break;
			case KEY_PRESS_TAB:
				if (getGold() > 0)
				{
					getWorld()->addObject(new Gold(getWorld(), getX(), getY(), 'T', true, false, true));
					m_gold--;
				}
				break;
		}
	}
}

void Iceman::fireWater()
{
	if (!getWorld()->hasBlockAhead(getX(), getY()))
		getWorld()->addObject(new Squirt(this));
	
	m_water--;
	getWorld()->playSound(SOUND_PLAYER_SQUIRT);
}
int Iceman::getWater()
{
	return m_water;
}

void Iceman::addWater(int amount)
{
	m_water += amount;
}

int Iceman::SquirtX()
{
	if (getDirection() == right)
		return min(getX() + 4, 60);
	else if (getDirection() == left)
		return max(getX() - 4, 0);
	else
		return getX();
}

int Iceman::SquirtY()
{
	if (getDirection() == up)
		return min(getY() + 4, 60);
	else if (getDirection() == down)
		return max(getY() - 4, 0);
	else
		return getY();
}

void Iceman::addGold()
{
	m_gold++;
}

int Iceman::getGold()
{
	return m_gold;
}

int Iceman::getScore()
{
	return m_score;
}

void Iceman::addScore(int amount)
{
	m_score += amount;
}

void Iceman::addSonar()
{
	m_sonar++;
}

int Iceman::getSonar()
{
	return m_sonar;
}

bool Iceman::notWithinBoulderRadius()
{
	int x = getX();
	int y = getY();
	
	ifMoveAhead(x, y);
	if (getWorld()->boulderSpace[x][y]) //Meaning will be moving into a circle of radius 3 from a boulder
		return false;
	
	return true;
}
//**********************************************************************************
// Squirt Implementation
//**********************************************************************************
void Squirt::doSomething()
{
	for (int i = 0; i < getWorld()->getVectorSize(); i++)
	{
		if (getWorld()->accessObject(i)->getname() == "Regular" || getWorld()->accessObject(i)->getname() == "Hardcore")
		{
			if (lessThanOrEqualToXUnitsAway(this, getWorld()->accessObject(i), 3))
			{
				getWorld()->accessObject(i)->decreaseHitPoints(2);
				getWorld()->accessObject(i)->makeAnnoyed();
				setDead();
				
				if (getWorld()->accessObject(i)->getHitPoints() <= 0 && !getWorld()->accessObject(i)->isLeaving())
				{
					getWorld()->getPlayer()->addScore(100);
				}
			}
		}
	}
	if (m_distance == 0 || getWorld()->hasBlockAhead(getX(), getY()))
	{
		setDead();
		return;
	}
	if(getDirection() == left)
	{
		moveTo(max(0, getX()-1), getY());
		m_distance--;
	}
	else if (getDirection() == right)
	{
		moveTo(min(60, getX()+1), getY());
		m_distance--;
	}
	else if (getDirection() == down)
	{
		moveTo(getX(), max(0, getY()-1));
		m_distance--;
	}
	else if (getDirection() == up)
	{
		moveTo(getX(), min(60, getY()+1));
		m_distance--;
	}
	
}

//**********************************************************************************
// Sonar Implementation
//**********************************************************************************
void Sonar::doSomething()
{
	if (m_lifeTime == 0)
		setDead();
	
	if (!isAlive())
		return;
	
	if (lessThanOrEqualToXUnitsAway(this, getWorld()->getPlayer(), 3))
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->addSonar();
		getWorld()->getPlayer()->addScore(75);
	}
	m_lifeTime--;
}


//**********************************************************************************
// Boulder Implementation
//**********************************************************************************
void Boulder::doSomething()
{
	if (!isAlive())
		return;
	if (m_state == "STABLE")
	{
		if (!checkIfStable())
			m_state = "WAITING";
	}
	
	if (m_state == "WAITING")
		m_waitingTicks++;
	
	if (m_state == "WAITING" && m_waitingTicks == 30)
	{
		m_state = "FALLING";
		getWorld()->playSound(SOUND_FALLING_ROCK);
	}
	
	if (m_state == "FALLING")
	{
		for (int k = 0; k < 60; k++)
		{
			for (int m = 0; m < 64; m++)
				if (getWorld()->radius(m, k, getX(), getY()) <= 3)
					getWorld()->boulderSpace[m][k] = false;
		}
		if(!hitStuff())
		{
			moveTo(getX(), getY()-1);
			hitHuman();
		}
		
		if(hitStuff())
			setDead();
		
	}
}

bool Boulder::checkIfStable()
{
	for (int i = 0; i < 4; i++)
	{
		if (getWorld()->hasIce(getX() + i, getY() - 1))
			return true;
	}
	return false;
}

bool Boulder::hitStuff()
{
	// Checks to see if Boulder hits an ice, another boulder, or the bottom
	for (int i = 0; i < 4; i++)
	{
		if (getWorld()->hasIce(getX() + i, getY()))
			return true;
	}
	if (getY() == 0)
		return true;
	if (hasBoulderBelow())
		return true;
	
	return false;
}

bool Boulder::hasBoulderBelow()
{
	if (getY() < 4)
		return false;
	
	for (int i = 0; i < getWorld()->getVectorSize(); i++)
	{
		if (getWorld()->accessObject(i)->getname() == "Boulder" && getWorld()->accessObject(i)->getY() == getY() - 4)
		{
			if (getWorld()->accessObject(i)->getX() > getX() - 4 && getWorld()->accessObject(i)->getX() < getX() + 4)
				return true;
		}
	}
	return false;
}

bool Boulder::hitHuman()
{
	if (lessThanOrEqualToXUnitsAway(this, getWorld()->getPlayer(), 3))
	{
		getWorld()->getPlayer()->zeroHitPoints();
		return true;
	}
	for (int i = 0; i < getWorld()->getVectorSize(); i++)
	{
		if(getWorld()->accessObject(i)->getname() == "Regular" || getWorld()->accessObject(i)->getname() == "Hardcore" )
		{
			 if (lessThanOrEqualToXUnitsAway(this, getWorld()->accessObject(i), 3))
			 {
				 getWorld()->accessObject(i)->zeroHitPoints();
				 getWorld()->getPlayer()->addScore(500);
				 return true;
			 }
		}
	}
	return false;
}
//**********************************************************************************
// Oil Implementation
//**********************************************************************************
void Oil::doSomething()
{
	if(!isAlive())
		return;
	
	if (isVisible() == false && lessThanOrEqualToXUnitsAway(this, getWorld()->getPlayer(), 4))
	{
		setVisible(true);
		return;
	}
	if (lessThanOrEqualToXUnitsAway(this, getWorld()->getPlayer(), 3))
	{
		setDead();
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->getPlayer()->addScore(1000);
		getWorld()->decreaseOil();
	}
	
}

//**********************************************************************************
// Gold Implementation
//**********************************************************************************

void Gold::doSomething()
{
	if (!isAlive())
		return;
	if (isVisible() == false && lessThanOrEqualToXUnitsAway(this, getWorld()->getPlayer(), 4))
	{
		setVisible(true);
		return;
	}
	if (m_pickableByIceman && lessThanOrEqualToXUnitsAway(this, getWorld()->getPlayer(), 3))
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->addScore(10);
		getWorld()->getPlayer()->addGold();
	}
	
	if (m_state == 'T')
	{
		m_lifetime--;
		if(m_lifetime == 0)
			setDead();
	}
}

//**********************************************************************************
// Pool Implementation
//**********************************************************************************
void Pool::doSomething()
{
	if (m_lifetime == 0)
		setDead();
	if (!isAlive())
		return;
	if (lessThanOrEqualToXUnitsAway(this, getWorld()->getPlayer(), 3))
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->addWater(5);
		getWorld()->getPlayer()->addScore(100);
	}
	m_lifetime--;
}
//**********************************************************************************
// Protester Implementation
//**********************************************************************************
int Protester::getStateCount()
{
	return m_stateCount;
}

void Protester::updateStateCount()
{
	if(m_stateCount > 0)
		m_stateCount--;
	else
		m_stateCount = fmax(0, 3-(getWorld()->getLevel())/4); // Set it back to the original
}

void Protester::resetStateCount()
{
	m_stateCount = 0;
}


void Protester::moveOneStepToExit()
{
	// TODO: COME BACK AND IMPLEMENT THIS SOON!
	
	int North = getWorld()->northValue(getX(), getY());
	int South = getWorld()->southValue(getX(), getY());
	int East  = getWorld()->eastValue(getX(), getY());
	int West  = getWorld()->westValue(getX(), getY());
	
	int a[4] = {North, South, East, West};

	sort(a, a+4);

	if (North == a[0])
	{
		moveNorth();
	}
	else if (South == a[0])
	{
		moveSouth();
	}
	else if (West == a[0])
	{
		moveWest();
	}
	else if (East == a[0])
	{
		moveEast();
	}

}

void Protester::moveNorth()
{
	setDirection(up);
	moveTo(getX(), getY() + 1);
}

void Protester::moveSouth()
{
	setDirection(down);
	moveTo(getX(), getY() - 1);
}

void Protester::moveWest()
{
	setDirection(left);
	moveTo(getX() - 1, getY());
}

void Protester::moveEast()
{
	setDirection(right);
	moveTo(getX() + 1, getY());
}


bool Protester::isFacingIceman()
{
	if (getDirection() == left)
	{
		if (getWorld()->getPlayer()->getX() <= getX())
			return true;
	}
	else if(getDirection() == right)
	{
		if (getWorld()->getPlayer()->getX() >=  getX())
			return true;
	}
	else if(getDirection() == up)
	{
		if (getWorld()->getPlayer()->getY() >=  getY())
			return true;
	}
	else if(getDirection() == down)
	{
		if (getWorld()->getPlayer()->getY() <= getY())
			return true;
	}
	return false;
}

int Protester::getNoShoutTicks()
{
	return m_noShoutTicks;
}

void Protester::updateNoShoutTicks()
{
	if (m_noShoutTicks < 15)
		m_noShoutTicks++;
	else if (m_noShoutTicks == 15)
		m_noShoutTicks = 0;
	else if (m_noShoutTicks > 15)
		m_noShoutTicks = 15;
}

int Protester::getNoPerpendicularTurnTicks()
{
	return m_noPerpendicularTurnTicks;
}

void Protester::updateNoPerpendicularTurnTicks()
{
	if (m_noPerpendicularTurnTicks < 200)
		m_noPerpendicularTurnTicks++;
	else if (m_noPerpendicularTurnTicks == 200)
		m_noPerpendicularTurnTicks = 0;
	else if (m_noPerpendicularTurnTicks > 200)
		m_noPerpendicularTurnTicks = 200; 
}

bool Protester::onTheSameLineAsIceman()
{
	if (getX() == getWorld()->getPlayer()->getX())
		return true;
	if (getY() == getWorld()->getPlayer()->getY())
		return true;
	return false;
}

bool Protester::noBlocksOnPathToIceman()
{
	int px = getWorld()->getPlayer()->getX();
	int py = getWorld()->getPlayer()->getY();
	
	if (getX() == px)
	{
		if (getY() < py)
		{
			for (int i = 0; i < 4; i++)
				for (int y = getY(); y != py; y++)
					if (getWorld()->hasIce(getX()+i, y) || getWorld()->boulderSpace[getX()+i][y])
						return false;
		}
		else
		{
			for (int i = 0; i < 4; i++)
				for (int y = getY(); y != py; y--)
					if (getWorld()->hasIce(getX()+i, y) || getWorld()->boulderSpace[getX()+i][y])
						return false;
		}
	}
	if (getY() == py)
	{
		if (getX() < px)
		{
			for (int i = 0; i < 4; i++)
				for (int x = getX(); x != px; x++)
					if (getWorld()->hasIce(x, getY()+i) || getWorld()->boulderSpace[x][getY()+i])
						return false;
		}
		else
		{
			for (int i = 0; i < 4; i++)
				for (int x = getX(); x != px; x--)
					if (getWorld()->hasIce(x, getY()+i) || getWorld()->boulderSpace[x][getY()+i])
						return false;
		}
	}
	return true;
}

Actor::Direction Protester::DirectionToIceman()
{
	if (getX() < getWorld()->getPlayer()->getX())
		return Actor::right;
	if (getX() > getWorld()->getPlayer()->getX())
		return Actor::left;
	if (getY() < getWorld()->getPlayer()->getY())
		return Actor::up;
	if (getY() > getWorld()->getPlayer()->getY())
		return Actor::down;
	
	return Actor::right; 
}

void Protester::ZeroNumSquaresToMoveInCurrentDirection()
{
	m_numSquaresToMoveInCurrentDirection = 0;
}

void Protester::DecrementNumSquaresToMoveInCurrentDirection()
{
	m_numSquaresToMoveInCurrentDirection --;
}

int Protester::getNumSquaresToMoveInCurrentDirection()
{
	return m_numSquaresToMoveInCurrentDirection;
}

void Protester::PickANewValueForNumSquaresToMoveInCurrentDirection()
{
	m_numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
}

void Protester::randomlyChangeDirection()
{
	int x = rand() % 4;
	switch (x)
	{
		case 0:
			setDirection(up);
			break;
		case 1:
			setDirection(down);
			break;
		case 2:
			setDirection(left);
			break;
		case 3:
			setDirection(right);
			break;
	}
}

bool Protester::cannotMoveAhead()
{
	int x; int y;
	ifMoveAhead(x, y);
	
	return false;
}

bool Protester::isAtIntersection()
{
	int count = 0;
	if (!hasIceInHumanDirection(left) && !hasBlouderSpaceInHumanDirection(left) && !isEdgeInHumanDirection(left))
		count++;
	if (!hasIceInHumanDirection(right) && !hasBlouderSpaceInHumanDirection(right) && !isEdgeInHumanDirection(right))
		count++;
	if (!hasIceInHumanDirection(up) && !hasBlouderSpaceInHumanDirection(up) && !isEdgeInHumanDirection(up))
		count++;
	if (!hasIceInHumanDirection(down) && !hasBlouderSpaceInHumanDirection(down) && !isEdgeInHumanDirection(down))
		count++;
	if (count > 2)
		return true;
	return false;
}

Actor::Direction Protester::pickAPerpendicularDirection()
{
	vector<Direction> choice;
	
	if (getDirection() == left || getDirection() == right)
	{
		if (!hasIceInHumanDirection(up) && !hasBlouderSpaceInHumanDirection(up) && !isEdgeInHumanDirection(up))
			choice.push_back(up);
		if (!hasIceInHumanDirection(down) && !hasBlouderSpaceInHumanDirection(down) && !isEdgeInHumanDirection(down))
			choice.push_back(down);
	}
	else if (getDirection() == up || getDirection() == down)
	{
		if (!hasIceInHumanDirection(left) && !hasBlouderSpaceInHumanDirection(left) && !isEdgeInHumanDirection(left))
			choice.push_back(left);
		if (!hasIceInHumanDirection(right) && !hasBlouderSpaceInHumanDirection(right) && !isEdgeInHumanDirection(right))
			choice.push_back(right);
	}
	return choice[rand() % choice.size()];
}

bool Protester::pickUpGoldIfThereIsOne()
{
	for (int i = 0; i < getWorld()->getVectorSize(); i++)
	{
		if (getWorld()->accessObject(i)->getname() == "Gold")
		{
			if (lessThanOrEqualToXUnitsAway(this, getWorld()->accessObject(i), 3))
			{
				getWorld()->accessObject(i)->setDead();
				getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
				getWorld()->getPlayer()->addScore(25);
				if (getname() == "Regular")
					makeLeave(); 
				return true;
			}
		}
	}
	return false;
}
//**********************************************************************************
// Regular Protester Implementation
//**********************************************************************************
void RegularProtester::doSomething()
{
	if (!isAlive())
		return;
	
	if (getStateCount() > 0)
	{
		updateStateCount();
		return;
	}
	if (isLeaving())
	{
		if (getX() == 60 && getY() == 60)
		{
			setDead();
			getWorld()->decreaseNumProtesters();
		}
		else
		{
			moveOneStepToExit();
		}
		updateStateCount();
		updateNoPerpendicularTurnTicks();
		updateNoShoutTicks();
		return;
	}
	if (getHitPoints() <= 0)
	{
		makeLeave();
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		resetStateCount(); 
		return;
	}
	if (isAnnoyed())
	{
		if (AnnoyedTicks < fmax(50, 100 - getWorld()->getLevel() * 10))
		{
			AnnoyedTicks++;
			return;
		}
		resetAnnoyed();
		AnnoyedTicks = 0;
	}
	if (lessThanOrEqualToXUnitsAway(this, getWorld()->getPlayer(), 4) && isFacingIceman())
	{
		if (getNoShoutTicks() >= 15)
		{
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			getWorld()->getPlayer()->decreaseHitPoints(2);
		}
		updateStateCount();
		updateNoPerpendicularTurnTicks();
		updateNoShoutTicks();
		return;
	}
	if (onTheSameLineAsIceman() && moreThanXUnitsAway(this, getWorld()->getPlayer(), 4) && noBlocksOnPathToIceman())
	{
		setDirection(DirectionToIceman());
		moveForwardOneStep();
		pickUpGoldIfThereIsOne();
		updateStateCount();
		updateNoPerpendicularTurnTicks();
		updateNoShoutTicks();
		ZeroNumSquaresToMoveInCurrentDirection();
		return;
	}
	else
	{
		DecrementNumSquaresToMoveInCurrentDirection();
		if(getNumSquaresToMoveInCurrentDirection() <= 0)
		{
			randomlyChangeDirection();
			while (hasIceInHumanDirection(getDirection()) || hasBlouderSpaceInHumanDirection(getDirection()) || isEdgeInHumanDirection(getDirection()))
				randomlyChangeDirection();
			PickANewValueForNumSquaresToMoveInCurrentDirection();
		}
		else if ( isAtIntersection() && getNoPerpendicularTurnTicks() >= 200)
		{
			setDirection(pickAPerpendicularDirection());
			PickANewValueForNumSquaresToMoveInCurrentDirection();
		}
	}
	
	while (hasIceInHumanDirection(getDirection()) || hasBlouderSpaceInHumanDirection(getDirection()) || isEdgeInHumanDirection(getDirection()))
		randomlyChangeDirection();
	moveForwardOneStep();
	pickUpGoldIfThereIsOne();
	updateStateCount();
	updateNoPerpendicularTurnTicks();
	updateNoShoutTicks();
	
	if (hasIceInHumanDirection(getDirection()) || hasBlouderSpaceInHumanDirection(getDirection()) || isEdgeInHumanDirection(getDirection()))
	{
		ZeroNumSquaresToMoveInCurrentDirection();
	}
}
//**********************************************************************************
// Hardcore Protester Implementation
//**********************************************************************************
void HardcoreProtester::doSomething()
{
	if (!isAlive())
		return;
	
	if (getStateCount() > 0)
	{
		updateStateCount();
		return;
	}
	if (isLeaving())
	{
		if (getX() == 60 && getY() == 60)
		{
			setDead();
			getWorld()->decreaseNumProtesters();
		}
		else
		{
			moveOneStepToExit();
		}
		updateStateCount();
		updateNoPerpendicularTurnTicks();
		updateNoShoutTicks();
		return;
	}
	if (getHitPoints() <= 0)
	{
		makeLeave();
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		resetStateCount();
		return;
	}
	if (isAnnoyed())
	{
		if (AnnoyedTicks < fmax(50, 100 - getWorld()->getLevel() * 10))
		{
			AnnoyedTicks++;
			return;
		}
		resetAnnoyed();
		AnnoyedTicks = 0;
	}
	if (lessThanOrEqualToXUnitsAway(this, getWorld()->getPlayer(), 4) && isFacingIceman())
	{
		if (getNoShoutTicks() >= 15)
		{
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			getWorld()->getPlayer()->decreaseHitPoints(2);
		}
		updateStateCount();
		updateNoPerpendicularTurnTicks();
		updateNoShoutTicks();
		return;
	}
	if (moreThanXUnitsAway(this, getWorld()->getPlayer(), 4))
	{
		int M = 16 + (getWorld()->getLevel()) * 2;
		// Implement Step 5
	}
	if (onTheSameLineAsIceman() && moreThanXUnitsAway(this, getWorld()->getPlayer(), 4) && noBlocksOnPathToIceman())
	{
		setDirection(DirectionToIceman());
		moveForwardOneStep();
		pickUpGoldIfThereIsOne();
		updateStateCount();
		updateNoPerpendicularTurnTicks();
		updateNoShoutTicks();
		ZeroNumSquaresToMoveInCurrentDirection();
		return;
	}
	else
	{
		DecrementNumSquaresToMoveInCurrentDirection();
		if(getNumSquaresToMoveInCurrentDirection() <= 0)
		{
			randomlyChangeDirection();
			while (hasIceInHumanDirection(getDirection()) || hasBlouderSpaceInHumanDirection(getDirection()) || isEdgeInHumanDirection(getDirection()))
				randomlyChangeDirection();
			PickANewValueForNumSquaresToMoveInCurrentDirection();
		}
		else if ( isAtIntersection() && getNoPerpendicularTurnTicks() >= 200)
		{
			setDirection(pickAPerpendicularDirection());
			PickANewValueForNumSquaresToMoveInCurrentDirection();
		}
	}
	moveForwardOneStep();
	pickUpGoldIfThereIsOne();
	updateStateCount();
	updateNoPerpendicularTurnTicks();
	updateNoShoutTicks();
	
	if (hasIceInHumanDirection(getDirection()) || hasBlouderSpaceInHumanDirection(getDirection()) || isEdgeInHumanDirection(getDirection()))
	{
		ZeroNumSquaresToMoveInCurrentDirection();
	}
}
