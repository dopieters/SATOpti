# SATOpti

## Introduction

While reading about collision detection between two convex polygons, I came across the SAT (Separating Axis Theorem) intersection test idea.

The basic idea behind this algorithm is simple: if there exists an axis along which the projections of both polygons do not overlap, then they do not intersect. To visualize this, one can project the vertices of both polygons onto lines that are perpendicular to the edges of the polygons. If, for all such axes, the projections do not overlap, the polygons do not intersect. If there is any axis where the projections do overlap, then the polygons are intersecting.

Many resources I found online suggest testing against all the edges, but this is not always necessary. Depending on the position and shape of the polygons, the test can be done against a reduced set of axes, which simplifies the calculation.
For example, in the image below, only the normals of the white edges need to be tested, while the normals of the red edges can be ignored.

![Screenshot](images/PolIntersection.png)

This project has the goal of comparing the traditional SAT test with the new one.

Note: The above description of SAT is valid for polygons in 2D. For polygons in 3D or polyhedra, the test is slightly different and involves additional considerations


## TODO list
  
1. A debug mode, where one can write out and load polygon data, needs to be added. 
2. A debug mode, to scan for coherence between the different method (that save bugged events)
3. Other convex polygon intersection tests, such as GJK or Chung–Wang, could be added



## Current results 

> Brute force with 500 vertices:
> - Average time with inter : 0.280373ms
> - Average time with no inter : 0.96294ms
> SAT with 500 vertices:
> - Average time with inter : 0.786621ms
> - Average time with no inter : 0.0314419ms
> SAT opti with 500 vertices:
> - Average time with inter : 0.0783183ms
> - Average time with no inter : 0.000664432ms

After the final optimization of both SAT and the new algorithm based on SAT, the time cost is quite similar. 
The only difference is due to the test without intersection. This difference is mainly due to two early exit tests that could easily be added to the original method.

Note: The new method does not provide any minimal vector translation to resolve collisions.


