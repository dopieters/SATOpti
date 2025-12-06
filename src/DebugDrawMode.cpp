#include "DebugDrawMode.h"
#include <cassert>

#include "Utilities.h"
#include "Params.h"




DebugDrawMode::DebugDrawMode() :
DrawWindow()
{

}

DebugDrawMode::~DebugDrawMode()
{

}

void DebugDrawMode::Run()
{
	bRunning = true;
	PrintCommand();

	MakePolygons();

	while (bRunning) {
		SDL_RenderClear(pRenderer);
		ProcessEvents();

		// black background
		SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
		SDL_RenderClear(pRenderer);

		// draw polygons
		DrawPolygons();

		// Present the backbuffer
		SDL_RenderPresent(pRenderer);
	}
}



void DebugDrawMode::DrawPolygons() const
{
	// Set polygon 1 edges draw color
	if (!bPolIntersect)
		SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);
	else
		SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);

	DrawPolygon(PolygonA);

	// set pol 2 draw color
	if (!bPolIntersect)
		SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255);

	DrawPolygon(PolygonB);

	if (bShowMinPol)
	{
		// set reduced polygon draw color and draw
		SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
		DrawPolygon(ReducedPolygonA);
		DrawPolygon(ReducedPolygonB);

		if (bShowMinPolDebug) {

			// Draw barycenter to barycenter line
			SDL_SetRenderDrawColor(pRenderer, 255, 255, 0, 255);
			SDL_RenderDrawLine(pRenderer, PolygonA.Barycenter.x * DRAW_SCALE + SHIFTX,
				PolygonA.Barycenter.y * DRAW_SCALE + SHIFTY,
				PolygonB.Barycenter.x * DRAW_SCALE + SHIFTX,
				PolygonB.Barycenter.y * DRAW_SCALE + SHIFTY);

			Geom::Vector barAxis = PolygonB.Barycenter - PolygonA.Barycenter;
			//barAxis = barAxis / barAxis.Mag();

			SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);
			auto pol1Proj = GetMinMaxPolygonProjAxis(PolygonA, barAxis);
			DrawHyperPlanes(barAxis, pol1Proj.first, pol1Proj.second);

			SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255);
			auto pol2Proj = GetMinMaxPolygonProjAxis(PolygonB, barAxis);
			DrawHyperPlanes(barAxis, pol2Proj.first, pol2Proj.second);
		}

	}
}


void DebugDrawMode::PrintCommand() const
{
	ClearTerminal();

	std::cout << "COMMAND \n";
	std::cout << "ENTER : Generate new polygon \n";
	std::cout << "BACKSPACE : Back to main menu \n";
	std::cout << "+ : Increase the number of vertex (Need to be re-generated) \n";
	std::cout << "- : Decrease the number of vertex (the minimum is 3 Vertices) \n";
	std::cout << "m : toggle min polygon view \n";
	std::cout << "d : toggle min pol debug helper (min polygon view need to be on) \n";

}

void DebugDrawMode::ProcessEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			bRunning = false;
		}
		else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym)
			{
			case SDLK_RETURN:
				MakePolygons();
				break;
			case SDLK_BACKSPACE:
				bRunning = false;
				break;
			case SDLK_PLUS:
				++iVertices;
				break;
			case SDLK_MINUS:
				--iVertices;
				iVertices = std::max(3, iVertices);
				break;

			case SDLK_SEMICOLON: // For my own keyboard layout
				if ((SDL_GetModState() & KMOD_SHIFT)) {
					++iVertices;
				}
				break;
			case SDLK_m:
				bShowMinPol = !bShowMinPol;
				break;
			case SDLK_d:
				bShowMinPolDebug = !bShowMinPolDebug;
			default:
				break;
			}
		}
	}
}

void DebugDrawMode::MakePolygons()
{
	PrintCommand();

	PolygonA.Vertices.clear();
	PolygonB.Vertices.clear();
	ReducedPolygonA.Vertices.clear();
	ReducedPolygonB.Vertices.clear();

	PolygonA = Geom::MakeConvexPol(iVertices);
	PolygonB = Geom::MakeConvexPol(iVertices);

	bPolIntersect = DoPolygonsIntersects(PolygonA, PolygonB);


	Geom::Vector barAxis = PolygonB.Barycenter - PolygonA.Barycenter;
	//barAxis = barAxis / barAxis.Mag();

	auto AProj = GetMinMaxPolygonProjAxis(PolygonA, barAxis);
	auto BProj = GetMinMaxPolygonProjAxis(PolygonB, barAxis);

	// check if this axis is not a separating axis
	if (AProj.first > BProj.second || AProj.second < BProj.first) {
		return;
	}

	// compute reduced polygon
	ReducedPolygonA = PolygonComputeReducePol(PolygonA, barAxis, BProj.first, true);
	ReducedPolygonB = PolygonComputeReducePol(PolygonB, barAxis, AProj.second, false);



}
