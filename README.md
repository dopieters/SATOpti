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
The current results, with 4835 intersections out of 10000 events, are:

> Brute force with 100 vertices:
> - Average time with intersection: 0.0116636ms
> - Average time with no intersection: 0.0345464ms
>
> SAT with 100 vertices:
> - Average time with intersection: 0.10532ms
> - Average time with no intersection: 0.00556886ms
>
> SAT Opti with 100 vertices:
> - Average time with intersection: 0.00999478ms
> - Average time with no intersection: 0.000595533ms

### Brute Force vs SAT Opti:
The SAT Opti method gives similar results to the brute force method when the polygons are intersecting. However, the SAT Opti method is significantly faster when there is no intersection. This is mainly due to the principle behind both methods. The brute force method will exit early when an intersection is found, while the SAT Opti method will exit early when a separating axis is found.

Both methods have their strengths, and one should choose the one appropriate to their usage.

### SAT vs SAT Opti:
The SAT Opti method is always faster than the SAT method. However, there are two parameters to take into account. The SAT Opti integrates two early exit checks that influence its results.

SAT Opti checks one separating axis early, which makes the code significantly faster if there is no intersection. If both methods include this test, the results are much closer to each other:

> SAT with 100 vertices:
> - Average time with no intersection: 0.000612266ms
> SAT Opti with 100 vertices:
> - Average time with no intersection: 0.000415229ms

SAT Opti checks if the barycenters are included in the other polygon, which helps confirm if there is an intersection faster. If we remove this test, the speed advantage decreases from 10 times faster to about 4-5 times faster:

> SAT with 100 vertices:
> - Average time with intersection: 0.108657ms
> SAT Opti with 100 vertices:
> - Average time with intersection: 0.0229175ms


Note: The new method does not provide any minimal vector translation to resolve collisions.


