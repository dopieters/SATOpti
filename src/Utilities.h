#pragma once
#include <iostream>

inline void ClearTerminal() {
	std::cout << "\x1B[2J\x1B[H"; // ANSI escape code
}
