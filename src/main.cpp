#include <bits/stdc++.h>
#include <raylib.h>
#include <PathFinder.hpp>
#include <Parser.hpp>
#include <GUI.hpp>
#include <BGFS.hpp>
#include <Djikstra.hpp>
#include <A_Star.hpp>
#include <UCS.hpp>


using namespace std;

int main() {
    GUI gui(1200, 900);
    uiScreen currentScreen = START;
    MapData activeMap;
    int stepCount = 0;
    long double executionTime = 0.0f;
    long double accumulatedTimeNs = 0.0f;
    float timer = 0.0f;
    float speed = 1.0f;

    bool closeWindowFromMenu = false;
    PathFinder* activeAlgorithm = nullptr;

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
                    try {
                        bool isMapLoaded = false;
                        parsing(activeMap, isMapLoaded); 
                        if (isMapLoaded) {
                            std::cout << "DEBUG: Map Loaded " << activeMap.rows << "x" << activeMap.cols << "\n";
                            std::cout << "DEBUG: Start at (" << activeMap.start.first << "," << activeMap.start.second << ")\n";
                            std::cout << "DEBUG: Target at (" << activeMap.target.first << "," << activeMap.target.second << ")\n";
                            currentScreen = SELECT;
                        }
                    } catch (const std::exception& e) {
                        std::cout << ">> GAGAL LOAD: " << e.what() << "\n";
                    } 
                }
                if (IsKeyPressed(KEY_TWO)) {
                    closeWindowFromMenu = true;
                }
                break;
            }

            case SELECT: {
                int centerX = GetScreenWidth() / 2;
                DrawText("PILIH ALGORITMA", centerX - (MeasureText("PILIH ALGORITMA", 60) / 2), 200, 60, DARKBLUE);
                
                DrawStyledBox(centerX - 150, 320, 300, 60, SKYBLUE, "[1] A*", 20);
                DrawStyledBox(centerX - 150, 400, 300, 60, SKYBLUE, "[2] BGFS", 20);
                DrawStyledBox(centerX - 150, 480, 300, 60, SKYBLUE, "[3] UCS", 20);
                DrawStyledBox(centerX - 150, 560, 300, 60, SKYBLUE, "[4] Dijkstra", 20);
                DrawStyledBox(centerX - 150, 640, 300, 60, MAROON, "[5] Kembali", 20);

                if (IsKeyPressed(KEY_ONE)) {
                    gui.setAlgoName("A* Search");
                    currentScreen = HEURISTIC;
                }
                else if (IsKeyPressed(KEY_THREE)) {
                    gui.setAlgoName("UCS Search");
                    currentScreen = SOLVING;
                    activeAlgorithm = new UCS_Solver(&activeMap);
                }
                else if (IsKeyPressed(KEY_FIVE)) {
                    currentScreen = START;
                }
                break;
            }

            case SOLVING: {
                if (IsKeyPressed(KEY_B)) {
                    if (activeAlgorithm != nullptr) {
                        delete activeAlgorithm;
                        activeAlgorithm = nullptr;
                    }
                    executionTime = 0;
                    accumulatedTimeNs = 0;
                    stepCount = 0; 
                    gui.resetUIState();
                    currentScreen = SELECT;
                    break; 
                }

                if (activeAlgorithm != nullptr) {
                    if (!activeAlgorithm->isFinished()) {
                        timer += GetFrameTime();
                        if (timer >= (1.0f / speed)) {
                            auto stepStart = std::chrono::steady_clock::now();
                            activeAlgorithm->step();
                            auto stepEnd = std::chrono::steady_clock::now();
                            accumulatedTimeNs += std::chrono::duration_cast<std::chrono::nanoseconds>(stepEnd - stepStart).count();
                            stepCount++; 
                            timer = 0.0f;
                        }
                    } 
                    else {
                        executionTime = accumulatedTimeNs / 1000000.0;
                    }

                    gui.render(activeAlgorithm, activeMap, executionTime, stepCount);
                } 
                else {
                    DrawText("WARNING: activeAlgorithm is NULL", 350, 300, 20, RED);
                    DrawText("Tekan [B] untuk kembali", 350, 330, 16, DARKGRAY);
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
                DrawStyledBox(startX, startY + (boxHeight + spacing) * 3 + 20, 120, 35, MAROON, "[B] BACK", 16);

                if (IsKeyPressed(KEY_ONE)) {
                    gui.setHeuristicName("Manhattan (H1)");
                    activeAlgorithm = new A_Star_Solver(&activeMap, HEUR_MANHATTAN);
                    currentScreen = SOLVING;
                }
                else if (IsKeyPressed(KEY_TWO)) {
                    gui.setHeuristicName("Euclidean (H2)");
                    activeAlgorithm = new A_Star_Solver(&activeMap, HEUR_EUCLIDEAN);
                    currentScreen = SOLVING;
                }
                else if (IsKeyPressed(KEY_THREE)) {
                    gui.setHeuristicName("Chebyshev (H3)");
                    activeAlgorithm = new A_Star_Solver(&activeMap, HEUR_CHEBYSHEV);
                    currentScreen = SOLVING;
                }

                if (IsKeyPressed(KEY_B)) {
                    currentScreen = SELECT;
                }
                break;
            }
            default:
                break;
        }

        EndDrawing();
    }

    if (activeAlgorithm != nullptr) {
        delete activeAlgorithm;
    }

    return 0;
}
