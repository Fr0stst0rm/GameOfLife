#include <iostream>
#include "GOLMap.h"
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>

int generations = 250;
std::string load_name;
std::string save_name;
bool should_measure = false;
bool is_sequential;

std::string formatTime(std::chrono::steady_clock::duration time);

int main(int argc, char* argv[])
{
	std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point initTime;
	std::chrono::steady_clock::time_point kernelTime;
	std::chrono::steady_clock::time_point finalTime;

	if (argc > 1)
	{
		for (int i = 0; i < argc; i++)//argv[i]
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

	GOLMap* map = new GOLMap("maps/random250_in.gol");
	//GOLMap* map = new GOLMap("maps/test.gol");

	initTime = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < generations; i++) {


		//system("cls");
		//std::cout << *map << "\n";
		map->nextGen();

		//std::this_thread::sleep_for(std::chrono::milliseconds(750));
	}

	//system("cls");
	//std::cout << *map << "\n";


	kernelTime = std::chrono::high_resolution_clock::now();

	map->printToFile("out.gol");

	delete map;
	finalTime = std::chrono::high_resolution_clock::now();

	std::cout << "Finished!\n";

	std::cout << "Init time:  " << formatTime(initTime - startTime) << "\n";
	std::cout << "Kenel time: " << formatTime(kernelTime - initTime) << "\n";
	std::cout << "Final time: " << formatTime(finalTime - kernelTime) << "\n";

	std::cout << "Total time: " << formatTime(finalTime - startTime) << "\n";
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