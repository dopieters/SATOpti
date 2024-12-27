#include "DebugMode.h"
#include <cassert>

#define W_WIDTH 680
#define W_HEIGHT 420
#define SCALE 50



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



void DebugMode::DrawPolygons()
{

	if (!m_isPolIntersect)
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 255);
	else
		SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);

	const float shiftX = W_WIDTH / 2;
	const float shiftY = W_HEIGHT / 2;
	for (int ii = 0; ii < pol1.vertices.size() - 1; ++ii) {
		SDL_RenderDrawLine(m_renderer, pol1.vertices[ii].x * SCALE + shiftX,
			pol1.vertices[ii].y * SCALE + shiftY,
			pol1.vertices[ii + 1].x * SCALE + shiftX,
			pol1.vertices[ii + 1].y * SCALE + shiftY);
	}
	SDL_RenderDrawLine(m_renderer, pol1.vertices[pol1.vertices.size() - 1].x * SCALE + shiftX,
		pol1.vertices[pol1.vertices.size() - 1].y * SCALE + shiftY,
		pol1.vertices[0].x * SCALE + shiftX,
		pol1.vertices[0].y * SCALE + shiftY);

	if (!m_isPolIntersect)
		SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);


	for (int ii = 0; ii < pol2.vertices.size() - 1; ++ii) {
		SDL_RenderDrawLine(m_renderer, pol2.vertices[ii].x * SCALE + shiftX,
			pol2.vertices[ii].y * SCALE + shiftY,
			pol2.vertices[ii + 1].x * SCALE + shiftX,
			pol2.vertices[ii + 1].y * SCALE + shiftY);
	}
	SDL_RenderDrawLine(m_renderer, pol2.vertices[pol2.vertices.size() - 1].x * SCALE + shiftX,
		pol2.vertices[pol2.vertices.size() - 1].y * SCALE + shiftY,
		pol2.vertices[0].x * SCALE + shiftX,
		pol2.vertices[0].y * SCALE + shiftY);

	
	
	

	if (m_isShowMinPol)
	{
		SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
		if (pol1Red.vertices.size() > 0)
		{
			for (int ii = 0; ii < pol1Red.vertices.size() - 1; ++ii) {
				SDL_RenderDrawLine(m_renderer, pol1Red.vertices[ii].x * SCALE + shiftX,
					pol1Red.vertices[ii].y * SCALE + shiftY,
					pol1Red.vertices[ii + 1].x * SCALE + shiftX,
					pol1Red.vertices[ii + 1].y * SCALE + shiftY);
			}
			SDL_RenderDrawLine(m_renderer, pol1Red.vertices[pol1Red.vertices.size() - 1].x * SCALE + shiftX,
				pol1Red.vertices[pol1Red.vertices.size() - 1].y * SCALE + shiftY,
				pol1Red.vertices[0].x * SCALE + shiftX,
				pol1Red.vertices[0].y * SCALE + shiftY);
		}

		if (pol2Red.vertices.size() > 0)
		{

			for (int ii = 0; ii < pol2Red.vertices.size() - 1; ++ii) {
				SDL_RenderDrawLine(m_renderer, pol2Red.vertices[ii].x * SCALE + shiftX,
					pol2Red.vertices[ii].y * SCALE + shiftY,
					pol2Red.vertices[ii + 1].x * SCALE + shiftX,
					pol2Red.vertices[ii + 1].y * SCALE + shiftY);
			}
			SDL_RenderDrawLine(m_renderer, pol2Red.vertices[pol2Red.vertices.size() - 1].x * SCALE + shiftX,
				pol2Red.vertices[pol2Red.vertices.size() - 1].y * SCALE + shiftY,
				pol2Red.vertices[0].x * SCALE + shiftX,
				pol2Red.vertices[0].y * SCALE + shiftY);
		}

		if (m_isShowMinPolDebug) {
			SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255);
			SDL_RenderDrawLine(m_renderer, pol1.baryCenter.x * SCALE + shiftX,
				pol1.baryCenter.y * SCALE + shiftY,
				pol2.baryCenter.x * SCALE + shiftX,
				pol2.baryCenter.y * SCALE + shiftY);


			SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 255);
			Vector barAxis = pol2.baryCenter - pol1.baryCenter;
			barAxis = barAxis / barAxis.Mag();

			auto pol1Proj = GetMinMaxPolygonProjAxis(pol1, barAxis);
			auto pol2Proj = GetMinMaxPolygonProjAxis(pol2, barAxis);

			Vector barAxisPerp = { -barAxis.y, barAxis.x };

			Point Amax = pol1Proj.second * barAxis;
			Point A1 = Amax + 100 * barAxisPerp;
			Point A2 = Amax - 100 * barAxisPerp;

			// first plane
			SDL_RenderDrawLine(m_renderer, A1.x * SCALE + shiftX,
				A1.y * SCALE + shiftY,
				A2.x * SCALE + shiftX,
				A2.y * SCALE + shiftY);

			Point Amin = pol1Proj.first * barAxis;
			A1 = Amin + 100 * barAxisPerp;
			A2 = Amin - 100 * barAxisPerp;

			// first plane
			SDL_RenderDrawLine(m_renderer, A1.x * SCALE + shiftX,
				A1.y * SCALE + shiftY,
				A2.x * SCALE + shiftX,
				A2.y * SCALE + shiftY);

			SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);

			Point Bmin = pol2Proj.first * barAxis;
			Point B1 = Bmin + 100 * barAxisPerp;
			Point B2 = Bmin - 100 * barAxisPerp;

			// second plane
			SDL_RenderDrawLine(m_renderer, B1.x * SCALE + shiftX,
				B1.y * SCALE + shiftY,
				B2.x * SCALE + shiftX,
				B2.y * SCALE + shiftY);

			Point Bmax = pol2Proj.second * barAxis;
			B1 = Bmax + 100 * barAxisPerp;
			B2 = Bmax - 100 * barAxisPerp;

			SDL_RenderDrawLine(m_renderer, B1.x * SCALE + shiftX,
				B1.y * SCALE + shiftY,
				B2.x * SCALE + shiftX,
				B2.y * SCALE + shiftY);
		}

	}


	

}

void DebugMode::PrintCommand()
{
	// clear terminal
	std::cout << "\x1B[2J\x1B[H"; // ANSI escape code

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
