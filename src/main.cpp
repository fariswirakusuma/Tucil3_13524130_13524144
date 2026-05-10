#include <bits/stdc++.h>
#include <chrono>
#include <raylib.h>
#include <PathFinder.hpp>
#include <Parser.hpp>
#include <GUI.hpp>
#include <GBFS.hpp>
#include <Djikstra.hpp>
#include <A_Star.hpp>
#include <UCS.hpp>

using namespace std;

enum SelectedAlgo { ALGO_ASTAR, ALGO_GBFS };
SelectedAlgo pendingAlgo = ALGO_ASTAR;

static vector<string> listInputFiles(const string& dirPath) {
    vector<string> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (!entry.is_regular_file()) continue;
            auto p = entry.path();
            if (p.extension() == ".txt") {
                files.push_back(p.filename().string());
            }
        }
        sort(files.begin(), files.end());
    } catch (...) {
    }
    return files;
}

int main() {
    GUI gui(1200, 900);
    uiScreen currentScreen = START;

    MapData activeMap;

    int stepCount = 0;
    long double cpuExecutionTime = 0.0;    
    long double simulationTime = 0.0;      
    long double simulationStartTime = 0.0; 
    float timer = 0.0f;
    float speed = 1.0f;

    float saveNotifTimer = 0.0f;
    bool showSaveNotif = false;

    bool closeWindowFromMenu = false;
    PathFinder* activeAlgorithm = nullptr;

    vector<string> inputFiles = listInputFiles("test/input");
    int selectedInputIndex = -1;

    while (!WindowShouldClose() && !closeWindowFromMenu) {
        BeginDrawing();
        ClearBackground(WHITE);

        switch (currentScreen) {
            case START: {
                int centerX = GetScreenWidth() / 2;
                DrawText("Ice Sliding Puzzle Solver", centerX - (MeasureText("Ice Sliding Puzzle Solver", 60) / 2), 200, 60, DARKBLUE);

                DrawStyledBox(centerX - 150, 400, 300, 60, SKYBLUE, "[1] LOAD", 20);
                DrawStyledBox(centerX - 150, 500, 300, 60, MAROON, "[2] EXIT", 20);

                if (IsKeyPressed(KEY_ONE)) {
                    inputFiles = listInputFiles("test/input");
                    selectedInputIndex = -1;
                    currentScreen = LOAD;
                }
                if (IsKeyPressed(KEY_TWO)) {
                    closeWindowFromMenu = true;
                }
                break;
            }

            case LOAD: {
                int centerX = GetScreenWidth() / 2;
                DrawText("PILIH MAP (test/input)", centerX - (MeasureText("PILIH MAP (test/input)", 40) / 2), 140, 40, DARKBLUE);

                int startX = 450;
                int startY = 240;
                int boxWidth = 320;
                int boxHeight = 40;
                int spacingY = 10;

                if (inputFiles.empty()) {
                    DrawText("Tidak ada file .txt di test/input", startX, startY, 20, RED);
                    DrawStyledBox(startX, startY + 70, boxWidth, boxHeight, MAROON, "[B] BACK", 16);
                    if (IsKeyPressed(KEY_B)) currentScreen = START;
                    break;
                }

                int maxItemsToShow = 10;
                int count = min((int)inputFiles.size(), maxItemsToShow);

                for (int i = 0; i < count; i++) {
                    string label = TextFormat("[%d] %s", i + 1, inputFiles[i].c_str());
                    Color col = (i == selectedInputIndex) ? SKYBLUE : DARKGRAY;
                    DrawStyledBox(startX, startY + i * (boxHeight + spacingY), boxWidth, boxHeight, col, label.c_str(), 16);
                    if (i < 9 && IsKeyPressed(KEY_ONE + i)) {
                        selectedInputIndex = i;
                    }
                }

                DrawStyledBox(startX, startY + count * (boxHeight + spacingY) + 20, boxWidth, boxHeight, MAROON, "[B] BACK", 16);

                if (IsKeyPressed(KEY_B)) {
                    currentScreen = START;
                }
                if (selectedInputIndex >= 0 && IsKeyPressed(KEY_ENTER)) {
                    string filePath = string("test/input/") + inputFiles[selectedInputIndex];
                    bool isMapLoaded = false;
                    parsing(activeMap, isMapLoaded, filePath);

                    if (isMapLoaded) {
                        currentScreen = SELECT;
                    } else {
                        selectedInputIndex = -1;
                        DrawText("Gagal load map", startX, startY - 30, 20, RED);
                    }
                }

                DrawText("Pilih [1..N], lalu tekan [ENTER]", startX, startY - 60, 18, GRAY);
                break;
            }

            case SELECT: {
                int centerX = GetScreenWidth() / 2;
                DrawText("PILIH ALGORITMA", centerX - (MeasureText("PILIH ALGORITMA", 60) / 2), 200, 60, DARKBLUE);

                DrawStyledBox(centerX - 150, 320, 300, 60, SKYBLUE, "[1] A*", 20);
                DrawStyledBox(centerX - 150, 400, 300, 60, SKYBLUE, "[2] GBFS", 20);
                DrawStyledBox(centerX - 150, 480, 300, 60, SKYBLUE, "[3] UCS", 20);
                DrawStyledBox(centerX - 150, 560, 300, 60, SKYBLUE, "[4] Dijkstra", 20);
                DrawStyledBox(centerX - 150, 640, 300, 60, MAROON, "[5] Kembali", 20);

                auto resetTimers = [&]() {
                    cpuExecutionTime = 0.0;
                    simulationTime = 0.0;
                    simulationStartTime = GetTime();
                };

                if (IsKeyPressed(KEY_ONE)) {
                    gui.setAlgoName("A* Search");
                    pendingAlgo = ALGO_ASTAR;
                    currentScreen = HEURISTIC;
                } else if (IsKeyPressed(KEY_TWO)) {
                    gui.setAlgoName("GBFS Search");
                    pendingAlgo = ALGO_GBFS;
                    currentScreen = HEURISTIC;
                } else if (IsKeyPressed(KEY_THREE)) {
                    gui.setAlgoName("UCS Search");
                    activeAlgorithm = new UCS_Solver(&activeMap);
                    resetTimers();
                    currentScreen = SOLVING;
                } else if (IsKeyPressed(KEY_FOUR)) {
                    gui.setAlgoName("Dijkstra Search");
                    activeAlgorithm = new Djikstra_Solver(&activeMap);
                    resetTimers();
                    currentScreen = SOLVING;
                } else if (IsKeyPressed(KEY_FIVE)) {
                    currentScreen = START;
                }
                break;
            }

            case HEURISTIC: {
                int startX = 350;
                int startY = 180;
                int boxWidth = 350;
                int boxHeight = 45;
                int spacing = 15;

                DrawText("PILIH HEURISTIC:", startX, startY - 40, 22, SKYBLUE);
                DrawStyledBox(startX, startY, boxWidth, boxHeight, DARKGRAY, "[1] Manhattan Distance (H1)", 18);
                DrawStyledBox(startX, startY + (boxHeight + spacing), boxWidth, boxHeight, DARKGRAY, "[2] Euclidean Distance (H2)", 18);
                DrawStyledBox(startX, startY + (boxHeight + spacing) * 2, boxWidth, boxHeight, DARKGRAY, "[3] Chebyshev Distance (H3)", 18);

                
                DrawStyledBox(startX, startY + (boxHeight + spacing) * 4 + 20, 120, 35, MAROON, "[B] BACK", 16);

                auto initAlgo = [&](HeuristicType h) {
                    if (activeAlgorithm != nullptr) delete activeAlgorithm;

                    if (pendingAlgo == ALGO_ASTAR) {
                        activeAlgorithm = new A_Star_Solver(&activeMap, h);
                    } else {
                        activeAlgorithm = new GBFS_Solver(&activeMap, h);
                    }
                    cpuExecutionTime = 0.0;
                    simulationTime = 0.0;
                    simulationStartTime = GetTime();  
                    currentScreen = SOLVING;
                };

                if (IsKeyPressed(KEY_ONE)) { gui.setHeuristicName("Manhattan (H1)"); initAlgo(HEUR_MANHATTAN); } 
                else if (IsKeyPressed(KEY_TWO)) { gui.setHeuristicName("Euclidean (H2)"); initAlgo(HEUR_EUCLIDEAN); } 
                else if (IsKeyPressed(KEY_THREE)) { gui.setHeuristicName("Chebyshev (H3)"); initAlgo(HEUR_CHEBYSHEV); }
              

                if (IsKeyPressed(KEY_B)) {
                    currentScreen = SELECT;
                }
                break;
            }

            case SOLVING: {
                if (IsKeyPressed(KEY_B)) {
                    if (activeAlgorithm != nullptr) {
                        delete activeAlgorithm;
                        activeAlgorithm = nullptr;
                    }
                    cpuExecutionTime = 0.0;
                    stepCount = 0;
                    gui.resetUIState();
                    currentScreen = SELECT;
                    break;
                } else if (IsKeyPressed(KEY_S)) {
                    if (activeAlgorithm != nullptr && activeAlgorithm->isFinished()) {
                        int finalCost = (int)activeAlgorithm->getTotalCost();
                        saveSolution(gui.getAlgoName(), gui.getHeuristicName(), finalCost, stepCount, cpuExecutionTime, gui.getSolutionMoves());

                        showSaveNotif = true;
                        saveNotifTimer = 1.3f;
                    }
                }

                if (activeAlgorithm != nullptr) {
                    if (!activeAlgorithm->isFinished()) {
                        timer += GetFrameTime();
                        
                        simulationTime = (GetTime() - simulationStartTime) * 1000.0;
                        
                        if (timer >= (1.0f / speed)) {
                            auto startNodeEx = std::chrono::steady_clock::now();
                            activeAlgorithm->step();
                            auto endNodeEx = std::chrono::steady_clock::now();
                            
                            cpuExecutionTime += std::chrono::duration<long double, std::milli>(endNodeEx - startNodeEx).count();

                            stepCount++;
                            timer = 0.0f;
                        }
                    } 

                    gui.render(activeAlgorithm, activeMap, cpuExecutionTime, stepCount);
                    DrawText(TextFormat("Sim Time : %.2Lf ms", simulationTime), 15, 290, 16, YELLOW); 

                } else {
                    DrawText("WARNING: activeAlgorithm is NULL", 350, 300, 20, RED);
                    DrawText("Tekan [B] untuk kembali", 350, 330, 16, DARKGRAY);
                }
                break;
            }

            default:
                break;
        }

        if (showSaveNotif) {
            saveNotifTimer -= GetFrameTime();
            if (saveNotifTimer <= 0.0f) {
                showSaveNotif = false;
            }

            int screenW = GetScreenWidth();
            int screenH = GetScreenHeight();

            int boxWidth = 320;
            int boxHeight = 40;
            int boxX = (screenW / 2) - (boxWidth / 2);
            int boxY = screenH - 80;

            DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(DARKGREEN, 0.85f));
            DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, LIME);
            DrawText("Solusi berhasil disimpan ke output/solusi.txt", boxX + 15, boxY + 12, 14, RAYWHITE);
        }

        EndDrawing();
    }

    if (activeAlgorithm != nullptr) {
        delete activeAlgorithm;
    }

    return 0;
}