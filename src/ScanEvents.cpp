#include "ScanEvents.h"




ScanEvents::ScanEvents(std::vector<std::pair<Polygon, Polygon>> pairPolygons):
DrawWindow(),
m_EventToDraw(pairPolygons)
{
}

ScanEvents::~ScanEvents() {
	m_EventToDraw.clear();
}

void ScanEvents::ScanPairOfPolygons(){
	while (currentPolPair >= 0 && currentPolPair < m_EventToDraw.size()) {
		SDL_RenderClear(m_renderer);

		// black background
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		SDL_RenderClear(m_renderer);

		// draw polygons
		DrawPolygonPairs();

		// Present the backbuffer
		SDL_RenderPresent(m_renderer);

		ProcessEvents();
	}
}

void ScanEvents::ProcessEvents(){
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			currentPolPair = -1;
		}
		else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym)
			{
			case SDLK_RETURN:
				++currentPolPair;
				break;
			}
		}
	}
}

void ScanEvents::DrawPolygonPairs(){
	// Set polygon 1 edges draw color
	SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
	DrawPolygon(m_EventToDraw[currentPolPair].first);

	// set pol 2 draw color
	SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
	DrawPolygon(m_EventToDraw[currentPolPair].second);
}