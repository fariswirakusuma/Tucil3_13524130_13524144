#pragma once
#include <bits/stdc++.h>
#include "portable-file-dialogs.h"
#include <raylib.h>
using namespace std;
using cord = pair<int, int>;

struct MapData {
    int rows, cols;
    vector<string> grid;
    vector<vector<int>> costMap;
    cord start;
    cord target;
    map<int, cord> waypoints; 
};
MapData parseMapInput(std::istream& input);
void parsing();