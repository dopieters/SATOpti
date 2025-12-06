#pragma once
#include <string>
#include <vector>
#include <functional>


// class used for the different menus
class MenuInterface {
public:
	MenuInterface(const std::string& InMenuText);
	void AddOptions(const std::string& InOptStr, std::function<void()> InOptFcn);

	void RunInterface();

private:
	std::string sMenuText;
	std::vector<std::string> sOptions;
	std::vector<std::function<void()>> MenuFunctions;


private:
	void NotAValidOption();

};