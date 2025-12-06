#include "MenuInterface.h"
#include "Utilities.h"


MenuInterface::MenuInterface(const std::string& InMenuText):
sMenuText(InMenuText)
{
}

void MenuInterface::AddOptions(const std::string& InOptStr, std::function<void()>InOptFcn) {
	sOptions.push_back(InOptStr);
	MenuFunctions.push_back(InOptFcn);
}

void MenuInterface::RunInterface() {

	while (true) {
		std::cout << sMenuText << "\n";
		for (int ii = 0; ii < sOptions.size(); ++ii) {
			std::cout << ii << ". " << sOptions[ii] << "\n";
		}

		int input = -1;

		if (std::cin >> input									// input int
			&& input >= 0 && input < sOptions.size()		// valid input
			){
			ClearTerminal();
			MenuFunctions[input]();
			break;
		}
		else {
			NotAValidOption();
		}
	}
	


}

void MenuInterface::NotAValidOption() {
	ClearTerminal();
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cout << "Not a valid input, try again" << std::endl;
}