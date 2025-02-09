#include "DrawWindow.h"
#include <cassert>
#include <iostream>

#include "Params.h"
#include "Utilities.h"


DrawWindow::DrawWindow() {
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

	SDL_RenderClear(m_renderer);
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

	// properly managed to allocate needed memory
	m_isValid = true;
}

DrawWindow::DrawWindow(std::vector<std::pair<Polygon, Polygon>> polygonsPair)
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


	m_EventToDraw = polygonsPair;

	// properly managed to allocate needed memory
	m_isValid = true;
}

DrawWindow::~DrawWindow()
{
	if (!m_isValid) return;


	// Cleanup
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();

	m_renderer = nullptr;
	m_window = nullptr;
}


void DrawWindow::ScanPairOfPolygons()
{
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

void DrawWindow::DrawPolygonPairs()
{
		// Set polygon 1 edges draw color
		SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);

		DrawPolygon(m_EventToDraw[currentPolPair].first);

		// set pol 2 draw color
		SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);

		DrawPolygon(m_EventToDraw[currentPolPair].second);
	
}

void DrawWindow::DrawPolygon(const Polygon& A) const {
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 255);
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

void DrawWindow::ProcessEvents()
{
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

