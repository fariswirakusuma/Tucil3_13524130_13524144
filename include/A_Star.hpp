#pragma once

#include <bits/stdc++.h>
#include "PathFinder.hpp"
#include "Parser.hpp"


using namespace std;
using cord = pair<int,int>;

class Node{
public:
    cord c;
    double g,h,f;
    Node(cord coord, double g_val, double h_val) 
        : c(coord), g(g_val), h(h_val), f(g_val + h_val) {}
    Node() : c({0,0}), g(0), h(0), f(0) {}

};
struct CompareF {
    bool operator()(const Node& a, const Node& b) {
        return a.f > b.f; 
    }
};
template <class T, class Container = std::vector<T>, class Compare = std::less<typename Container::value_type>>
struct IterablePriorityQueue : std::priority_queue<T, Container, Compare> {
    const Container& get_container() const { return this->c; }
};
enum HeuristicType {
    HEUR_MANHATTAN,
    HEUR_EUCLIDEAN,
    HEUR_CHEBYSHEV
};
class A_Star_Solver:public PathFinder{
private:
    HeuristicType heuristic;
public:
    const MapData* p_mapData;
    bool found;
    cord start;
    cord end;
    void expandNode(Node current);
    IterablePriorityQueue<Node, vector<Node>, CompareF> openSet;
    set<cord> closedSet;
    map<cord, cord> parentMap;

    void step()override;
    bool isFinished()override;
    vector<cord> getOpenSetCoords() const  override;
    vector<cord> getClosedSetCoords() const override;
    vector<cord> getFinalPathCoords() const override;
    A_Star_Solver(MapData* m,HeuristicType heuType) ;
    ~A_Star_Solver() override=default;
    double calculate_h(cord current, cord end);
    double calculate_g(double parent_g, cord current, cord neighbor);
    
};


