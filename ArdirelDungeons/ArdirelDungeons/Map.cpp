#include "Map.h"
#include "Room.h"
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <ctime>
using namespace std;

Map::Map(void)
{
	SMIN = 3;
	SMAX = 5;
}


Map::~Map(void)
{
}

void Map::genLevel()
{
	srand(time(0));
	rand();

	for (int e = 0; e < 600; e++) {map[e] = 'w';}
	map[601] = '\0';
	int rCount = randomize(10, 12);
	//cout<<rCount<<endl;
	for (int r = 0; r < rCount; r++)
	{
		genRoom();
		//cout<<rooms[r].x << " " << rooms[r].y << " " << rooms[r].width << " " << rooms[r].height<<endl;
	}

	for (int c = 0; c < rooms.size(); c++)
	{
		int rtcIndex = c;
		while (rtcIndex == c)
		{
			rtcIndex = randomize(0, rooms.size()-1);
		}
		//cout<<c<<endl;
		connect(rooms[c], rooms[rtcIndex]);
	}
	//connect(rooms[0], rooms[1]);

	ofstream outMap;
	outMap.open("Maps/map.txt");
	outMap << map;
	outMap.close();
}

void Map::genRoom()
{
	int width = randomize(SMIN, SMAX);
	int height = randomize(SMIN, SMAX);
	int x = randomize(1, 29-width);
	int y = randomize(1, 19-height);
	bool clear = true;

	for (int a = x-1; a < x+width+1; a++)
	{
		for (int b = y-1; b < y+height+1; b++)
		{
			if(map[(a)*20 + (b)] == 'f') {clear = false;}
		}
	}
	
	if(clear)
		{
		rooms.push_back(Room(x, y, width, height));
		for (int a = x; a < x+width; a++)
		{
			for (int b = y; b < y+height; b++)
			{
				map[(a)*20 + (b)] = 'f';
				//cout<<(a)*20 + (b)<<endl;
			}
		}
	}
	else
	{
		genRoom();
	}
}

void Map::connect(Room r1, Room r2)
{
	int r1x = randomize(r1.x, r1.x + r1.width - 1);
	int r1y = randomize(r1.y, r1.y + r1.height - 1);
	int r2x = randomize(r2.x, r2.x + r2.width - 1);
	//int r2y = r1y;
	//while(r2y == r1y)
	//{
		int r2y = randomize(r2.y, r2.y + r2.height - 1);
	//}

	//if (r1x > r2x)
    //{
    //    int tmp = r1x;
    //    r1x = r2x;
    //    r2x = tmp;
    //}
    //if (r1y > r2y)
    //{
    //    int tmp = r1y;
    //    r1y = r2y;
    //    r2y = tmp;
    //}
	//
	//fill(r1x, r1y, r2x+1, r1y+1);
	//if(r1x < r2x)
	//{
	//	//fill(r1x, r1y, r1x+1, r2y+1);
	//	fill(r2x, r1y, r2x+1, r2y+1);
	//}
	//else
	//{
	//	//fill();
	//}

	if(r1x < r2x)
	{
		if(r1y < r2y) {fill(r1x, r1y, r2x+1, r1y+1); fill(r2x, r1y, r2x+1, r2y+1);}
		else {fill(r1x, r1y, r2x+1, r1y+1); fill(r2x, r2y, r2x+1, r1y+1);}
	}
	else
	{
		if(r1y < r2y) {fill(r2x, r2y, r1x+1, r2y+1); fill(r1x, r1y, r1x+1, r2y+1);}
		else {fill(r2x, r2y, r1x+1, r2y+1); fill(r1x, r2y, r1x+1, r1y+1);}
	}
}

void Map::fill(int startX, int startY, int endX, int endY)
{
	for (int x = startX; x < endX; x++)
    {
        for (int y = startY; y < endY; y++)
        {
			map[(x)*20 + (y)] = 'f';
            //map[(x)*20 + (y)] = 'h';
			//if((x == startX || x==endX-1) && (y == startY || y==endY-1))
			//{
			//	map[(x)*20 + (y)] = 'c';
			//}
        }
    }
}

int Map::randomize(int min, int max)
{
	if (max-min > 0)
	{
		return (rand() % (max-min+1)) + min;
	}
	else {
		return min;
	}
}
