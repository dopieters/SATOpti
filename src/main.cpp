#include <iostream>
#include "Utilities.h"
#include "PerfTest.h"
#include "MenuInterface.h"
#include "DebugMode.h"

#undef main

bool bQuitProg = false;

void QuitProgram() {
	bQuitProg = true;
}


void RunDebugMode() {
	DebugMode db;
	db.Run();
};

void RunPerfComp() {
	PerfTest test(1000, {10, 50, 100, 500, 1000, 5000, 10000});
	// init seed
	srand(123);
	test.Run();


	std::cout << "Press Enter to continue..." << std::endl;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main(int argc, char* argv[])
{	

	MenuInterface menu("Choose a mode");
	menu.AddOptions("Debug mode", &RunDebugMode);
	menu.AddOptions("Performance comparizon", &RunPerfComp);
	menu.AddOptions("Quit", &QuitProgram);

	while (!bQuitProg) {
		menu.RunInterface();
	}

	return 0;

}