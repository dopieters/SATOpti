#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include"Geom.h"

struct Polygon;

class DebugMode
{
public:
	DebugMode();
	~DebugMode();

	// main function to run the debug mode
	void Run();


private: 
	bool m_QuitDebugMode = false;
	void RunDrawDebugMode();
	void RunDebugMethodConsistency();
	void RunLoadEvent();
	void Quit() { m_QuitDebugMode = true; }


	bool bDrawWndOpen = false;
	
};

