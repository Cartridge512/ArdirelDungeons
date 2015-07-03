#pragma once
class Room
{
public:
	Room(void);
	Room(int xx, int yy, int w, int h);
	~Room(void);
	int width, height, x, y;
};

