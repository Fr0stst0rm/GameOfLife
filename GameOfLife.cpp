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
bool is_sequential = true;

std::string formatTime(std::chrono::steady_clock::duration time);

int main(int argc, char* argv[])
{
	
	std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point initTime;
	std::chrono::steady_clock::time_point kernelTime;
	std::chrono::steady_clock::time_point finalTime;

	if (argc > 1)
	{
		for (int i = 0; i < argc; i++)
		{
			const std::string arg = argv[i];
			if (arg == "--load") {
				load_name = argv[i + 1];
				i++;
			}
			else if (arg == "--generations") {
				generations = std::stoi(argv[i + 1]);
				i++;
			}
			else if (arg == "--save") {
				save_name = argv[i + 1];
				i++;
			}
			else if (arg == "--measure") {
				should_measure = true;
			}
			else if (arg == "--mode") {
				std::string mode(argv[i + 1]);
				if (mode == "seq")
				{
					is_sequential = true;
				}
				i++;
			}
		}
	}
	std::cout << "Starting " << load_name << "\n";

	GOLMap* map = new GOLMap(load_name);

	initTime = std::chrono::high_resolution_clock::now();
	//if (should_measure) std::cout << "Init time:  " << formatTime(initTime - startTime) << "\n";
	if(should_measure) std::cout << formatTime(initTime - startTime) << "; ";
	
	map->run(generations);
	
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