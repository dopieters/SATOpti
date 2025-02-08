#include "DebugMode.h"
#include <cassert>
#include "Params.h"

#include "MenuInterface.h"

#include "DebugDrawMode.h"


DebugMode::DebugMode():
	m_QuitDebugMode(false)
{
}

DebugMode::~DebugMode()
{
}

void DebugMode::Run()
{

	MenuInterface menu("Choose a debug mode");
	menu.AddOptions("Debug with drawing", [&]() {RunDrawDebugMode();});
	menu.AddOptions("Scan method consistency", [&]() {RunDebugMethodConsistency();});
	menu.AddOptions("Load event", [&]() {RunDebugMethodConsistency();});
	menu.AddOptions("Quit", [&]() {Quit();});

	while (!m_QuitDebugMode) {
		menu.RunInterface();
	}

}


void DebugMode::RunDrawDebugMode()
{
	DebugDrawMode db;
	if (db.IsValid()) {
		db.Run();
	}
}

void DebugMode::RunDebugMethodConsistency()
{
}

void DebugMode::RunLoadEvent()
{
}
