#include <SDL2/SDL.h>
#include <iostream>

#include "Cust_Window.h"


#undef main




int main(int argc, char* argv[])
{	
	Cust_Window* wnd = new Cust_Window();
	if (wnd->IsValid()) {
		wnd->Run();
	}
	return 0;

}