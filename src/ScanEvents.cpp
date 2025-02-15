#include "ScanEvents.h"
#include "EventIO.h"




ScanEvents::ScanEvents(std::vector<std::pair<Polygon, Polygon>> pairPolygons, bool save):
DrawWindow(),
m_EventToDraw(pairPolygons),
m_isSave(save)
{
}

ScanEvents::~ScanEvents() {

	if (m_isSave) {
		savePairsOfPolygons(m_EventToDraw, "events.bin");
	}
	m_EventToDraw.clear();
}

void ScanEvents::ScanPairOfPolygons(){
	DoPolygonColTests();
	while (m_CurrentPolPair >= 0 && m_CurrentPolPair < m_EventToDraw.size()) {
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
			m_CurrentPolPair = -1;
		}
		else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym)
			{
			case SDLK_RETURN:
				++m_CurrentPolPair;
				DoPolygonColTests();
				break;
			default:
				CameraMovementEvents(event);
			}
		}
	}
}

void ScanEvents::DrawPolygonPairs(){
	// Set polygon 1 edges draw color
	SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
	DrawPolygon(m_EventToDraw[m_CurrentPolPair].first);

	// set pol 2 draw color
	SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
	DrawPolygon(m_EventToDraw[m_CurrentPolPair].second);


	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
	DrawPolygon(pairOfRedPol.first); DrawPolygon(pairOfRedPol.second);


}

void ScanEvents::DoPolygonColTests()
{
	
	if (m_CurrentPolPair >= 0 && m_CurrentPolPair < m_EventToDraw.size()) {
		auto& CurrentPolPair = m_EventToDraw[m_CurrentPolPair];
		DoPolygonsIntersects(CurrentPolPair.first, CurrentPolPair.second);


		

	
		// compute reduced polygon
		Vector barAxis = CurrentPolPair.second.baryCenter - CurrentPolPair.first.baryCenter;

		//barAxis = barAxis / barAxis.Mag();

		auto AProj = GetMinMaxPolygonProjAxis(CurrentPolPair.first, barAxis);
		auto BProj = GetMinMaxPolygonProjAxis(CurrentPolPair.second, barAxis);

		if (AProj.first > BProj.second || AProj.second < BProj.first) {
			pairOfRedPol.first.vertices.clear(); pairOfRedPol.second.vertices.clear();
			return;
		}

		pairOfRedPol.first = PolygonComputeReducePol(CurrentPolPair.first, barAxis, BProj.first, true);
		pairOfRedPol.second = PolygonComputeReducePol(CurrentPolPair.second, barAxis, AProj.second, false);

	}
}
