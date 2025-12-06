#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include"Geom.h"

struct Polygon;




// Draw debug mode interface used to call the different options

class DebugMode
{
public:
	DebugMode();
	~DebugMode();

	// main function to run the debug mode
	void Run();


private: 
	bool bQuitDebugMode = false;

	// Run the SAT debugging mode
	void RunDrawDebugMode();

	// Run the GJK debugging mode
	void RunDrawDebugGJK();

	// Run a scanning to check consistencies between the differents 
	// Methods
	void RunDebugMethodConsistency();


	// Run events that was previously saved
	// for debuggin special cases
	void RunLoadEvent();


	void Quit() { bQuitDebugMode = true; }

};

