#pragma once

#include <bits/stdc++.h>
#include "PathFinder.hpp"
#include "Parser.hpp"


using namespace std;
using cord = pair<int,int>;

class Node{
public:
    cord c;
    int seq;
    double g,h,f;
    
    Node(cord coord, int sequence, double g_val, double h_val, double f_val) 
        : c(coord), 
          seq(sequence), 
          g(g_val), 
          h(h_val), 
          f(f_val) 
    {}

    Node() : c({0,0}), seq(0), g(0), h(0), f(0) {}

    bool operator>(const Node& other) const {
        return f > other.f;
    }
};
struct CompareF {
    bool operator()(const Node& a, const Node& b) {
        return a.f > b.f; 
    }
};
struct State {
    int r, c, seq;
    bool operator<(const State& other) const {
        if (r != other.r) return r < other.r;
        if (c != other.c) return c < other.c;
        return seq < other.seq;
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
    State lastState;
public:
    const MapData* p_mapData;
    bool found;
    cord start;
    cord end;
    int seq;
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



