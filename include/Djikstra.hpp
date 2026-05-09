#pragma once

#include <bits/stdc++.h>
#include "PathFinder.hpp"
#include "Parser.hpp"

using namespace std;
using cord = pair<int,int>;

class Djikstra_Solver : public PathFinder {
private:
    map<State, double> gScore;
    IterablePriorityQueue<Node, vector<Node>, CompareF> openSet;
    set<State> closedSet;
    map<State, State> parentMap;

public:
    Djikstra_Solver(MapData* m);
    ~Djikstra_Solver() override = default;

    void step() override;
    bool isFinished() override;
    void expandNode(Node current);
    
    vector<cord> getOpenSetCoords() const override;
    vector<cord> getClosedSetCoords() const override;
    vector<cord> getFinalPathCoords() const override;
    
    double calculate_g(double parent_g, cord current, cord neighbor);
};