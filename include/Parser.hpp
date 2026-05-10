#pragma once
#include <bits/stdc++.h>
#include <raylib.h>
#include "portable-file-dialogs.h"
using namespace std;
using cord = pair<int, int>;

struct MapData {
    int rows, cols;
    vector<string> grid;
    vector<vector<int>> costMap;
    cord start;
    cord target;
    map<int, cord> waypoints; 
    int maxSequence;

};
MapData parseMapInput(std::istream& input);
void parsing(MapData& activeMap, bool& isMapLoaded);
void parsing(MapData& activeMap, bool& isMapLoaded, const std::string& filePath);

void saveSolution(const string& algo, const string& heuristic, int cost, int iterations, long double time, const string& moves);
