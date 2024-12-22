#include "Cust_Window.h"

#define W_WIDTH 680
#define W_HEIGHT 420
#define SCALE 75



Cust_Window::Cust_Window():
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

	
	m_isValid = true;
}

Cust_Window::~Cust_Window()
{
	if (!m_isValid) return;


	// Cleanup
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void Cust_Window::Run()
{
	m_isRunning = true;

	
	pol1 = MakeConvexPol(nVertices);
	pol2 = MakeConvexPol(nVertices);

	while (m_isRunning) {
		SDL_RenderClear(m_renderer);
		ProcessEvents();

		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		SDL_RenderClear(m_renderer);

		DrawPolygons();
		//SDL_RenderDrawLines // for drawing polygon

		// Present the backbuffer
		SDL_RenderPresent(m_renderer);

	}
}

void Cust_Window::DrawPolygons()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 255);

	const float shiftX = W_WIDTH / 2;
	const float shiftY = W_HEIGHT / 2;
	for (int ii = 0; ii < nVertices - 1; ++ii) {
		SDL_RenderDrawLine(m_renderer, pol1.vertices[ii].x * SCALE + shiftX,
			pol1.vertices[ii].y * SCALE + shiftY,
			pol1.vertices[ii + 1].x * SCALE + shiftX,
			pol1.vertices[ii + 1].y * SCALE + shiftY);
	}
	SDL_RenderDrawLine(m_renderer, pol1.vertices[nVertices - 1].x * SCALE + shiftX,
		pol1.vertices[nVertices - 1].y * SCALE + shiftY,
		pol1.vertices[0].x * SCALE + shiftX,
		pol1.vertices[0].y * SCALE + shiftY);


	SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);


	for (int ii = 0; ii < nVertices - 1; ++ii) {
		SDL_RenderDrawLine(m_renderer, pol2.vertices[ii].x * SCALE + shiftX,
			pol2.vertices[ii].y * SCALE + shiftY,
			pol2.vertices[ii + 1].x * SCALE + shiftX,
			pol2.vertices[ii + 1].y * SCALE + shiftY);
	}
	SDL_RenderDrawLine(m_renderer, pol2.vertices[nVertices - 1].x * SCALE + shiftX,
		pol2.vertices[nVertices - 1].y * SCALE + shiftY,
		pol2.vertices[0].x * SCALE + shiftX,
		pol2.vertices[0].y * SCALE + shiftY);
}

void Cust_Window::ProcessEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{

		if (event.type == SDL_QUIT)
		{
			m_isRunning = false;
		}
		else if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_RETURN) {
				pol1.vertices.clear();
				pol1 = MakeConvexPol(nVertices);
				pol2.vertices.clear();
				pol2 = MakeConvexPol(nVertices);
			}
		}
	}
}
