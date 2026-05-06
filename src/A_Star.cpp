#include <A_Star.hpp>

double calculate_h(cord current, cord end){
    return abs(current.first - end.first) + abs(current.second - end.second);
}
double calculate_g(double parent_g, cord current, cord neighbor){
    return parent_g + sqrt(pow(current.first - neighbor.first, 2) + pow(current.second - neighbor.second, 2));
}

priority_queue<Node, vector<Node>, CompareF> A_Star_Solver::openSet(){

}

void A_Star_Solver::expandNode(Node current){
    vector<Node> neighbors;
    
}
