#include "GOLMap.h"

//Eine tote Zelle mit genau drei lebenden Nachbarn wird in der Folgegeneration neu geboren.
//Eine lebende Zelle mit zwei oder drei lebenden Nachbarn bleibt in der Folgegeneration am Leben
//Lebende Zellen mit weniger als zwei lebenden Nachbarn sterben in der Folgegeneration an Einsamkeit.
//Lebende Zellen mit mehr als drei lebenden Nachbarn sterben in der Folgegeneration an Überbevölkerung.
const bool lookup[2][9] = {
	{0,0,0,1,0,0,0,0,0},
	{0,0,1,1,0,0,0,0,0}
};

GOLMap::GOLMap()
{
	initMap(0, 0);
}

GOLMap::GOLMap(std::string path)
{
	loadFromFile(path);
}

GOLMap::GOLMap(int width, int height)
{
	initMap(width, height);
}

GOLMap::GOLMap(Size size)
{
	initMap(size.width, size.height);
}

void GOLMap::loadFromFile(std::string path) {
	int width = 0;
	int height = 0;

	std::ifstream inFile;
	inFile.open(path);

	if (!inFile.good()) {
		std::cout << "File can not be opened\n";
		exit(0);
	}

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
				m_MapBuffer[m_MapSwitch][XY_TO_INDEX(charNr, lineNr)] = (line.c_str()[charNr] != m_charDead) ? true : false;
			}
		}
	}
	inFile.close();

	memcpy(m_MapBuffer[!m_MapSwitch], m_MapBuffer[m_MapSwitch], sizeof(bool) * m_Size.height * m_Size.width);
}

inline void GOLMap::initMap(int width, int height)
{
	m_Size.width = width;
	m_Size.height = height;

	m_MapBuffer = new bool* [2];
	m_MapBuffer[0] = new bool[m_Size.height * m_Size.width];
	m_MapBuffer[1] = new bool[m_Size.height * m_Size.width];

}


GOLMap::~GOLMap()
{
	delete[] m_MapBuffer[0];
	delete[] m_MapBuffer[1];

	delete[] m_MapBuffer;
}


inline int GOLMap::getNrOfNeighbors(int x, int y)
{
	int line1 = ((y - 1) < 0) ? m_Size.height - 1 : (y - 1);
	int line3 = ((y + 1) >= m_Size.height) ? 0 : (y + 1);

	int row1 = ((x - 1) < 0) ? m_Size.width - 1 : (x - 1);
	int row3 = ((x + 1) >= m_Size.width) ? 0 : (x + 1);

	return m_MapBuffer[m_MapSwitch][XY_TO_INDEX(row1, line1)]
		+ m_MapBuffer[m_MapSwitch][XY_TO_INDEX(x, line1)]
		+ m_MapBuffer[m_MapSwitch][XY_TO_INDEX(row3, line1)]
		+ m_MapBuffer[m_MapSwitch][XY_TO_INDEX(row1, y)]
		+ m_MapBuffer[m_MapSwitch][XY_TO_INDEX(row3, y)]
		+ m_MapBuffer[m_MapSwitch][XY_TO_INDEX(row1, line3)]
		+ m_MapBuffer[m_MapSwitch][XY_TO_INDEX(x, line3)]
		+ m_MapBuffer[m_MapSwitch][XY_TO_INDEX(row3, line3)];
}

inline int GOLMap::getNrOfNeighbors(int index)
{
	return getNrOfNeighbors(index - ((index / m_Size.width) * m_Size.width), (index / m_Size.width));
}

void GOLMap::run()
{

	switch (m_mode) {
	case Mode::SEQUENTIAL:
		runSeq();
		break;
	case Mode::OPENMP:
		runOMP();
		break;
	case Mode::OPENCL:
		runOCL();
		break;
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
	return (m_MapBuffer[m_MapSwitch][XY_TO_INDEX(x, y)]) ? m_charAlive : m_charDead;
}

void GOLMap::runSeq() {
	int index = 0;
	int nrOfN = 0;
	for (int g = 0; g < m_generations; g++) {
		for (int y = 0; y < m_Size.height; y++) {
			for (int x = 0; x < m_Size.width; x++) {
				nrOfN = getNrOfNeighbors(x, y);

				index = XY_TO_INDEX(x, y);
				m_MapBuffer[!m_MapSwitch][index] = lookup[m_MapBuffer[m_MapSwitch][index]][nrOfN];
			}
		}
		m_MapSwitch = !m_MapSwitch;
	}
}
void GOLMap::runOMP() {
	int index = 0;
	int nrOfN = 0;
	if (m_threads > 0) {
		omp_set_dynamic(0);
		omp_set_num_threads(m_threads);
	}
	for (int g = 0; g < m_generations; g++) {
#pragma omp parallel for
		for (int y = 0; y < m_Size.height; y++) {
			for (int x = 0; x < m_Size.width; x++) {
				nrOfN = getNrOfNeighbors(x, y);

				index = XY_TO_INDEX(x, y);
				m_MapBuffer[!m_MapSwitch][index] = lookup[m_MapBuffer[m_MapSwitch][index]][nrOfN];
			}
		}
		m_MapSwitch = !m_MapSwitch;
	}
}
void GOLMap::runOCL() {

	std::ifstream inFile;
	inFile.open("golStep_kernel.cl");

	if (!inFile.good()) {
		std::cout << "Error! Kernel file can not be opened\n";
		exit(0);
	}

	std::string  line = "";
	std::stringstream clData("");

	while (!inFile.eof()) {
		std::getline(inFile, line);
		clData << line << "\n";
	}

	inFile.close();

	int index = 0;
	int nrOfN = 0;

	const std::string KERNEL_FILE = "golStep_kernel.cl";
	cl_int err = CL_SUCCESS;
	cl::Program program;
	std::vector<cl::Device> devices;

	try {

		// get available platforms ( NVIDIA, Intel, AMD,...)
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		if (platforms.size() == 0) {
			std::cout << "No OpenCL platforms available!\n";
			return;
		}

		// create a context and get available devices
		cl::Platform platform = platforms[m_platformId]; // on a different machine, you may have to select a different platform!
		cl_context_properties properties[] =
		{ CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[m_platformId])(), 0 };

		cl::Context context;

		if (m_device == Device::GPU) {
			context = cl::Context(CL_DEVICE_TYPE_GPU, properties);
		}
		else if (m_device == Device::CPU) {
			context = cl::Context(CL_DEVICE_TYPE_CPU, properties);
		}
		else {
			context = cl::Context(CL_DEVICE_TYPE_GPU, properties);
		}

		devices = context.getInfo<CL_CONTEXT_DEVICES>();

		// load and build the kernel
		std::ifstream sourceFile(KERNEL_FILE);
		if (!sourceFile)
		{
			std::cout << "kernel source file " << KERNEL_FILE << " not found!" << std::endl;
			return;
		}
		std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
		cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
		program = cl::Program(context, source);
		program.build(devices);

		size_t mapSize = (m_Size.width * m_Size.height);

		// create input and output data
		// input buffers
		cl::Buffer bufferIN = cl::Buffer(context, CL_MEM_READ_ONLY, mapSize);
		// output buffers
		cl::Buffer bufferOUT = cl::Buffer(context, CL_MEM_WRITE_ONLY, mapSize);

		// fill buffers
		cl::Kernel golKernel(program, "gol_Step", &err);
		cl::Event event;
		golKernel.setArg(0, bufferIN);
		golKernel.setArg(1, bufferOUT);
		golKernel.setArg(2, m_Size.width);
		golKernel.setArg(3, m_Size.height);

		cl::CommandQueue queue(context, devices[m_deviceId], 0, &err);

		// launch add kernel
		// Run the kernel on specific ND range
		cl::NDRange global(mapSize);
		cl::NDRange local(1); //make sure global range is divisible by local range
		cl::NDRange offset(0);
		//std::cout << "call 'gol_Step' kernel" << std::endl;

		for (int g = 0; g < m_generations; g++) {

			// Copy the lists A and B to their respective memory buffers
			queue.enqueueWriteBuffer(bufferIN, CL_FALSE, 0, mapSize, &m_MapBuffer[m_MapSwitch][0]);

			//Run kernel
			queue.enqueueNDRangeKernel(golKernel, offset, global, local);

			// Read the memory buffer C on the device to the local variable C
			queue.enqueueReadBuffer(bufferOUT, CL_FALSE, 0, mapSize, &m_MapBuffer[!m_MapSwitch][0]);

			m_MapSwitch = !m_MapSwitch;
		}
	}
	catch (cl::Error err) {
		// error handling
		// if the kernel has failed to compile, print the error log
		std::string s;
		program.getBuildInfo(devices[m_deviceId], CL_PROGRAM_BUILD_LOG, &s);
		std::cout << s << std::endl;
		program.getBuildInfo(devices[m_deviceId], CL_PROGRAM_BUILD_OPTIONS, &s);
		std::cout << s << std::endl;

		std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
	}

}

std::ostream& operator<<(std::ostream& strm, const GOLMap& map)
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
