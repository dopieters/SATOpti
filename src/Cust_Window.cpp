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

	MakePolygons();
	 
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
	m_isPolIntersect = false;
	if (!m_isPolIntersect)
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 255);
	else
		SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);

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

	if (!m_isPolIntersect)
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

	Point Amax = pol1Proj.second * barAxis;// +pol1.baryCenter;
	Point A1 = Amax + 100 * barAxisPerp;
	Point A2 = Amax - 100 * barAxisPerp;

	// first plane
	SDL_RenderDrawLine(m_renderer, A1.x * SCALE + shiftX,
		A1.y * SCALE + shiftY,
		A2.x * SCALE + shiftX,
		A2.y * SCALE + shiftY);

	Point Amin = pol1Proj.first * barAxis;// +pol1.baryCenter;
	A1 = Amin + 100 * barAxisPerp;
	A2 = Amin - 100 * barAxisPerp;

	// first plane
	SDL_RenderDrawLine(m_renderer, A1.x * SCALE + shiftX,
		A1.y * SCALE + shiftY,
		A2.x * SCALE + shiftX,
		A2.y * SCALE + shiftY);

	SDL_SetRenderDrawColor(m_renderer, 0, 255,0, 255);

	Point Bmin = pol2Proj.first * barAxis;
	Point B1 = Bmin + 100 * barAxisPerp;
	Point B2 = Bmin - 100 * barAxisPerp;

	// first plane
	SDL_RenderDrawLine(m_renderer, B1.x * SCALE + shiftX,
		B1.y * SCALE + shiftY,
		B2.x * SCALE + shiftX,
		B2.y * SCALE + shiftY);

	Point Bmax = pol2Proj.second * barAxis;
	B1 = Bmax+ 100 * barAxisPerp;
	B2 = Bmax - 100 * barAxisPerp;

	SDL_RenderDrawLine(m_renderer, B1.x * SCALE + shiftX,
		B1.y * SCALE + shiftY,
		B2.x * SCALE + shiftX,
		B2.y * SCALE + shiftY);
	


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
				MakePolygons();
			}
		}
	}
}

void Cust_Window::MakePolygons()
{
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
