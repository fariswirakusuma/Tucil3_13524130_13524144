#include <A_Star.hpp>

double A_Star_Solver::calculate_h(cord current, cord end){
    double dx = abs(current.first - end.first);
    double dy = abs(current.second - end.second);
    
    switch (heuristic) {
        case HEUR_MANHATTAN:
            return dx + dy;

        case HEUR_EUCLIDEAN:
            return sqrt(dx * dx + dy * dy);

        case HEUR_CHEBYSHEV:
            return (dx > dy) ? dx : dy;

        default:
            return 0;
    }
    return abs(current.first - end.first) + abs(current.second - end.second);
}
double A_Star_Solver::calculate_g(double parent_g, cord current, cord neighbor){
    return parent_g + sqrt(pow(current.first - neighbor.first, 2) + pow(current.second - neighbor.second, 2));
}

void A_Star_Solver::expandNode(Node current) {
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++) {
        int r = current.c.first;
        int c = current.c.second;
        double moveCost = 0;
        bool validStop = false;

        while (true) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (nr < 0 || nr >= p_mapData->rows || nc < 0 || nc >= p_mapData->cols) break;

            if (p_mapData->grid[nr][nc] == 'X') {
                validStop = true;
                break;
            }

            r = nr;
            c = nc;
            moveCost += p_mapData->costMap[r][c];
            if (p_mapData->grid[r][c] == 'L') {
                validStop = false; 
                break;
            }
        }

        if (validStop) {
            cord nextPos = {r, c};
            currentExploringEdges.push_back({current.c, nextPos});

            if (nextPos != current.c && closedSet.find(nextPos) == closedSet.end()) {
                double h_val = calculate_h(nextPos, end);
                openSet.push(Node(nextPos, current.g + moveCost, h_val));
                parentMap[nextPos] = current.c;
            }
        }
    }
}



void A_Star_Solver::step() {
    currentExploringEdges.clear(); 
    if (!openSet.empty() && !found) {
        Node current = openSet.top();
        openSet.pop();

        if (current.c == end) {
            found = true;
            totalCost = current.g; 
            return;
        }
        
        closedSet.insert(current.c);
        expandNode(current);
    }
}

bool A_Star_Solver::isFinished() {
    return found || openSet.empty(); 
}
vector<cord> A_Star_Solver::getOpenSetCoords() const {
    vector<cord> coords;
    const auto& container = openSet.get_container(); 
    for (const auto& node : container) {
        coords.push_back(node.c);
    }
    return coords;
}

vector<cord> A_Star_Solver::getClosedSetCoords() const {
    return vector<cord>(closedSet.begin(), closedSet.end());
}

vector<cord> A_Star_Solver::getFinalPathCoords() const {
    vector<cord> path;
    if (!found) return path; 

    cord curr = end;
    while (parentMap.find(curr) != parentMap.end()) {
        path.push_back(curr);
        curr = parentMap.at(curr);
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

A_Star_Solver::A_Star_Solver(MapData* m,HeuristicType heuType) :heuristic(heuType), p_mapData(m){
    this->start = m->start;
    this->end = m->target;
    this->found = false;

    double h_val = calculate_h(start, end);
    openSet.push(Node(start, 0, h_val));
}