#pragma once
#include <bits/stdc++.h>
#include <raylib.h>
#include <raymath.h>
using namespace std;
using cord = pair<int, int>;

class PathFinder {
public:
    virtual void step() = 0;
    virtual bool isFinished() = 0;
    virtual std::vector<cord> getOpenSetCoords() const = 0;
    virtual std::vector<cord> getClosedSetCoords() const = 0;
    virtual std::vector<cord> getFinalPathCoords() const = 0;
    virtual ~PathFinder() = default;
};