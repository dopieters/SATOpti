#include "DebugDrawModeGJK.h"
#include <iostream>

DebugDrawModeGJK::DebugDrawModeGJK():
DrawWindow()
{

}

DebugDrawModeGJK::~DebugDrawModeGJK() {

}



void DebugDrawModeGJK::Run()
{
	bRunning = true;

	MakePolygons();

	while (bRunning) {
		SDL_RenderClear(pRenderer);
		ProcessEvents();

		// black background
		SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
		SDL_RenderClear(pRenderer);

		// draw polygons
		{
			SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
			DrawPolygon(PolygonA);

			SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255);
			DrawPolygon(PolygonB);
		}

		// Draw simplex
		{
			SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);
			DrawSimplex(GJKSimplex);
		}


		// Draw axis
		SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
		DrawOriginAxis();

		// Present the backbuffer
		SDL_RenderPresent(pRenderer);
	}
}

void DebugDrawModeGJK::ProcessEvents()
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
				UpdateSimplex();
				break;
			case SDLK_BACKSPACE:
				bRunning = false;
				break;
			case SDLK_r:
				MakePolygons();
				break;
			default:
				break;
			}
		}
	}
}

void DebugDrawModeGJK::MakePolygons()
{
	PolygonA.Vertices.clear();
	PolygonB.Vertices.clear();
	GJKSimplex.iSize = 0;


	PolygonA = Geom::MakeConvexPol(100);
	PolygonB = Geom::MakeConvexPol(100);


}

void DebugDrawModeGJK::UpdateSimplex()
{
	if (GJKSimplex.iSize == 0) {
		GJKSimplex.Add(Geom::GetFurthestPoint(PolygonA, GJKDir) - Geom::GetFurthestPoint(PolygonB, -GJKDir));
		GJKDir = -GJKSimplex.Vertices[0];
	}
	else {
		const Geom::Point simpSup = Geom::GetFurthestPoint(PolygonA, GJKDir) - Geom::GetFurthestPoint(PolygonB, -GJKDir);
		if (Geom::DotProduct(simpSup, GJKDir) < 0) {
			std::cout << "No collision" << std::endl;
			return;
		}

		GJKSimplex.Add(simpSup);

		if (GJKSimplex.UpdateSimplex(GJKDir)) {
			std::cout << "Collision" << std::endl;
		}

	}
	

}
