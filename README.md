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
  
1. Scan for current bottleneck
2. Add different method
3. Add histogram to see the cost function of the number of vertices



## Current Results
The current results, with 4870 intersections out of 10000 events, are:

> Brute force with 100 vertices:
> - Average time with inter : 0.0107166ms
> - Average time with no inter : 0.0349345ms
>   
> SAT with 100 vertices:
> - Average time with inter : 0.0662554ms
> - Average time with no inter : 0.00381162ms
>   
> SAT Opti with 100 vertices:
> - Average time with inter : 0.00582407ms
> - Average time with no inter : 0.000538469ms

### Brute Force vs SAT Opti:
The SAT Opti method is around twice as fast as the brute force one when working with colliding polygons having each 100 vertices. In the case we are working with non-colliding one, the SAT method is way faster. That is because the brute force method is focusing on finding an intersection while the SAT algorithm just search for a separating axis.

Both methods have their strengths, and one should choose the one appropriate to their usage.

### SAT vs SAT Opti (not latest results):
The SAT Opti method is always faster than the SAT method. However, there are two parameters to take into account. The SAT Opti integrates two early exit checks that influence its results.

SAT Opti checks one separating axis early, which makes the code significantly faster if there is no intersection. If both methods include this test, the results are much closer to each other:

> SAT with 100 vertices:
> - Average time with no intersection: 0.000612266ms
>   
> SAT Opti with 100 vertices:
> - Average time with no intersection: 0.000415229ms

SAT Opti checks if the barycenters are included in the other polygon, which helps confirm if there is an intersection faster. If we remove this test, the speed advantage decreases from 10 times faster to about 4-5 times faster:

> SAT with 100 vertices:
> - Average time with intersection: 0.108657ms
>   
> SAT Opti with 100 vertices:
> - Average time with intersection: 0.0229175ms


Note: The new method does not provide any minimal vector translation to resolve collisions.


