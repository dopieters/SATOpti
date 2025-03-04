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
	m_isRunning = true;

	MakePolygons();

	while (m_isRunning) {
		SDL_RenderClear(m_renderer);
		ProcessEvents();

		// black background
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		SDL_RenderClear(m_renderer);

		// draw polygons
		{
			SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
			DrawPolygon(pol1);

			SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
			DrawPolygon(pol2);
		}

		// Draw simplex
		{
			SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 255);
			DrawSimplex(simp);
		}


		// Draw axis
		SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
		DrawOriginAxis();

		// Present the backbuffer
		SDL_RenderPresent(m_renderer);
	}
}

void DebugDrawModeGJK::ProcessEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			m_isRunning = false;
		}
		else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym)
			{
			case SDLK_RETURN:
				UpdateSimplex();
				break;
			case SDLK_BACKSPACE:
				m_isRunning = false;
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
	pol1.vertices.clear();
	pol2.vertices.clear();
	simp.m_size = 0;


	pol1 = MakeConvexPol(100);
	pol2 = MakeConvexPol(100);


}

void DebugDrawModeGJK::UpdateSimplex()
{
	if (simp.m_size == 0) {
		simp.Add(GetFurthestPoint(pol1, dir) - GetFurthestPoint(pol2, -dir));
		dir = -simp.vertices[0];
	}
	else {
		const Point simpSup = GetFurthestPoint(pol1, dir) - GetFurthestPoint(pol2, -dir);
		if (DotProduct(simpSup, dir) < 0) {
			std::cout << "No collision" << std::endl;
			return;
		}

		simp.Add(simpSup);

		if (simp.UpdateSimplex(dir)) {
			std::cout << "Collision" << std::endl;
		}

	}
	

}
