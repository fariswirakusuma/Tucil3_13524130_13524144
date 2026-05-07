#include <A_Star.hpp>

double A_Star_Solver::calculate_h(cord current, int currentSeq) {
    cord currentTarget;
    auto it = p_mapData->waypoints.find(currentSeq);
    
    if (it != p_mapData->waypoints.end()) {
        currentTarget = it->second; 
    } else {
        currentTarget = p_mapData->target;
    }

    double dx = (double)abs(current.first - currentTarget.first);
    double dy = (double)abs(current.second - currentTarget.second);
    
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
}
double A_Star_Solver::calculate_g(double parent_g, cord current, cord neighbor) {
    double stepCost = 0;
    int dr = (neighbor.first > current.first) - (neighbor.first < current.first);
    int dc = (neighbor.second > current.second) - (neighbor.second < current.second);

    int r = current.first;
    int c = current.second;

    while (r != neighbor.first || c != neighbor.second) {
        r += dr;
        c += dc;
        stepCost += p_mapData->costMap[r][c];
    }

    return parent_g + stepCost;
}   

void A_Star_Solver::expandNode(Node current) {
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++) {
        int r = current.c.first;
        int c = current.c.second;
        int seq = current.seq;
        bool validStop = false;

        while (true) {
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (nr < 0 || nr >= p_mapData->rows || nc < 0 || nc >= p_mapData->cols) {
                validStop = true;
                break;
            }

            if (p_mapData->grid[nr][nc] == 'X') {
                validStop = true;
                break;
            }

            r = nr; c = nc;
            char cell = p_mapData->grid[r][c];

            if (cell == 'L') {
                validStop = false;
                break;
            }

            if (isdigit(cell)) {
                int val = cell - '0';
                if (val == seq) seq++;
            }
        }

        if (validStop && (r != current.c.first || c != current.c.second)) {
            cord nextPos = {r, c};
            State nextState = {r, c, seq};

            currentExploringEdges.push_back({current.c, nextPos});

            double new_g = calculate_g(current.g, current.c, nextPos);
            if (gScore.find(nextState) == gScore.end() || new_g < gScore[nextState]) {
                gScore[nextState] = new_g;
                parentMap[nextState] = {current.c.first, current.c.second, current.seq};

                double h_val = calculate_h(nextPos, seq);
                openSet.push(Node(nextPos, seq, new_g, h_val, new_g + h_val));
            }
        }
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
    set<cord> uniqueCoords;

    for (const auto& s : closedSet) {
        uniqueCoords.insert({s.r, s.c});
    }

    return vector<cord>(uniqueCoords.begin(), uniqueCoords.end());
}

vector<cord> A_Star_Solver::getFinalPathCoords() const {
    vector<cord> path;
    if (!found) return path;

    State current = this->lastState;
    int safety = 0;
    int maxLimit = p_mapData->rows * p_mapData->cols * ((int)p_mapData->waypoints.size() + 1);

    while (safety < maxLimit) {
        path.push_back({current.r, current.c});

        if (current.r == start.first && current.c == start.second && current.seq == 0) break;

        auto it = parentMap.find(current);
        if (it == parentMap.end()) break;
        
        current = it->second;
        safety++;
    }

    std::reverse(path.begin(), path.end());
    return path;
}
void A_Star_Solver::step() {
    currentExploringEdges.clear();
    if (!openSet.empty() && !found) {
        Node current = openSet.top();
        openSet.pop();

        State currentState = {current.c.first, current.c.second, current.seq};

        auto itBest = gScore.find(currentState);
        if (itBest != gScore.end() && current.g > itBest->second) {
            return;
        }
        if (closedSet.find(currentState) != closedSet.end()) return;

        if (current.c.first == p_mapData->target.first && 
            current.c.second == p_mapData->target.second && 
            current.seq == (int)p_mapData->waypoints.size()) {

            this->lastState = currentState;
            this->found = true;
            this->totalCost = current.g; 
            return;
        }

        closedSet.insert(currentState);
        expandNode(current);
    }
}

A_Star_Solver::A_Star_Solver(MapData* m, HeuristicType heuType) 
    :PathFinder(m), heuristic(heuType){
    this->start = m->start;
    this->end = m->target;
    this->found = false;

    int initialSeq = 0;
    double h_val = calculate_h(start, initialSeq);
    
    State startState = {start.first, start.second, initialSeq};
    gScore[startState] = 0.0;

    openSet.push(Node(start, initialSeq, 0.0, h_val, h_val));
}