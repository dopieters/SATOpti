#pragma once
#include <string>
#include <vector>
#include <functional>

class MenuInterface {
public:
	MenuInterface(const std::string& mainString);
	void AddOptions(const std::string& optString, std::function<void()>optFunction);

	void RunInterface();

private:
	std::string m_MainString;
	std::vector<std::string> m_StringOption;
	std::vector<std::function<void()>> m_MenuFunctions;


private:
	void NotAValidOption();

};