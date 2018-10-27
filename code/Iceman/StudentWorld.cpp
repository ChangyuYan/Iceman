#include "StudentWorld.h"
#include <string>
#include <cstdlib>
#include <algorithm>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}
// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

bool StudentWorld::hasIce(int x, int y)
{
	if ( x < 0 || x > 63 || y < 0 || y > 63)
		return false; 
	if (Arena[x][y] != nullptr)
		return Arena[x][y]->isAlive();
	
	return false;
}

bool StudentWorld::hasBlockAhead(int x, int y)
{
	if (m_player->getDirection() == Actor::up)
		return (y > 56 || hasIce(x, y+4) || hasIce(x, y+5) || hasIce(x, y+6) || hasIce(x, y+7));
	else if (m_player->getDirection() == Actor::down)
		return (y < 4 || hasIce(x, y-1) || hasIce(x, y-2) || hasIce(x, y-3) || hasIce(x, y-4));
	else if (m_player->getDirection() == Actor::left)
		return (x < 4 || hasIce(x-1, y) || hasIce(x-2, y) || hasIce(x-3, y) || hasIce(x-4, y));
	else if (m_player->getDirection() == Actor::right)
		return (x > 56 || hasIce(x+4, y) || hasIce(x+5, y) || hasIce(x+6, y) || hasIce(x+7, y));
	
	return false;
}


void StudentWorld::destoryIce(int x, int y)
{
	
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (hasIce(x+i, y+j)) // Also means that they are undiscovered
			{
				delete Arena[x+i][y+j];
				Arena[x+i][y+j] = nullptr;
				playSound(SOUND_DIG);
				
				// Now they ARE discovered, so we need to update MazeSolver
				updateMazeSolver();
			}
		}
	}
}

void StudentWorld::updateMazeSolver() // Double Check
{
	for (int x = 0; x <= 60; x++)
	{
		for (int y = 0; y <= 60; y++)
		{
			if (Arena[x][y] == nullptr)
			{
				MazeSolver[x][y] =  min(westValue(x, y), min(eastValue(x, y), min(northValue(x, y), southValue(x, y)))) + 1;
			}
		}
	}
}

int StudentWorld::northValue(int x, int y)
{
	return MazeSolver[x][y+1];
}

int StudentWorld::southValue(int x, int y)
{
	if ( y > 0)
		return MazeSolver[x][y-1];
	return 999;
}

int StudentWorld::westValue(int x, int y)
{
	if (x > 0)
		return MazeSolver[x-1][y];
	return 999;
}

int StudentWorld::eastValue(int x, int y)
{
	if (x < 60)
		return MazeSolver[x+1][y];
	return 999;
}

void StudentWorld::deleteAllIce()
{
	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 64; y++)
		{
			if(Arena[x][y] != nullptr)
			{
				delete Arena[x][y];
				Arena[x][y] = nullptr;
			}
			
		}
	}
}

void StudentWorld::addObject(Actor* pObject)
{
	Objects.push_back(pObject);
	if (pObject->getname() != "Sonar")
	{
		for (int y = 0; y < 60; y++)
		{
			for (int x = 0; x < 64; x++)
			{
				if (radius(pObject, x, y) <= 6)
					BoulderOccupied[x][y] = true;
			}
		}
	}
}

size_t StudentWorld::getVectorSize()
{
	return Objects.size();
}

Iceman* StudentWorld::getPlayer()
{
	return m_player;
}

void StudentWorld::setDisplayText()
{
	int level   = getLevel();
	int lives   = getLives();
	int health  = m_player->getHitPoints() * 10;
	int squirts = m_player->getWater();
	int gold    = m_player->getGold();
	int barrelsLeft = oilLeft(); 
	int sonar = getPlayer()->getSonar();
	int score = getPlayer()->getScore();
	
	string s = formStatusString(level, lives, health, squirts, gold, barrelsLeft, sonar, score);
	
	setGameStatText(s); 
}

void StudentWorld::addSonarOrWater()
{
	int level = getLevel();
	
	int G = level * 25 + 300;
	
	if (rand() % G == 0)
	{
		if (rand() % 4 == 0)
			addObject(new Sonar(max(100, (300 - (10*level))), this));
		else
		{
			int x = 0; int y = 0;
			decidePoolLoc(x, y);
			addObject(new Pool(this, x, y, max(100, 300 - 10*level)));
		}
			
	}
}

Actor* StudentWorld::accessObject(int i)
{
	return Objects[i];
}

double StudentWorld::radius(Actor* a1, Actor* a2)
{
	return sqrt( pow(a1->getX() - a2->getX(), 2) + pow(a1->getY() - a2->getY(), 2));
}

double StudentWorld::radius(Actor* a, int x, int y)
{
	return sqrt( pow(a->getX() - x, 2) + pow(a->getY() - y, 2));
}

double StudentWorld::radius(int x1, int y1, int x2, int y2)
{
	return sqrt( pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

int StudentWorld::randomX()
{
	int x1 = rand() % 27;
	int x2 = rand() % 27 + 34;
	int p  = rand() % 2;
	if (p == 0)
		return x1;
	return x2;
}

int StudentWorld::randomY(int x)
{
	int y = rand() % 57;
	
	while (BoulderOccupied[x][y] == true)
		y = rand() % 57;
	
	return y;
}

void StudentWorld::decidePoolLoc(int& x, int& y)
{
	while(gridHasIce(x, y))
	{
		x = rand() % 64;
		y = rand() % 60;
	}
}

bool StudentWorld::gridHasIce(int x, int y)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (hasIce(x + i, y + j))
				return true;
		}
	}
	return false;
}

string  StudentWorld::formStatusString(int level, int lives, int health, int squirts, int gold, int barrelsleft, int sonar, int score)
{
	string r_level;
	if (level < 10) r_level = r_level += " " + to_string(level);
	else r_level = to_string(level);
	
	string r_lives = to_string(lives);
	
	string r_health;
	if (health == 100) r_health = to_string(health);
	else if ( 9 < health && health < 100) r_health = r_health + " " + to_string(health);
	else if ( health < 10 ) r_health = r_health + "  " + to_string(health);
	
	string r_squirts;
	if (squirts < 10) r_squirts = r_squirts + " " + to_string(squirts);
	else r_squirts = to_string(squirts);
	
	string r_gold;
	if (gold < 10) r_gold = r_gold + " " + to_string(gold);
	else r_gold = to_string (gold);
	
	string r_barrels;
	if (barrelsleft < 10) r_barrels = " " + to_string(barrelsleft);
	else r_barrels = to_string(barrelsleft);
	
	string r_sonar;
	if (sonar < 10) r_sonar = " " + to_string(sonar);
	else r_sonar = to_string(sonar);
	
	string r_score;
	size_t zeros = 6 - to_string(score).size();
	for (int i = 0; i < zeros; i++)
		r_score+= "0";
	r_score += to_string(score);
	
	string result = "";
	return result + "Lvl: " + r_level + "  Lives: " + r_lives + "  Hlth: " + r_health +"%  Wtr: " + r_squirts + "  Gld: " + r_gold + "  Oil Left: " + r_barrels + "  Sonar: " + r_sonar + "  Scr: " + r_score;
}

int StudentWorld::oilLeft()
{
	return m_oil; 
}

void StudentWorld::decreaseOil()
{
	m_oil--; 
}
