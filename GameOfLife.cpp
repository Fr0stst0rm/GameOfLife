#include <iostream>
#include "GOLMap.h"

#include <chrono>
#include <thread>

int generations = 250;
std::string load_name;
std::string save_name;
bool should_measure = false;
bool is_sequential;

int main(int argc, char * argv[])
{
	std::chrono::milliseconds startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	std::chrono::milliseconds initTime;
	std::chrono::milliseconds kernelTime;
	std::chrono::milliseconds finalTime;

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

	initTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	for (int i = 0; i < generations; i++) {


		//system("cls");
		//std::cout << *map << "\n";
		map->nextGen();

		//std::this_thread::sleep_for(std::chrono::milliseconds(750));
	}

	//system("cls");
	//std::cout << *map << "\n";


	kernelTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	map->printToFile("out.gol");

	delete map;
	finalTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	std::cout << "Finished!\n";

	std::cout << "Init time: " << (initTime - startTime).count() << "ms\n";
	std::cout << "Kenel time: " << (kernelTime - initTime).count() << "ms\n";
	std::cout << "Final time: " << (finalTime - kernelTime).count() << "ms\n";

	std::cout << "Total time: " << (finalTime - startTime).count() << "ms\n";
}
