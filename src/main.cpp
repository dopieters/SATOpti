#include <iostream>
#include "Utilities.h"
#include "PerfTest.h"
#include "MenuInterface.h"
#include "DebugMode.h"

#undef main


void RunDebugMode() {
	DebugMode db;
	db.Run();
};



void RunPerfComp() {
	PerfTest Tests(100000, { 5, 10, 25, 50, 75, 100, 200, 300, 400, 500});//, 1000, 2500, 5000, 7500, 10000});
	// init seed
	srand(123);
	Tests.Run();


	std::cout << "Press Enter to continue..." << std::endl;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


int main(int argc, char* argv[])
{	
	bool IsQuit = false;

	auto Quit = [&IsQuit]() {
		IsQuit = true;
		};


	MenuInterface MainMenu("Choose a mode");
	MainMenu.AddOptions("Debug mode", &RunDebugMode);
	MainMenu.AddOptions("Performance comparizon", &RunPerfComp);
	MainMenu.AddOptions("Quit", Quit);

	while (!IsQuit) {
		MainMenu.RunInterface();
	}

	return 0;

}