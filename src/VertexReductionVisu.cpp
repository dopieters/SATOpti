#include "VertexReductionVisu.h"
#include "matplot/matplot.h"
#include "Geom.h"



void VertexReductionVisu::Run() {


	//std::vector<int> VertexCountBeforeRed, VertexCountAfterRed;

	//
	//// track polygon reduction
	//{
	//	for (int nVert = 5; nVert < 100; nVert += 5)
	//	{
	//		for (int i = 0; i < 5000; i++)
	//		{
	//			auto A = Geom::MakeConvexPol(nVert); auto B = Geom::MakeConvexPol(nVert);
	//			while (true)
	//			{
	//				// check if barycenter inside each other
	//				if (IsPointInsidePolygon(A.baryCenter, B) || IsPointInsidePolygon(B.baryCenter, A)) {
	//					break;
	//				}

	//				Geom::Vector barAxis = B.baryCenter - A.baryCenter;
	//				//barAxis = barAxis / barAxis.Mag();
	//				auto aMax = GetMaxPolygonProjAxis(A, barAxis);
	//				auto bMin = -GetMaxPolygonProjAxis(B, -barAxis);

	//				// check if this axis is not a separating axis
	//				if (aMax < bMin) {
	//					break;
	//				}

	//				// compute reduced polygon
	//				Geom::Polygon C = PolygonComputeReducePol(A, barAxis, bMin, true);
	//				Geom::Polygon D = PolygonComputeReducePol(B, barAxis, aMax, false);


	//				VertexCountBeforeRed.push_back(A.vertices.size()); VertexCountBeforeRed.push_back(B.vertices.size());
	//				VertexCountAfterRed.push_back(C.vertices.size()); VertexCountAfterRed.push_back(D.vertices.size());

	//				if (C.vertices.size() >= A.vertices.size() || D.vertices.size() >= B.vertices.size()) {
	//					break;
	//				}

	//				A = C; B = D;
	//				break;

	//			}
	//		}
	//	}
	//}


	//// draw 2D histogram
	//// Create a 2D histogram
	//auto f = matplot::figure(true);

	//f->width(f->width() * 2);
	//f->height(f->height() * 2);
	//f->x_position(200);
	//f->y_position(100);

	//if (VertexCountAfterRed.size() == 0 || VertexCountBeforeRed.size() == 0) return;

	//matplot::binscatter(VertexCountAfterRed, VertexCountBeforeRed, 20, 20, matplot::bin_scatter_style::point_colormap);
	//matplot::axis(matplot::tight);
	//matplot::show();

	std::vector<int> VertexCountAfterRed;

	
	// track polygon reduction
	{
		const int nVert = 10000;
		{
			for (int i = 0; i < 10000; i++)
			{
				auto A = Geom::MakeConvexPol(nVert); auto B = Geom::MakeConvexPol(nVert);
				while (true)
				{
					// check if barycenter inside each other
					if (IsPointInsidePolygon(A.baryCenter, B) || IsPointInsidePolygon(B.baryCenter, A)) {
						break;
					}

					Geom::Vector barAxis = B.baryCenter - A.baryCenter;
					//barAxis = barAxis / barAxis.Mag();
					auto aMax = GetMaxPolygonProjAxis(A, barAxis);
					auto bMin = -GetMaxPolygonProjAxis(B, -barAxis);

					// check if this axis is not a separating axis
					if (aMax < bMin) {
						break;
					}

					// compute reduced polygon
					Geom::Polygon C = PolygonComputeReducePol(A, barAxis, bMin, true);
					Geom::Polygon D = PolygonComputeReducePol(B, barAxis, aMax, false);


					VertexCountAfterRed.push_back(C.vertices.size()); VertexCountAfterRed.push_back(D.vertices.size());

					if (C.vertices.size() >= A.vertices.size() || D.vertices.size() >= B.vertices.size()) {
						break;
					}

					A = C; B = D;
					break;

				}
			}
		}
	}

	auto h = matplot::hist(VertexCountAfterRed);
	matplot::show();

}