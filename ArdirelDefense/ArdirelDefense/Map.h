#pragma once
#include "Room.h"
#include <vector>
using namespace std;
class Map
{
public:
	Map(void);
	~Map(void);
	void genLevel();
	void genRoom();
	void connect(Room r1, Room r2);
	int randomize(int min, int max);
	void fill(int startX, int startY, int endX, int endY);
	char map[601]; 
	int SMIN;
	int SMAX;
	vector<Room> rooms;
};

