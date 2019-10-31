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
				m_Map[XY_TO_INDEX(charNr, lineNr)] = (line.c_str()[charNr] != m_charDead) ? true : false;
				m_Buffermap[XY_TO_INDEX(charNr, lineNr)] = (line.c_str()[charNr] != m_charDead) ? true : false;
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

inline void GOLMap::initMap(int width, int height)
{
	m_Size.width = width;
	m_Size.height = height;

	m_Map = new bool[m_Size.height * m_Size.width];
	m_Buffermap = new bool[m_Size.height * m_Size.width];

}


void GOLMap::copyMap(bool* from, bool* to)
{
	memcpy(to, from, sizeof(bool) * m_Size.height * m_Size.width);
}

GOLMap::~GOLMap()
{
	delete[] m_Map;

	delete[] m_Buffermap;
}


inline int GOLMap::getNrOfNeighbors(int x, int y)
{
	int line1 = ((y - 1) < 0) ? m_Size.height - 1 : (y - 1);
	int line3 = ((y + 1) >= m_Size.height) ? 0 : (y + 1);

	int row1 = ((x - 1) < 0) ? m_Size.width - 1 : (x - 1);
	int row3 = ((x + 1) >= m_Size.width) ? 0 : (x + 1);


	return m_Map[XY_TO_INDEX(row1, line1)]
		+ m_Map[XY_TO_INDEX(x, line1)]
		+ m_Map[XY_TO_INDEX(row3, line1)]
		+ m_Map[XY_TO_INDEX(row1, y)]
		+ m_Map[XY_TO_INDEX(row3, y)]
		+ m_Map[XY_TO_INDEX(row1, line3)]
		+ m_Map[XY_TO_INDEX(x, line3)]
		+ m_Map[XY_TO_INDEX(row3, line3)];
}

inline int GOLMap::getNrOfNeighbors(int index)
{
	return getNrOfNeighbors(index - ((index / m_Size.width) * m_Size.width), (index / m_Size.width));
}

void GOLMap::run(int gen)
{
	int index = 0;
	int nrOfN = 0;

	for (int g = 0; g < gen; g++) {
		
		for (int y = 0; y < m_Size.height; y++) {
			for (int x = 0; x < m_Size.width; x++) {
				nrOfN = getNrOfNeighbors(x, y);
				index = XY_TO_INDEX(x, y);
				//Eine tote Zelle mit genau drei lebenden Nachbarn wird in der Folgegeneration neu geboren.
				//Eine lebende Zelle mit zwei oder drei lebenden Nachbarn bleibt in der Folgegeneration am Leben
				if (nrOfN == 3) {
					m_Buffermap[index] = m_alive;
				}
				//Lebende Zellen mit weniger als zwei lebenden Nachbarn sterben in der Folgegeneration an Einsamkeit.
				//Lebende Zellen mit mehr als drei lebenden Nachbarn sterben in der Folgegeneration an Überbevölkerung.
				if (nrOfN < 2 || nrOfN > 3) {
					m_Buffermap[index] = m_dead;
				}
			}
		}
		
		copyMap(m_Buffermap, m_Map);
	}
}

void GOLMap::printToFile(std::string path)
{
	std::ofstream outfile;
	outfile.open(path);
	outfile << m_Size.width << "," << m_Size.height << "\n";
	outfile << *this;
	outfile.close();
}

char GOLMap::getCell(int x, int y) const
{
	return (m_Map[XY_TO_INDEX(x, y)]) ? m_charAlive : m_charDead;
}

std::ostream& operator<<(std::ostream & strm, const GOLMap & map)
{
	Size m_Size = map.m_Size;

	for (int y = 0; y < map.m_Size.height - 1; y++) {
		for (int x = 0; x < map.m_Size.width; x++) {
			strm << map.getCell(x, y);
		}
		strm << std::endl;
	}

	for (int x = 0; x < map.m_Size.width; x++) {
		strm << map.getCell(x, map.m_Size.height - 1);
	}

	return strm;
}
