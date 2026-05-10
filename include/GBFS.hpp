#pragma once

#include <bits/stdc++.h>
#include "PathFinder.hpp"
#include "A_Star.hpp"

class GBFS_Solver : public PathFinder {
private:
    HeuristicType heuristic;
public:
    IterablePriorityQueue<Node, vector<Node>, CompareF> openSet;
    set<State> closedSet;
    map<State, State> parentMap;
    map<State, double> actualG; 

    GBFS_Solver(MapData* m, HeuristicType hType);
    void step() override;
    bool isFinished() override;
    void expandNode(Node current);
    double calculate_h(cord current, int currentSeq);
    
    vector<cord> getOpenSetCoords() const override;
    vector<cord> getClosedSetCoords() const override;
    vector<cord> getFinalPathCoords() const override;
};

