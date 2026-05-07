#pragma once

#include <bits/stdc++.h>
#include "PathFinder.hpp"
#include "Parser.hpp"


using namespace std;
using cord = pair<int,int>;

enum HeuristicType {
    HEUR_MANHATTAN,
    HEUR_EUCLIDEAN,
    HEUR_CHEBYSHEV
};


class A_Star_Solver:public PathFinder{
private:
    HeuristicType heuristic;
public:
    map<State, double> gScore;
    void expandNode(Node current);
    IterablePriorityQueue<Node, vector<Node>, CompareF> openSet;
    set<State> closedSet;
    map<State, State> parentMap;

    void step()override;
    bool isFinished()override;
    vector<cord> getOpenSetCoords() const  override;
    vector<cord> getClosedSetCoords() const override;
    vector<cord> getFinalPathCoords() const override;
    A_Star_Solver(MapData* m,HeuristicType heuType) ;
    ~A_Star_Solver() override=default;
    double calculate_h(cord current, int currentSeq);
    double calculate_g(double parent_g, cord current, cord neighbor);
    
};



