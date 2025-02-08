#include "MenuInterface.h"
#include "Utilities.h"


MenuInterface::MenuInterface(const std::string &mainString):
m_MainString(mainString)
{
}

void MenuInterface::AddOptions(const std::string& optString, std::function<void()>optFunction) {
	m_StringOption.push_back(optString);
	m_MenuFunctions.push_back(optFunction);
}

void MenuInterface::RunInterface() {

	while (true) {
		std::cout << m_MainString << "\n";
		for (int ii = 0; ii < m_StringOption.size(); ++ii) {
			std::cout << ii << ". " << m_StringOption[ii] << "\n";
		}

		int input = -1;

		if (std::cin >> input									// input int
			&& input >= 0 && input < m_StringOption.size()		// valid input
			){
			ClearTerminal();
			m_MenuFunctions[input]();
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