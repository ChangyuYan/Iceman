#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include <string>
#include "Actor.h"
#include <iostream>
#include <vector>
#include <cmath>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir): GameWorld(assetDir)
	{
		
	}

	virtual int init()
	{
		tick = 0;
		numProtesters = 0;
		// Populating a 2D array with Ice;
		for (int y = 0; y < 60; y++)
		{
			for (int x = 0; x < 30; x++)
			{
				Arena[x][y] = new Ice(this, x, y);
				BoulderOccupied[x][y] = false;
			}
			for (int x = 30; x < 34; x++)
			{
				Arena[x][y] = nullptr;
				BoulderOccupied[x][y] = true;
			}
			for (int x = 34; x < 64; x++)
			{
				Arena[x][y] = new Ice(this, x, y);
				BoulderOccupied[x][y] = false;
			}
			
		}
		
		// Initialize MazeSolver
		for (int x = 0; x <= 60; x++)
		{
			for (int y = 0; y <= 60; y++)
			{
				MazeSolver[x][y] = 999; // Undiscovered
			}
		}
		for (int x = 0; x <= 60; x++)
			MazeSolver[x][60] = 60 - x; // The spaces are open, so the steps to exit point (60, 60) is 120 - (x + y)
		for (int x = 30; x < 34; x++)
		{
			for (int y = 0; y <= 60; y++)
			{
				MazeSolver[x][y] = 120 - x - y;
			}
		}
		
		// Initialize Spaces occupied by boulders
		for (int y = 0; y < 64; y++)
		{
			for (int x = 0; x < 64; x++)
				boulderSpace[x][y] = false; 
		}
		
		// Add Player
		m_player = new Iceman(this);
		
		// Add First Protester
		int probHardcore = fmin(90, getLevel()*10 + 30);
		int t = rand() % 100;
		if (t < probHardcore)
		{
			addObject(new HardcoreProtester(this, fmax(0, 3-getLevel()/4)));
			numProtesters++;
		}
		else
		{
			addObject(new RegularProtester(this, fmax(0, 3-getLevel()/4)));
			numProtesters++;
		}
		
		// Add Boulders
		int B = fmin( getLevel()/ 2 + 2, 9); // initialize the number of boulders
		
		for (int i = 0; i < B; i++)
		{
			int x = randomX(); int y = randomY(x);
			addObject(new Boulder(this, x, y));
			
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					delete Arena[x+i][y+j];
					Arena[x+i][y+j] = nullptr;
				}
			}
			
			for (int k = 0; k < 60; k++)
			{
				for (int m = 0; m < 64; m++)
					if (radius(m, k, x, y) <= 3)
						boulderSpace[m][k] = true;
			}
		}
		
		// Add Oil
		m_oil = fmin(2 + getLevel(), 21);
		for (int i = 0; i < m_oil; i++)
		{
			int x = randomX(); int y = randomY(x);
			addObject(new Oil(this, x, y));
		}
		
		// Add Gold
		int G = fmax(5 - getLevel()/2, 2);
		for (int i = 0; i < G; i++)
		{
			int x = randomX(); int y = randomY(x);
			addObject(new Gold(this, x, y, 'P', false, true, false));
		}
		
		return GWSTATUS_CONTINUE_GAME;
	}
	
	
	virtual int move()
	{
		tick++;
		int probHardcore = fmin(90, getLevel()*10 + 30);
		
		if (m_oil == 0)
			return GWSTATUS_FINISHED_LEVEL;
		
		// add protesters
		double target = fmin(15, 2 + getLevel()*1.5);
		if (tick >= fmax(25, 200 - getLevel()) && numProtesters < target)
		{
			int k = rand() % 100;
			if (k < probHardcore)
			{
				addObject(new HardcoreProtester(this, fmax(0, 3-getLevel()/4)));
			}
			else
			{
				addObject(new RegularProtester(this, fmax(0, 3-getLevel()/4)));
			}
			numProtesters++;
			tick = 0;
		}
			
		
		// Updating the Display Text
		setDisplayText();
		
		// Add Sonar/Water or not...
		addSonarOrWater();
		
		// As long as the actors are alive, let them do something
		if (!m_player->isAlive())
		{
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		
		// Player doSomething
		m_player->doSomething();
		
		
		// Objects doSomething
		if (Objects.size() > 0)
		{
			for (std::vector<Actor*>::iterator i = Objects.begin(); i != Objects.end(); )
			{
				if(! (*i)-> isAlive())
				{
					delete *i;
					i = Objects.erase(i);
				}
				else
				{
					(*i)->doSomething();
					i++;
				}
			}
		}
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual void cleanUp()
	{
		delete m_player;
		m_player = nullptr;
		deleteAllIce();
		for (std::vector<Actor*>::iterator i = Objects.begin(); i != Objects.end(); )
		{
			delete *i;
			i = Objects.erase(i);
		}
	}
	
	~StudentWorld()
	{
		if (m_player != nullptr){ delete m_player; m_player = nullptr; }
		deleteAllIce();
	}

	
	bool   hasIce(int x, int y);
	bool   hasBlockAhead(int x, int y);
	void   destoryIce(int x, int y);
	void   deleteAllIce();
	void   addObject(Actor* pObject);
	void   addSonarOrWater();
	Actor* accessObject(int i);
	size_t getVectorSize();
	double radius(Actor* a1, Actor* a2);
	double radius(Actor* a, int x, int y);
	double radius(int x1, int y1, int x2, int y2);
	int    randomX(); // To determine location of boulders, gold, and oil
	int    randomY(int x); // To determine location of boulders, gold, and oil
	bool   gridHasIce(int x, int y); 
	void   decidePoolLoc(int& x, int& y); // To determine location of add-on ice
	int    oilLeft();
	void   decreaseOil();
	void   updateMazeSolver();
	int    northValue(int x, int y);
	int    southValue(int x, int y);
	int    westValue(int x, int y);
	int    eastValue(int x, int y);
	bool   noOverlapWithIce(int x, int y);
	void   decreaseNumProtesters()
	{
		numProtesters--; 
	}
	
	Iceman* getPlayer();
	std::string formStatusString(int level, int lives, int health, int squirts, int gold, int barrelsLeft, int sonar, int score);
	bool boulderSpace[64][64]; // a location is true, if it's within radius 3 of a boulder
	int MazeSolver[61][61]; // Stores the distance of each grid to exit point. If undiscovered, make it 999.
	
private:
	void   setDisplayText();
	Actor* Arena[64][64]; // Holds Ice
	bool BoulderOccupied[64][60]; // Checks if location already has add-on boulders
	Iceman* m_player;
	std::vector <Actor*> Objects;
	int numProtesters;
	int tick;
	int m_oil;
};

#endif // STUDENTWORLD_H_
