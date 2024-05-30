# High-Performance-Computing
This repository contains an implementation of the Traveling Salesman Problem (TSP) solver using the Branch and Bound method. The TSP is a classic algorithmic problem in the fields of computer science and operations research which focuses on optimization. The aim is to find the shortest possible route that visits each city exactly once and returns to the origin city.

Key Features:
- Efficient Branch and Bound implementation.
- Handles various sizes of TSP instances.
- Easy-to-understand code with comments and documentation.

The distance between cities are pre-loaded. 

Algorithm Explanation:

The Branch and Bound method systematically considers all possible routes and eliminates those that exceed the current best solution (bound). The steps are as follows:

- Branching: Split the problem into smaller subproblems.
- Bounding: Calculate a bound on the minimum cost of any solution within a subproblem.
- Pruning: Discard subproblems whose bounds are higher than the current best solution.
