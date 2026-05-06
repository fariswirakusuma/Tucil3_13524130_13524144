#include <GUI.hpp>
#include <PathFinder.hpp>
#include <BGFS.hpp>
#include <Djikstra.hpp>
#include <A_Star.hpp>

void GUI::drawBaseMap(const MapData& map, cord actorPos) {
    for (int i = 0; i < map.rows; i++) {
        for (int j = 0; j < map.cols; j++) {
            float xPos = (float)(j * tileSize) + offsetX;
            float yPos = (float)(i * tileSize) + offsetY;
            DrawRectangle(xPos, yPos, tileSize, tileSize, RAYWHITE);
            DrawRectangleLines(xPos, yPos, tileSize, tileSize, LIGHTGRAY);
            if (i == actorPos.first && j == actorPos.second) {
                DrawRectangle(xPos, yPos, tileSize, tileSize, BLUE);
                DrawText("Z", xPos + (tileSize / 3), yPos + (tileSize / 4), 20, WHITE);
                continue; 
            }

            char cell = map.grid[i][j];
            switch (cell) {
                case 'X': DrawRectangle(xPos, yPos, tileSize, tileSize, BLACK); break;
                case 'O': 
                    DrawRectangle(xPos, yPos, tileSize, tileSize, GREEN);
                    DrawText("O", xPos + (tileSize/3), yPos + (tileSize/4), 20, WHITE);
                    break;
                case 'L': DrawRectangle(xPos, yPos, tileSize, tileSize, ORANGE); break;
                default:
                    if (isdigit(cell)) {
                        DrawRectangle(xPos, yPos, tileSize, tileSize, PURPLE);
                        DrawText(TextFormat("%c", cell), xPos + (tileSize/3), yPos + (tileSize/4), 20, WHITE);
                    }
                    break;
            }
        }
    }
}

void GUI::drawAlgorithmOverlay(PathFinder* activeAlgorithm) {
    if (activeAlgorithm == nullptr) return;

    vector<cord> openSet = activeAlgorithm->getOpenSetCoords();
    vector<cord> closedSet = activeAlgorithm->getClosedSetCoords();

    for (auto const& c : closedSet) {
        float xPos = (float)(c.second * tileSize) + offsetX;
        float yPos = (float)(c.first * tileSize) + offsetY;
        
        DrawRectangle(xPos, yPos, tileSize, tileSize, Fade(RED, 0.3f));
    }

    for (auto const& c : openSet) {
        float xPos = (float)(c.second * tileSize) + offsetX;
        float yPos = (float)(c.first * tileSize) + offsetY;
        
        DrawRectangle(xPos + 5, yPos + 5, tileSize - 10, tileSize - 10, Fade(GOLD, 0.6f));
    }
}

void GUI::drawFinalPath(const vector<cord>& path) {
    if (path.empty()) return;

    for (size_t i = 0; i < path.size(); i++) {
        float xPos = (float)(path[i].second * tileSize) + offsetX;
        float yPos = (float)(path[i].first * tileSize) + offsetY;

        DrawRectangle(xPos + 10, yPos + 10, tileSize - 20, tileSize - 20, BLUE);
        if (i > 0) {
            float prevX = (float)(path[i-1].second * tileSize) + offsetX + (tileSize / 2);
            float prevY = (float)(path[i-1].first * tileSize) + offsetY + (tileSize / 2);
            float currX = xPos + (tileSize / 2);
            float currY = yPos + (tileSize / 2);

            DrawLineEx({prevX, prevY}, {currX, currY}, 5.0f, DARKBLUE);
        }
    }
}


void GUI::render(PathFinder* activeAlgorithm, const MapData& map, long long execTime, int steps){
    if (activeAlgorithm == nullptr) return;

    bool finished = activeAlgorithm->isFinished();
    int sidebarWidth = 300;

    if (finished && !resultProcessed) {
        finalPath = activeAlgorithm->getFinalPathCoords();
        solutionMoves = getDirectionString(finalPath);
        totalCost = (int)activeAlgorithm->getTotalCost();
        resultProcessed = true;
    }

    cord currentActorPos = map.start;
    if (finished && !finalPath.empty()) {
        currentActorPos = isPlaybackActive ? finalPath[playbackIndex] : finalPath.back();
    }

    drawBaseMap(map, currentActorPos);

    if (!finished) {
        const auto& edges = activeAlgorithm->getExploringEdges();
        for (const auto& edge : edges) {
            Vector2 start = {(float)edge.first.second * tileSize + offsetX + tileSize/2, 
                             (float)edge.first.first * tileSize + offsetY + tileSize/2};
            Vector2 end   = {(float)edge.second.second * tileSize + offsetX + tileSize/2, 
                             (float)edge.second.first * tileSize + offsetY + tileSize/2};
            
            float nodeX = (float)(edge.second.second * tileSize) + offsetX;
            float nodeY = (float)(edge.second.first * tileSize) + offsetY;

            DrawLineEx(start, end, 5.0f, Fade(DARKBLUE, 0.3f)); 
            DrawRectangle(nodeX + 10, nodeY + 10, tileSize - 20, tileSize - 20, Fade(BLUE, 0.3f));
        }
    } 
    else if (!finalPath.empty()) {
        drawFinalPath(finalPath);
    }

    DrawRectangle(0, 0, sidebarWidth, screen_height, (Color){20, 20, 20, 255});
    DrawRectangle(sidebarWidth - 2, 0, 2, screen_height, (Color){50, 50, 50, 255});

    int py = 30;
    const int margin = 25;
    const int lh = 28;

    DrawText("ICE SLIDING SOLVER", margin, py, 20, SKYBLUE);
    DrawRectangle(margin, py + 28, 150, 2, SKYBLUE);
    
    py += 55;
    DrawText(TextFormat("Algo  : %s", currentAlgoName.c_str()), margin, py, 17, LIGHTGRAY);
    if (currentAlgoName == "A* Search")DrawText(TextFormat("Heur  : %s", currentHeuristicName.c_str()), margin, py += lh, 17, LIGHTGRAY);
    

    py += 45;
    DrawText("ENGINE STATS", margin, py, 15, GRAY);
    DrawText(TextFormat("Iterasi : %d", stepCount), margin, py += 25, 18, RAYWHITE);
    DrawText(TextFormat("Cost    : %d", totalCost), margin, py += lh, 18, GOLD);
    

    py += 50;
    if (!finished) {
        DrawText("STATUS: SEARCHING...", margin, py, 18, VIOLET);
        timer += GetFrameTime();
        if (timer >= (1.0f / speed)) {
            activeAlgorithm->step();
            stepCount++;
            timer = 0.0f;
        }
    } else {
        DrawText("SEARCH FINISHED", margin, py, 18, LIME);
        py += 35;
        DrawText("Solution Moves:", margin, py, 14, GRAY);
        DrawText(solutionMoves.c_str(), margin, py += 20, 18, RAYWHITE);

        py += 45;
        if (!isPlaybackActive) {
            DrawText(">> Tekan [P] Playback", margin, py, 17, ORANGE);
            if (IsKeyPressed(KEY_P)) isPlaybackActive = true;
        } else {
            DrawRectangle(margin - 5, py - 5, 250, 45, (Color){40, 40, 40, 255});
            const char* stepLabel = (playbackIndex == 0) ? "Initial State" : 
                                    TextFormat("Step %d : %c", playbackIndex, solutionMoves[playbackIndex-1]);
            DrawText(stepLabel, margin, py + 8, 19, VIOLET);
            
            if (IsKeyPressed(KEY_RIGHT) && playbackIndex < (int)finalPath.size() - 1) playbackIndex++;
            if (IsKeyPressed(KEY_LEFT) && playbackIndex > 0) playbackIndex--;
        }
    }
    py = screen_height - 150; 
    const int lineH = 22; 
    DrawText(TextFormat("Execution : %lld ms", execTime), margin, py, 15, GRAY);
    DrawText(TextFormat("Steps     : %d", steps), margin, py += lineH, 15, GRAY);
    DrawText(TextFormat("Speed     : %.1fx (+/-)", speed), margin, py += lineH, 15, GRAY);
    py += 30;

    DrawText(">> Simpan Solusi [S]", margin, py, 17, RED);
    if (IsKeyPressed(KEY_S)) saveSolutionToTxt(map, solutionMoves);
    DrawText(">> Kembali [ESC] / [B]", margin, py += 25, 15, DARKGRAY);
    }

void DrawStyledBox(int x, int y, int width, int height, Color color, const char* text, int fontSize) {
    DrawRectangle(x + 2, y + 2, width, height, Fade(BLACK, 0.3f)); 
    DrawRectangle(x, y, width, height, color);
    DrawRectangleLines(x, y, width, height, DARKGRAY);

    int textWidth = MeasureText(text, fontSize);
    DrawText(text, x + (width / 2) - (textWidth / 2), y + (height / 2) - (fontSize / 2), fontSize, WHITE);
}

GUI::~GUI() {
    if (IsWindowReady()) {
        CloseWindow();
    }
}
void GUI::setTileSize(int size){
    tileSize = size;
}

string GUI::getDirectionString(const vector<cord>& path) {
    string moves = "";
    for (size_t i = 0; i < path.size() - 1; ++i) {
        int dr = path[i+1].first - path[i].first;
        int dc = path[i+1].second - path[i].second;
        if (dr < 0) moves += "U";
        else if (dr > 0) moves += "D";
        else if (dc < 0) moves += "L";
        else if (dc > 0) moves += "R";
    }
    return moves;
}

void GUI::saveSolutionToTxt(const MapData&, const string& moves) {
    ofstream outFile("test/output/solusi.txt");
    if (outFile.is_open()) {
        outFile << "Algoritma: " << currentAlgoName << "\n";
        outFile << "Heuristic: " << currentHeuristicName << "\n";
        outFile << "Solusi Yang Ditemukan : " << moves << "\n";
        outFile << "Cost dari Solusi : " << totalCost << "\n";
        outFile << "Banyak iterasi: " << stepCount << "\n";
        outFile << "Waktu eksekusi: " << executionTime << " ms\n";
        outFile.close();
    }
}