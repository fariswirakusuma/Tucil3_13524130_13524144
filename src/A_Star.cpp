#include <A_Star.hpp>

double calculate_h(cord current, cord end){
    return abs(current.first - end.first) + abs(current.second - end.second);
}
double calculate_g(double parent_g, cord current, cord neighbor){
    return parent_g + sqrt(pow(current.first - neighbor.first, 2) + pow(current.second - neighbor.second, 2));
}



void A_Star_Solver::expandNode(Node current){
    vector<Node> neighbors;
    for (auto neighbor : neighbors){
        double g = calculate_g(current.g, current.c, neighbor.c);
        double h = calculate_h(neighbor.c, end);
        neighbor.g = g;
        neighbor.h = h;
        neighbor.f = g + h;
        openSet.push(neighbor);
    }
}

void A_Star_Solver::step(){
    if(!openSet.empty()){
        Node current = openSet.top();
        if(current.c == end){
            return;
        }
        openSet.pop();
        expandNode(current);
    }
}

bool A_Star_Solver::isFinished(){
    return openSet.empty();

}