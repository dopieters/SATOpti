#include <SDL2/SDL.h>
#include <iostream>

#include <iostream> // cin, cout
#include "DebugMode.h"


#undef main

void ClearTerminal() {
	std::cout << "\x1B[2J\x1B[H"; // ANSI escape code
}

void RunDebugMode() {
	DebugMode db;
	if (db.IsValid()) {
		db.Run();
	}
}

void RunPerfComp() {

}

int main(int argc, char* argv[])
{	

	bool bRun = true;

	while (bRun) {
		std::cout << "Choose a mode \n";
		std::cout << "1. Debug \n" << "2. Performance comp \n" << "3. Quit" << std::endl;
		int input = 0;
		if (std::cin >> input) {

			switch (input) {
			case 1:
				RunDebugMode();
				break;
			case 2:
				RunPerfComp();
				break;
			case 3:
				bRun = false;
				break;
			default:
				std::cin.clear();
				ClearTerminal();
				std::cout << "Not a valid option, try again" << std::endl;
				break;
			}
		}
		else {
			ClearTerminal();
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Not a valid input, try again" << std::endl;
		}

	}



	
	return 0;

}