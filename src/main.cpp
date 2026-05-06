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
    long long executionTime = 0;
    float timer = 0.0f;
    bool timerStarted = false;
    auto searchStart = std::chrono::steady_clock::now();
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
                        parsing(activeMap, isMapLoaded); // Fungsi parser yang sudah diperbaiki sebelumnya
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
                    // activeAlgorithm = new A_Star_Solver(activeMap);
                    
                    
                    // currentScreen = SOLVING;
                }
                else if (IsKeyPressed(KEY_FIVE)) {
                    currentScreen = START;
                }
                break;
            }

            case SOLVING: {
                // 1. INPUT HANDLING (Top Priority)
                if (IsKeyPressed(KEY_B) || IsKeyPressed(KEY_ESCAPE)) {
                    if (activeAlgorithm != nullptr) {
                        delete activeAlgorithm;
                        activeAlgorithm = nullptr;
                    }
                    // Reset state untuk sesi berikutnya
                    executionTime = 0;
                    stepCount = 0;
                    timerStarted = false;
                    gui.resetUIState();
                    currentScreen = SELECT;
                    break; 
                }

                if (activeAlgorithm != nullptr) {
                    // 2. START TIMER (Saat mulai langkah pertama)
                    if (!timerStarted && !activeAlgorithm->isFinished()) {
                        searchStart = std::chrono::steady_clock::now();
                        timerStarted = true;
                    }

                    // 3. LOGIC UPDATE (Stepping)
                    if (!activeAlgorithm->isFinished()) {
                        timer += GetFrameTime();
                        if (timer >= (1.0f / speed)) {
                            activeAlgorithm->step();
                            stepCount++;
                            timer = 0.0f;
                        }
                    } 
                    // 4. STOP TIMER (Tepat saat algoritma selesai)
                    else if (timerStarted) {
                        auto searchEnd = std::chrono::steady_clock::now();
                        executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(searchEnd - searchStart).count();
                        timerStarted = false; 
                    }

                    // 5. RENDERING
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

                // Judul Menu
                DrawText("PILIH HEURISTIC:", startX, startY - 40, 22, SKYBLUE);

                // Render Pilihan Heuristic dengan Styled Box
                DrawStyledBox(startX, startY, boxWidth, boxHeight, DARKGRAY, "[1] Manhattan Distance (H1)", 18);
                DrawStyledBox(startX, startY + (boxHeight + spacing), boxWidth, boxHeight, DARKGRAY, "[2] Euclidean Distance (H2)", 18);
                DrawStyledBox(startX, startY + (boxHeight + spacing) * 2, boxWidth, boxHeight, DARKGRAY, "[3] Chebyshev Distance (H3)", 18);
                
                // Tombol Navigasi Kembali
                DrawStyledBox(startX, startY + (boxHeight + spacing) * 3 + 20, 120, 35, MAROON, "[B] BACK", 16);

                // Logika Input dan Inisialisasi Solver
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
