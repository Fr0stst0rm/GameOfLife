#include <iostream>
#include "GOLMap.h"
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>

int generations = 250;
std::string load_name = "maps/random10000_in.gol";
std::string save_name = "out.gol";
bool should_measure = true;


std::string formatTime(std::chrono::steady_clock::duration time);

int main(int argc, char* argv[])
{
	std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point initTime;
	std::chrono::steady_clock::time_point kernelTime;
	std::chrono::steady_clock::time_point finalTime;

	GOLMap* map = new GOLMap();
	map->m_generations = 250;
	

	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			const std::string arg = argv[i];
			if (arg == "--load") {
				load_name = argv[++i];
			}
			else if (arg == "--generations") {
				map->m_generations = std::stoi(argv[++i]);
			}
			else if (arg == "--save") {
				save_name = argv[++i];
			}
			else if (arg == "--measure") {
				should_measure = true;
			}
			else if (arg == "--mode") {
				std::string m(argv[++i]);
				if (m == "omp") {
					map->m_mode = Mode::OPENMP;
				}
				else if(m == "ocl") {
					map->m_mode = Mode::OPENCL;
				}
			}
			else if (arg == "--threads") {
				map->m_threads = std::stoi(argv[++i]);
			}
			else if (arg == "--device") {
				if (argv[++i] == "gpu") {
					map->m_device = Device::GPU;
				}
			}
			else if (arg == "--platformId") {
				map->m_platformId = std::stoi(argv[++i]);
			}
			else if (arg == "--deviceId") {
				map->m_deviceId = std::stoi(argv[++i]);
			}
			else {
				std::cout << "--load NAME (where NAME is a filename with the extension '.gol')\n";
				std::cout << "--generations NUM	(where NUM is a number)\n";
				std::cout << "--save NAME (where NAME is a filename with the extension '.gol')\n";
				std::cout << "--measure (generates measurement 	output on stdout)\n";
				std::cout << "--mode seq (select sequential version)\n";
				std::cout << "--mode omp --threads <NUM> (select parallel OpenMP version)\n";
				std::cout << "--mode ocl (select parallel OpenCL version)\n";
				std::cout << "--device cpu|gpu (automatically selects platform & device)\n";
				std::cout << "--platformId NUM (user-defined selection of platform)\n";
				std::cout << "--deviceId NUM (user-defined selection of device on platform)\n";
				return 0;
			}
		}
	}

	std::cout << "Starting " << load_name << "\n";
	map->loadFromFile(load_name);

	map->m_mode = Mode::OPENCL;
	//map->m_device = Device::CPU;
	
	initTime = std::chrono::high_resolution_clock::now();
	//if (should_measure) std::cout << "Init time:  " << formatTime(initTime - startTime) << "\n";
	if(should_measure) std::cout << formatTime(initTime - startTime) << "; ";
	
	map->run();
	
	kernelTime = std::chrono::high_resolution_clock::now();
	//if (should_measure) std::cout << "Kernel time: " << formatTime(kernelTime - initTime) << "\n";
	if (should_measure) std::cout << formatTime(kernelTime - initTime) << "; ";

	map->printToFile(save_name);

	delete map;
	finalTime = std::chrono::high_resolution_clock::now();
	//if (should_measure) std::cout << "Final time: " << formatTime(finalTime - kernelTime) << "\n";
	if (should_measure) std::cout << formatTime(finalTime - kernelTime) << ";\n";

	std::cout << "Finished!\n";


	if (should_measure) std::cout << "Total time: " << formatTime(finalTime - startTime) << "\n";
}

std::string formatTime(std::chrono::steady_clock::duration time) {
	std::chrono::hours h = std::chrono::duration_cast<std::chrono::hours>(time);
	time -= h;
	std::chrono::minutes m = std::chrono::duration_cast<std::chrono::minutes>(time);
	time -= m;
	std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(time);
	time -= s;
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(2) << h.count() << ":" << std::setw(2) << m.count() << ":" << std::setw(2) << s.count() << "." << std::setw(3) << ms.count();
	return ss.str();
}