#include "ScanEvents.h"
#include "EventIO.h"




ScanEvents::ScanEvents(std::vector<std::pair<Geom::Polygon, Geom::Polygon>> pairPolygons, bool save):
DrawWindow(),
PolygonPairs(pairPolygons),
bSave(save)
{
}

ScanEvents::~ScanEvents() {

	if (bSave) {
		SavePairs(PolygonPairs, "events.bin");
	}
	PolygonPairs.clear();
}

void ScanEvents::ScanPairOfPolygons(){
	DoPolygonColTests();
	while (iCurrentPair >= 0 && iCurrentPair < PolygonPairs.size()) {
		SDL_RenderClear(pRenderer);

		// black background
		SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
		SDL_RenderClear(pRenderer);

		// draw polygons
		DrawPolygonPairs();

		// Present the backbuffer
		SDL_RenderPresent(pRenderer);

		ProcessEvents();
	}
}

void ScanEvents::ProcessEvents(){
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			iCurrentPair = -1;
		}
		else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym)
			{
			case SDLK_RETURN:
				++iCurrentPair;
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
	SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
	DrawPolygon(PolygonPairs[iCurrentPair].first);

	// set pol 2 draw color
	SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255);
	DrawPolygon(PolygonPairs[iCurrentPair].second);


	SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
	DrawPolygon(RedPolygonsPair.first); DrawPolygon(RedPolygonsPair.second);


}

void ScanEvents::DoPolygonColTests()
{
	
	if (iCurrentPair >= 0 && iCurrentPair < PolygonPairs.size()) {
		auto& CurrentPolPair = PolygonPairs[iCurrentPair];
		DoPolygonsIntersects(CurrentPolPair.first, CurrentPolPair.second);


		

	
		// compute reduced polygon
		Geom::Vector barAxis = CurrentPolPair.second.Barycenter - CurrentPolPair.first.Barycenter;

		//barAxis = barAxis / barAxis.Mag();

		auto AProj = GetMinMaxPolygonProjAxis(CurrentPolPair.first, barAxis);
		auto BProj = GetMinMaxPolygonProjAxis(CurrentPolPair.second, barAxis);

		if (AProj.first > BProj.second || AProj.second < BProj.first) {
			RedPolygonsPair.first.Vertices.clear(); RedPolygonsPair.second.Vertices.clear();
			return;
		}

		RedPolygonsPair.first = PolygonComputeReducePol(CurrentPolPair.first, barAxis, BProj.first, true);
		RedPolygonsPair.second = PolygonComputeReducePol(CurrentPolPair.second, barAxis, AProj.second, false);

	}
}
