#pragma once
#include <bits/stdc++.h>
#include <raylib.h>
#include <raymath.h>
using namespace std;
using cord = pair<int, int>;

class PathFinder {
protected:
    double totalCost = 0.0; 
    bool found = false;
    vector<std::pair<cord, cord>> currentExploringEdges;
public:
    virtual void step() = 0;
    virtual bool isFinished() = 0;
    virtual vector<cord> getOpenSetCoords() const = 0;
    virtual vector<cord> getClosedSetCoords() const = 0;
    virtual vector<cord> getFinalPathCoords() const = 0;
    virtual ~PathFinder() = default;
    virtual double getTotalCost() const { return totalCost; }
    virtual const std::vector<std::pair<cord, cord>>& getExploringEdges() const {
        return currentExploringEdges;
    }

};