#pragma once

#include <bits/stdc++.h>
using namespace std;

// template <typename T>
using cord = pair<int,int>;

class Node{
public:
    cord c;
    double g,h,f;
    Node(cord c, double g, double h):c(c),g(0),h(0){
        f = g+h;
    }

};
struct CompareF {
    bool operator()(const Node& a, const Node& b) {
        return a.f > b.f; 
    }
};

// template <typename T>
class A_Star_Solver:public PathFinder{
public:

    cord start;
    cord end;
    void expandNode(Node current);
    priority_queue<Node, vector<Node>, CompareF> openSet();

    void step();
    bool isFinished();
    vector<cord> getOpenSetCoords() const  override;
    vector<cord> getClosedSetCoords() const override;
    vector<cord> getFinalPathCoords() const override;
};

double calculate_h(cord current, cord end);
double calculate_g(double parent_g, cord current, cord neighbor);
