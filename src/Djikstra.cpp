#include <Djikstra.hpp>

Djikstra_Solver::Djikstra_Solver(MapData* m) : PathFinder(m) {
    this->found = false;
    State startState = {start.first, start.second, 0};
    gScore[startState] = 0.0;
    openSet.push(Node(start, 0, 0.0, 0.0, 0.0));
}

double Djikstra_Solver::calculate_g(double parent_g, cord current, cord neighbor) {
    double stepCost = 0;
    int dr = (neighbor.first > current.first) - (neighbor.first < current.first);
    int dc = (neighbor.second > current.second) - (neighbor.second < current.second);
    int r = current.first;
    int c = current.second;

    while (r != neighbor.first || c != neighbor.second) {
        r += dr; c += dc;
        stepCost += p_mapData->costMap[r][c];
    }
    return parent_g + stepCost;
}

void Djikstra_Solver::expandNode(Node current) {
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++) {
        int r = current.c.first, c = current.c.second, seq = current.seq;
        bool validStop = false;

        while (true) {
            int nr = r + dr[i], nc = c + dc[i];
            if (nr < 0 || nr >= p_mapData->rows || nc < 0 || nc >= p_mapData->cols) {
                validStop = true; break;
            }
            if (p_mapData->grid[nr][nc] == 'X') {
                validStop = true; break;
            }
            r = nr; c = nc;
            if (p_mapData->grid[r][c] == 'L') { validStop = false; break; }
            if (isdigit(p_mapData->grid[r][c])) {
                if ((p_mapData->grid[r][c] - '0') == seq) seq++;
            }
        }

        if (validStop && (r != current.c.first || c != current.c.second)) {
            cord nextPos = {r, c};
            State nextState = {r, c, seq};
            double new_g = calculate_g(current.g, current.c, nextPos);

            if (gScore.find(nextState) == gScore.end() || new_g < gScore[nextState]) {
                gScore[nextState] = new_g;
                parentMap[nextState] = {current.c.first, current.c.second, current.seq};
                openSet.push(Node(nextPos, seq, new_g, 0.0, new_g));
                currentExploringEdges.push_back({current.c, nextPos});
            }
        }
    }
}

void Djikstra_Solver::step() {
    currentExploringEdges.clear();
    if (openSet.empty() || found) return;

    Node current = openSet.top();
    openSet.pop();

    State currentState = {current.c.first, current.c.second, current.seq};
    if (closedSet.count(currentState)) return;
    if (gScore.count(currentState) && current.g > gScore[currentState]) return;

    if (current.c == p_mapData->target && current.seq == (int)p_mapData->waypoints.size()) {
        this->lastState = currentState;
        this->found = true;
        this->totalCost = current.g;
        return;
    }

    closedSet.insert(currentState);
    expandNode(current);
}

bool Djikstra_Solver::isFinished() { return found || openSet.empty(); }

vector<cord> Djikstra_Solver::getOpenSetCoords() const {
    vector<cord> coords;
    for (const auto& node : openSet.get_container()) coords.push_back(node.c);
    return coords;
}

vector<cord> Djikstra_Solver::getClosedSetCoords() const {
    set<cord> unique;
    for (auto const& s : closedSet) unique.insert({s.r, s.c});
    return vector<cord>(unique.begin(), unique.end());
}

vector<cord> Djikstra_Solver::getFinalPathCoords() const {
    vector<cord> path;
    if (!found) return path;
    State curr = lastState;
    while (!(curr.r == start.first && curr.c == start.second && curr.seq == 0)) {
        path.push_back({curr.r, curr.c});
        curr = parentMap.at(curr);
    }
    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}