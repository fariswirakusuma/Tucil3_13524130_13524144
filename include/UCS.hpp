#pragma once
#include "PathFinder.hpp"
#include "Parser.hpp"
#include <map>
#include <set>
#include <vector>

class UCS_Solver : public PathFinder {
private:
    std::map<State, double> gScore;
    std::map<State, State> parentMap;
    std::set<State> closedSet;
    IterablePriorityQueue<Node, std::vector<Node>, CompareF> openSet;

    double calculate_g(double parent_g, cord current, cord neighbor);
    void expandNode(Node current);

public:
    UCS_Solver(MapData* m);

    void step() override;
    bool isFinished() override;
    std::vector<cord> getOpenSetCoords() const override;
    std::vector<cord> getClosedSetCoords() const override;
    std::vector<cord> getFinalPathCoords() const override;
    ~UCS_Solver() override = default;

};