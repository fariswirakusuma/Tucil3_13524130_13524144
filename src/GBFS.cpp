#include <GBFS.hpp>

GBFS_Solver::GBFS_Solver(MapData* m, HeuristicType hType) : PathFinder(m), heuristic(hType) {
    double h_val = calculate_h(start, 0);
    openSet.push(Node(start, 0, 0.0, h_val, h_val));
    actualG[{start.first, start.second, 0}] = 0.0;
}

double GBFS_Solver::calculate_h(cord current, int currentSeq) {
    cord target = (p_mapData->waypoints.count(currentSeq)) ? p_mapData->waypoints.at(currentSeq) : p_mapData->target;
    double dx = abs(current.first - target.first);
    double dy = abs(current.second - target.second);
    if (heuristic == HEUR_MANHATTAN) return dx + dy;
    if (heuristic == HEUR_EUCLIDEAN) return sqrt(dx*dx + dy*dy);
    return max(dx, dy);
}

void GBFS_Solver::expandNode(Node current) {
    int dr[] = {-1, 1, 0, 0}, dc[] = {0, 0, -1, 1};
    for (int i = 0; i < 4; i++) {
        int r = current.c.first, c = current.c.second, seq = current.seq;
        double moveCost = 0;
        bool valid = false;
        while (true) {
            int nr = r + dr[i], nc = c + dc[i];
            if (nr < 0 || nr >= p_mapData->rows || nc < 0 || nc >= p_mapData->cols || p_mapData->grid[nr][nc] == 'X') {
                valid = true; break;
            }
            r = nr; c = nc;
            if (p_mapData->grid[r][c] == 'L') { valid = false; break; }
            moveCost += p_mapData->costMap[r][c];
            if (isdigit(p_mapData->grid[r][c]) && (p_mapData->grid[r][c]-'0' == seq)) seq++;
        }
        if (valid && (r != current.c.first || c != current.c.second)) {
            State nextS = {r, c, seq};
            if (!closedSet.count(nextS)) {
                double h_val = calculate_h({r, c}, seq);
                parentMap[nextS] = {current.c.first, current.c.second, current.seq};
                actualG[nextS] = actualG[{current.c.first, current.c.second, current.seq}] + moveCost;
                openSet.push(Node({r, c}, seq, actualG[nextS], h_val, h_val));
                currentExploringEdges.push_back({current.c, {r, c}});
            }
        }
    }
}

void GBFS_Solver::step() {
    currentExploringEdges.clear();
    if (openSet.empty() || found) return;
    Node current = openSet.top(); openSet.pop();
    State currS = {current.c.first, current.c.second, current.seq};
    if (closedSet.count(currS)) return;
    if (current.c == p_mapData->target && current.seq == (int)p_mapData->waypoints.size()) {
        found = true; lastState = currS; totalCost = actualG[currS]; return;
    }
    closedSet.insert(currS);
    expandNode(current);
}

bool GBFS_Solver::isFinished() { return found || openSet.empty(); }
vector<cord> GBFS_Solver::getOpenSetCoords() const {
    vector<cord> res;
    for(auto const& n : openSet.get_container()) res.push_back(n.c);
    return res;
}
vector<cord> GBFS_Solver::getClosedSetCoords() const {
    set<cord> u; for(auto const& s : closedSet) u.insert({s.r, s.c});
    return vector<cord>(u.begin(), u.end());
}
vector<cord> GBFS_Solver::getFinalPathCoords() const {
    vector<cord> p; if(!found) return p;
    State c = lastState;
    while(!(c.r == start.first && c.c == start.second && c.seq == 0)) {
        p.push_back({c.r, c.c}); c = parentMap.at(c);
    }
    p.push_back(start); reverse(p.begin(), p.end()); return p;
}

