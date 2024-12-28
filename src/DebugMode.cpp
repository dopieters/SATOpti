#include "DebugMode.h"
#include <cassert>

#include "Utilities.h"
#include "Params.h"




DebugMode::DebugMode():
	m_window(nullptr), m_renderer(nullptr), m_isValid(false)
{


	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return;
	}

	// Create a window
	m_window = SDL_CreateWindow("Hello SDL",
		100, 100, W_WIDTH, W_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (m_window == nullptr)
	{
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return;
	}

	// Create a renderer
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (m_renderer == nullptr)
	{
		SDL_DestroyWindow(m_window);
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return;
	}

	// init seed
	srand(RDN_SEED);

	// properly managed to allocate needed memory
	m_isValid = true;
}

DebugMode::~DebugMode()
{
	if (!m_isValid) return;


	// Cleanup
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();

	m_renderer = nullptr;
	m_window = nullptr;
}

void DebugMode::Run()
{
	m_isRunning = true;
	PrintCommand();

	MakePolygons();
	 
	while (m_isRunning) {
		SDL_RenderClear(m_renderer);
		ProcessEvents();

		// black background
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		SDL_RenderClear(m_renderer);

		// draw polygons
		DrawPolygons();

		// Present the backbuffer
		SDL_RenderPresent(m_renderer);
	}
}



void DebugMode::DrawPolygons() const
{
	// Set polygon 1 edges draw color
	if (!m_isPolIntersect)
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 255);
	else
		SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);

	DrawPolygon(pol1);

	// set pol 2 draw color
	if (!m_isPolIntersect)
		SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);

	DrawPolygon(pol2);	

	if (m_isShowMinPol)
	{
		// set reduced polygon draw color and draw
		SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
		DrawPolygon(pol1Red);
		DrawPolygon(pol2Red);

		if (m_isShowMinPolDebug) {

			// Draw barycenter to barycenter line
			SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255);
			SDL_RenderDrawLine(m_renderer, pol1.baryCenter.x * DRAW_SCALE + SHIFTX,
				pol1.baryCenter.y * DRAW_SCALE + SHIFTY,
				pol2.baryCenter.x * DRAW_SCALE + SHIFTX,
				pol2.baryCenter.y * DRAW_SCALE + SHIFTY);
			
			Vector barAxis = pol2.baryCenter - pol1.baryCenter;
			barAxis = barAxis / barAxis.Mag();
			
			SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 255);
			auto pol1Proj = GetMinMaxPolygonProjAxis(pol1, barAxis);
			DrawHyperPlanes(barAxis, pol1Proj.first, pol1Proj.second);

			SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
			auto pol2Proj = GetMinMaxPolygonProjAxis(pol2, barAxis);
			DrawHyperPlanes(barAxis, pol2Proj.first, pol2Proj.second);
		}

	}


	

}

void DebugMode::DrawPolygon(const Polygon& A) const
{
	const int nVert = A.vertices.size();

	if (nVert < 3) return;


	for (int ii = 0; ii < nVert - 1; ++ii) {
		SDL_RenderDrawLine(m_renderer, A.vertices[ii].x * DRAW_SCALE + SHIFTX,
			A.vertices[ii].y * DRAW_SCALE + SHIFTY,
			A.vertices[ii + 1].x * DRAW_SCALE + SHIFTX,
			A.vertices[ii + 1].y * DRAW_SCALE + SHIFTY);
	}
	SDL_RenderDrawLine(m_renderer, A.vertices[nVert - 1].x * DRAW_SCALE + SHIFTX,
		A.vertices[nVert - 1].y * DRAW_SCALE + SHIFTY,
		A.vertices[0].x * DRAW_SCALE + SHIFTX,
		A.vertices[0].y * DRAW_SCALE + SHIFTY);

}

void DebugMode::DrawHyperPlanes(const Vector v, const float min, const float max) const
{
	Vector barAxisPerp = { -v.y, v.x };

	Point Pmax = max * v;
	Point P1 = Pmax + 100 * barAxisPerp;
	Point P2 = Pmax - 100 * barAxisPerp;

	// first plane
	SDL_RenderDrawLine(m_renderer, P1.x * DRAW_SCALE + SHIFTX,
		P1.y * DRAW_SCALE + SHIFTY,
		P2.x * DRAW_SCALE + SHIFTX,
		P2.y * DRAW_SCALE + SHIFTY);

	Point Pmin = min * v;
	P1 = Pmin + 100 * barAxisPerp;
	P2 = Pmin - 100 * barAxisPerp;

	// first plane
	SDL_RenderDrawLine(m_renderer, P1.x * DRAW_SCALE + SHIFTX,
		P1.y * DRAW_SCALE + SHIFTY,
		P2.x * DRAW_SCALE + SHIFTX,
		P2.y * DRAW_SCALE + SHIFTY);



}

void DebugMode::PrintCommand() const
{
	ClearTerminal();

	std::cout << "COMMAND \n";
	std::cout << "ENTER : Generate new polygon \n";
	std::cout << "BACKSPACE : Back to main menu \n";
	std::cout << "+ : Increase the number of vertex (Need to be re-generated) \n";
	std::cout << "- : Decrease the number of vertex (the minimum is 3 vertices) \n";
	std::cout << "m : toggle min polygon view \n";
	std::cout << "d : toggle min pol debug helper (min polygon view need to be on) \n";

}

void DebugMode::ProcessEvents()
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
				MakePolygons();
				break;
			case SDLK_BACKSPACE:
				m_isRunning = false;
				break;
			case SDLK_PLUS:
				++nVertices;
				break;
			case SDLK_MINUS:
				--nVertices;
				nVertices = std::max(3, nVertices);
				break;
			
			case SDLK_SEMICOLON: // For my own keyboard layout
				if ((SDL_GetModState() & KMOD_SHIFT)) {
					++nVertices;
				}
				break;
			case SDLK_m:
				m_isShowMinPol = !m_isShowMinPol;
				break;
			case SDLK_d:
				m_isShowMinPolDebug = !m_isShowMinPolDebug;
			default:
				break;
			}
		}
	}
}

void DebugMode::MakePolygons()
{
	PrintCommand();

	pol1.vertices.clear();
	pol2.vertices.clear();
	pol1Red.vertices.clear();
	pol2Red.vertices.clear();

	pol1 = MakeConvexPol(nVertices);
	pol2 = MakeConvexPol(nVertices);

	m_isPolIntersect = DoPolygonsIntersects(pol1, pol2);


	Vector barAxis = pol2.baryCenter - pol1.baryCenter;

	auto AProj = GetMinMaxPolygonProjAxis(pol1, barAxis);
	auto BProj = GetMinMaxPolygonProjAxis(pol2, barAxis);

	// check if this axis is not a separating axis
	if (AProj.first > BProj.second || AProj.second < BProj.first) {
		return;
	}

	// compute reduced polygon
	pol1Red = PolygonComputeReducePol(pol1, barAxis, BProj.first, true);
	pol2Red = PolygonComputeReducePol(pol2, barAxis, AProj.second, false);


	
}
