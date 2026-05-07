#pragma once
#include <bits/stdc++.h>
#include <raylib.h>
#include <raymath.h>
#include "Parser.hpp"
using namespace std;
using cord = pair<int, int>;

template <class T, class Container = std::vector<T>, class Compare = std::less<typename Container::value_type>>
struct IterablePriorityQueue : std::priority_queue<T, Container, Compare> {
    const Container& get_container() const { return this->c; }
};
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


class PathFinder {
protected:
    const MapData* p_mapData;
    cord start;
    cord end;
    double totalCost = 0.0; 
    bool found = false;
    vector<std::pair<cord, cord>> currentExploringEdges;
    State lastState;
public:
    PathFinder(const MapData* m) : p_mapData(m) {
        if (m != nullptr) {
            this->start = m->start;
            this->end = m->target;
        }
    }
    virtual ~PathFinder() = default;

    virtual void step() = 0;
    virtual bool isFinished() = 0;
    virtual vector<cord> getOpenSetCoords() const = 0;
    virtual vector<cord> getClosedSetCoords() const = 0;
    virtual vector<cord> getFinalPathCoords() const = 0;
    virtual double getTotalCost() const { return totalCost; }
    virtual const std::vector<std::pair<cord, cord>>& getExploringEdges() const {
        return currentExploringEdges;
    }

};