
#ifndef _GOL_MAP_H
#define _GOL_MAP_H

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define __CL_ENABLE_EXCEPTIONS

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <omp.h>
#include <CL/opencl.h>
#include <CL/cl.hpp>

#pragma comment(lib, "OpenCL.lib")

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

enum class Mode {
	SEQUENTIAL,
	OPENMP,
	OPENCL
};

enum class Device {
	CPU,
	GPU
};

class GOLMap
{
public:
	GOLMap();
	GOLMap(std::string path);
	GOLMap(int width, int height);
	GOLMap(Size size);
	~GOLMap();

	void loadFromFile(std::string path);
	
	inline int getNrOfNeighbors(int x, int y);
	inline int getNrOfNeighbors(int index);
	
	void printToFile(std::string path);

	char getCell(int x, int y) const;

	void run();

	Mode m_mode;
	Device m_device;
	int m_threads = 0;
	int m_platformId = 0;
	int m_deviceId = 0;

	int m_generations = 10;
private:


	const bool m_dead = false;
	const bool m_alive = true;

	const char m_charDead = '.';
	const char m_charAlive = 'x';
	
	Size m_Size;
	int m_Generations = 0;

	bool m_MapSwitch = false;
	bool** m_MapBuffer = nullptr;
	
	inline void initMap(int width, int height);

	friend std::ostream& operator<<(std::ostream& strm, const GOLMap& map);

	void runSeq();
	void runOMP();
	void runOCL();

};

#endif // !_GOL_MAP_H
