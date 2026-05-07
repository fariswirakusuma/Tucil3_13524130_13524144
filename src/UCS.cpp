#include <UCS.hpp>

#include <algorithm>
#include <cctype>

double UCS_Solver::calculate_g(double parent_g, cord current, cord neighbor) {
    double stepCost = 0.0;
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

void UCS_Solver::expandNode(Node current) {
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

                openSet.push(Node(nextPos, seq, new_g, 0.0, new_g));
            }
        }
    }
}

void UCS_Solver::step() {
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

bool UCS_Solver::isFinished() {
    return found || openSet.empty();
}

std::vector<cord> UCS_Solver::getOpenSetCoords() const {
    std::vector<cord> coords;
    const auto& container = openSet.get_container(); 
    for (const auto& node : container) {
        coords.push_back(node.c);
    }
    return coords;
}

std::vector<cord> UCS_Solver::getClosedSetCoords() const {
    std::set<cord> uniqueCoords;
    for (const auto& s : closedSet) {
        uniqueCoords.insert({s.r, s.c});
    }
    return std::vector<cord>(uniqueCoords.begin(), uniqueCoords.end());
}

std::vector<cord> UCS_Solver::getFinalPathCoords() const {
    std::vector<cord> path;
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

UCS_Solver::UCS_Solver(MapData* m) : PathFinder(m) {
    int initialSeq = 0;
    State startState = {start.first, start.second, initialSeq};
    gScore[startState] = 0.0;
    openSet.push(Node(start, initialSeq, 0.0, 0.0, 0.0));
}