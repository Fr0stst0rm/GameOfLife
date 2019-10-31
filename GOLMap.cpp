#include "GOLMap.h"

GOLMap::GOLMap(std::string path)
{
	int width = 0;
	int height = 0;

	std::ifstream inFile;
	inFile.open(path);

	std::string  line = "";

	if (!inFile.eof()) {
		std::getline(inFile, line);
		std::stringstream sstream(line);

		std::string integer;
		std::getline(sstream, integer, ',');
		width = std::stoi(integer);
		std::getline(sstream, integer, ',');
		height = std::stoi(integer);

	}

	initMap(width, height);

	int charNr = 0;

	for (int lineNr = 0; !inFile.eof(); lineNr++) {
		std::getline(inFile, line);

		if (!line.empty()) {
			for (charNr = 0; charNr < width; charNr++) {
				m_Map[XY_TO_INDEX(charNr,lineNr)] = line.c_str()[charNr];
				m_Buffermap[XY_TO_INDEX(charNr, lineNr)] = line.c_str()[charNr];
				//m_Map[lineNr][charNr] = line.c_str()[charNr];
				//m_Buffermap[lineNr][charNr] = line.c_str()[charNr];
			}
		}
	}
	inFile.close();

}

GOLMap::GOLMap(int width, int height)
{
	initMap(width, height);
}

GOLMap::GOLMap(Size size)
{
	initMap(size.width, size.height);
}

void GOLMap::initMap(int width, int height)
{
	m_Size.width = width;
	m_Size.height = height;

	m_Map = new char[m_Size.height * m_Size.width];
	m_Buffermap = new char[m_Size.height * m_Size.width];

	/*
	m_Map = new char* [m_Size.height];
	m_Buffermap = new char* [m_Size.height];
	for (int y = 0; y < m_Size.height; y++) {
		m_Map[y] = new char[m_Size.width];
		m_Buffermap[y] = new char[m_Size.width];
		//for (int x = 0; x < m_Size.width; x++) {
		//	m_Map[y][x] = m_dead;
		//}
	}
	*/
}

void GOLMap::copyMap2D(char** from, char** to)
{
	for (int y = 0; y < m_Size.height; y++) {
		for (int x = 0; x < m_Size.width; x++) {
			to[y][x] = from[y][x];
		}
	}
}

void GOLMap::copyMap(char* from, char* to)
{
	for (int i = 0; i < (m_Size.height * m_Size.width); i++) {
		to[i] = from[i];
	}
}

GOLMap::~GOLMap()
{
	//for (int y = 0; y < m_Size.height; y++) {
	//	delete[] m_Map[y];
	//}
	delete[] m_Map;

	//for (int y = 0; y < m_Size.height; y++) {
	//	delete[] m_Buffermap[y];
	//}
	delete[] m_Buffermap;
}

void GOLMap::toggle(int x, int y)
{
	x = x % m_Size.width;
	y = y % m_Size.height;

	/*
	if (m_Map[y][x] == m_dead) {
		m_Map[y][x] = m_alive;
	}
	else {
		m_Map[y][x] = m_dead;
	}
	*/
}

void GOLMap::toggle(Point point)
{
	this->toggle(m_Size.width, m_Size.height);
}

int GOLMap::getNrOfNeighbors(int x, int y)
{

	int count = 0;

	x += m_Size.width;
	y += m_Size.height;

	if (m_Map[XY_TO_INDEX((x - 1) % m_Size.width, (y - 1) % m_Size.height)] == m_alive)count++;
	if (m_Map[XY_TO_INDEX((x) % m_Size.width, (y - 1) % m_Size.height)] == m_alive)count++;
	if (m_Map[XY_TO_INDEX((x + 1) % m_Size.width, (y - 1) % m_Size.height)] == m_alive)count++;
	if (m_Map[XY_TO_INDEX((x - 1) % m_Size.width, (y) % m_Size.height)] == m_alive)count++;
	if (m_Map[XY_TO_INDEX((x + 1) % m_Size.width, (y) % m_Size.height)] == m_alive)count++;
	if (m_Map[XY_TO_INDEX((x - 1) % m_Size.width, (y + 1) % m_Size.height)] == m_alive)count++;
	if (m_Map[XY_TO_INDEX((x) % m_Size.width, (y + 1) % m_Size.height)] == m_alive)count++;
	if (m_Map[XY_TO_INDEX((x + 1) % m_Size.width, (y + 1) % m_Size.height)] == m_alive)count++;

	/*
	if (m_Map[(y + 1) % m_Size.height][(x - 1) % m_Size.width] == m_alive)count++;
	if (m_Map[(y) % m_Size.height][(x - 1) % m_Size.width] == m_alive)count++;
	if (m_Map[(y - 1) % m_Size.height][(x - 1) % m_Size.width] == m_alive)count++;
	if (m_Map[(y + 1) % m_Size.height][(x) % m_Size.width] == m_alive)count++;
	if (m_Map[(y - 1) % m_Size.height][(x) % m_Size.width] == m_alive)count++;
	if (m_Map[(y + 1) % m_Size.height][(x + 1) % m_Size.width] == m_alive)count++;
	if (m_Map[(y) % m_Size.height][(x + 1) % m_Size.width] == m_alive)count++;
	if (m_Map[(y - 1) % m_Size.height][(x + 1) % m_Size.width] == m_alive)count++;
	*/

	return count;
}

int GOLMap::nextGen()
{
	for (int y = 0; y < m_Size.height; y++) {
		for (int x = 0; x < m_Size.width; x++) {
			int nrOfN = getNrOfNeighbors(x, y);
			//Eine tote Zelle mit genau drei lebenden Nachbarn wird in der Folgegeneration neu geboren.
			//Eine lebende Zelle mit zwei oder drei lebenden Nachbarn bleibt in der Folgegeneration am Leben
			if (nrOfN == 3) {
				m_Buffermap[XY_TO_INDEX(x,y)] = m_alive;
				//m_Buffermap[y][x] = m_alive;
			}
			//Lebende Zellen mit weniger als zwei lebenden Nachbarn sterben in der Folgegeneration an Einsamkeit.
			//Lebende Zellen mit mehr als drei lebenden Nachbarn sterben in der Folgegeneration an Überbevölkerung.
			if (nrOfN < 2 || nrOfN > 3) {
				m_Buffermap[XY_TO_INDEX(x, y)] = m_dead;
				//m_Buffermap[y][x] = m_dead;

			}
		}
	}

	copyMap(m_Buffermap, m_Map);

	return ++m_Generations;
}

void GOLMap::printToFile(std::string path)
{
	std::ofstream outfile;
	outfile.open(path);
	outfile << m_Size.width << "," << m_Size.height << "\n";
	outfile << *this;
	outfile.close();
}

std::ostream& operator<<(std::ostream& strm, const GOLMap& map)
{
	Size m_Size = map.m_Size;

	for (int y = 0; y < map.m_Size.height - 1; y++) {
		for (int x = 0; x < map.m_Size.width; x++) {
			strm << map.m_Map[XY_TO_INDEX(x, y)];
			//strm << map.m_Map[y][x];
		}
		strm << std::endl;
	}

	for (int x = 0; x < map.m_Size.width; x++) {
		strm << map.m_Map[XY_TO_INDEX(x, map.m_Size.height - 1)];
		//strm << map.m_Map[map.m_Size.height - 1][x];
	}

	return strm;
}
