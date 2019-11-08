
#ifndef _GOL_MAP_H
#define _GOL_MAP_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define XY_TO_INDEX(X,Y) (((Y) * m_Size.width) + ( X ))

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
	
	inline int getNrOfNeighbors(int x, int y);
	inline int getNrOfNeighbors(int index);
	
	void printToFile(std::string path);

	char getCell(int x, int y) const;

	void run(int gen);

private:

	const bool m_dead = false;
	const bool m_alive = true;

	const char m_charDead = '.';
	const char m_charAlive = 'x';
	
	Size m_Size;
	int m_Generations = 0;

	
	bool* m_Map;
	bool* m_Buffermap;
	
	inline void initMap(int width, int height);

	friend std::ostream& operator<<(std::ostream& strm, const GOLMap& map);
	
	void copyMap(bool* from, bool* to);

};

#endif // !_GOL_MAP_H
