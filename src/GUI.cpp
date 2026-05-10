#include <GUI.hpp>
#include <PathFinder.hpp>
#include <GBFS.hpp>
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


void GUI::render(PathFinder* activeAlgorithm, const MapData& map, long double execTime, int iterations) {
    if (activeAlgorithm == nullptr) return;

    bool finished = activeAlgorithm->isFinished();
    int sidebarWidth = 300;

    if (finished && !resultProcessed) {
        finalPath = activeAlgorithm->getFinalPathCoords();
        solutionMoves = getDirectionString(finalPath);
        totalCost = (int)activeAlgorithm->getTotalCost();
        this->executionTime = execTime;
        this->stepCount = iterations;
        resultProcessed = true;
    }

    cord currentActorPos = map.start;
    if (finished && !finalPath.empty()) {
        currentActorPos = isPlaybackActive ? finalPath[playbackIndex] : finalPath.back();
    }

    drawBaseMap(map, currentActorPos);

    if (!finished || (finished && finalPath.empty())) {
        const auto& edges = activeAlgorithm->getExploringEdges();
        for (const auto& edge : edges) {
            Vector2 start = {(float)edge.first.second * tileSize + offsetX + tileSize/2, 
                             (float)edge.first.first * tileSize + offsetY + tileSize/2};
            Vector2 end   = {(float)edge.second.second * tileSize + offsetX + tileSize/2, 
                             (float)edge.second.first * tileSize + offsetY + tileSize/2};
            DrawLineEx(start, end, 3.0f, Fade(BLUE, 0.4f)); 
            DrawLineEx(start, end, 8.0f, Fade(BLUE, 0.15f)); 
            
            DrawCircleV(end, 8.0f, Fade(BLUE, 0.15f));
            DrawCircleV(start, 8.0f, Fade(BLUE, 0.15f));
        }
    }

    if (finished && !finalPath.empty()) {
        drawFinalPath(finalPath);
    }

    DrawRectangle(0, 0, sidebarWidth, screen_height, (Color){25, 25, 25, 255});
    
    int py = 30;
    const int margin = 20;

    DrawText("ICE SLIDING SOLVER", margin, py, 20, SKYBLUE);
    
    py += 60;
    DrawText("CONFIG", margin, py, 15, GRAY);
DrawText(TextFormat("Algo : %s", currentAlgoName.c_str()), margin, py += 25, 17, LIGHTGRAY);
    if (getAlgoName() == "A* Search"|| getAlgoName() == "GBFS Search")DrawText(TextFormat("Heur : %s", currentHeuristicName.c_str()), margin, py += 25, 17, LIGHTGRAY);
    
    
    py += 50;
    DrawText("ENGINE STATS", margin, py, 15, GRAY);
    DrawText(TextFormat("Iterasi : %d", iterations), margin, py += 25, 18, RAYWHITE);
    DrawText(TextFormat("Waktu   : %llf ms", execTime), margin, py += 25, 18, RAYWHITE);
    DrawText(TextFormat("Cost    : %d", totalCost), margin, py += 25, 18, GOLD);
    
    py += 50;
    if (!finished) {
        DrawText("STATUS: SEARCHING...", margin, py, 18, VIOLET);
    } else {
        if (finalPath.empty()) {
            DrawText("STATUS: NO SOLUTION", margin, py, 18, RED);
        } else {
            static float playbackSpeed = 2.0f;
            static bool isPaused = true;
            static float playbackTimer = 0.0f;
            static bool isPromptingJump = false;
            static std::string jumpInput = "";
            DrawText("STATUS: SUCCESS", margin, py, 18, LIME);
            
            py += 40;
            DrawText("Moves:", margin, py, 14, GRAY);
            DrawText(solutionMoves.c_str(), margin, py += 20, 16, RAYWHITE);

            py += 45;
            if (!isPlaybackActive) {
                DrawText(">> Press [P] Playback", margin, py, 16, ORANGE);
                if (IsKeyPressed(KEY_P)) { 
                    isPlaybackActive = true; 
                    playbackIndex = 0; 
                    isPaused = true; 
                }
            } else {

                if (!isPaused && !isPromptingJump) {
                    playbackTimer += GetFrameTime();
                    if (playbackTimer >= (1.0f / playbackSpeed)) {
                        if (playbackIndex < (int)finalPath.size() - 1) {
                            playbackIndex++;
                        } else {
                            isPaused = true; 
                        }
                        playbackTimer = 0.0f;
                    }
                }

                if (!isPromptingJump) {
                    if (IsKeyPressed(KEY_SPACE)) isPaused = !isPaused;
                    if (IsKeyPressed(KEY_RIGHT) && playbackIndex < (int)finalPath.size() - 1) { playbackIndex++; isPaused = true; }
                    if (IsKeyPressed(KEY_LEFT) && playbackIndex > 0) { playbackIndex--; isPaused = true; }
                    if (IsKeyPressed(KEY_EQUAL)) playbackSpeed += 0.5f;
                    if (IsKeyPressed(KEY_MINUS)) playbackSpeed = fmaxf(0.5f, playbackSpeed - 0.5f); 
                    if (IsKeyPressed(KEY_ESCAPE)) { isPromptingJump = true; isPaused = true; jumpInput = ""; } 
                }

                const char* stepInfo = (playbackIndex == 0) ? "State: Initial" : TextFormat("Step %d : %c", playbackIndex, solutionMoves[playbackIndex - 1]);
                
                DrawRectangle(margin - 5, py - 5, 260, 105, (Color){45, 45, 45, 255});
                DrawText(stepInfo, margin, py + 10, 18, VIOLET);
                DrawText(TextFormat("Speed: %.1fx", playbackSpeed), margin, py + 35, 14, GOLD);
                DrawText(isPaused ? "Status: PAUSED [SPACE]" : "Status: PLAYING [SPACE]", margin, py + 55, 14, isPaused ? ORANGE : LIME);
                DrawText("Arrows: Step | +/-: Speed | ESC: Jump", margin, py + 80, 10, LIGHTGRAY);
                
                if (isPromptingJump) {
                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
                    DrawRectangle(GetScreenWidth()/2 - 150, GetScreenHeight()/2 - 50, 300, 100, DARKGRAY);
                    DrawText(TextFormat("Jump to step (0 - %d):", (int)finalPath.size()-1), GetScreenWidth()/2 - 130, GetScreenHeight()/2 - 40, 16, RAYWHITE);
                    DrawText(jumpInput.c_str(), GetScreenWidth()/2 - 130, GetScreenHeight()/2 - 10, 20, GOLD);
                    
                    int key = GetCharPressed();
                    while (key > 0) {
                        if (isdigit(key) && jumpInput.length() < 4) jumpInput += (char)key;
                        key = GetCharPressed();
                    }
                    if (IsKeyPressed(KEY_BACKSPACE) && jumpInput.length() > 0) jumpInput.pop_back();
                    if (IsKeyPressed(KEY_ENTER)) {
                        if (!jumpInput.empty()) {
                            int target = stoi(jumpInput);
                            if (target >= 0 && target < (int)finalPath.size()) playbackIndex = target;
                        }
                        isPromptingJump = false; 
                    }
                }
            }
        }
    }

    py = GetScreenHeight() - 60;
    DrawText("[S] Save Solution  [B] Back  [Q] Exit", margin, py, 14, DARKGRAY);
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
    if (path.size() < 2) return moves; 

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