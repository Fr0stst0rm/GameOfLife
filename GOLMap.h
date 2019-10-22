
#ifndef _GOL_MAP_H
#define _GOL_MAP_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct Size
{
	int width;
	int height;
};

struct Point
{
	int x;
	int y;
};

class GOLMap
{
public:
	GOLMap(std::string path);
	GOLMap(int width, int height);
	GOLMap(Size size);
	~GOLMap();

	void toggle(int x, int y);
	void toggle(Point point);

	int getNrOfNeighbors(int x, int y);
	int getNrOfNeighbors(Point point);

	int nextGen();

	void printToFile(std::string path);

private:

	char m_dead = '.';
	char m_alive = 'x';

	Size m_Size;
	int m_Generations = 0;

	char ** m_Map;
	char** m_Buffermap;

	void initMap(int width, int height);

	friend std::ostream& operator<<(std::ostream& strm, const GOLMap& map);

	void copyMap(char** from, char** to);

};

#endif // !_GOL_MAP_H
