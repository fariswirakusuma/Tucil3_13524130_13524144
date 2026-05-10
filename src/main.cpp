#include <bits/stdc++.h>
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
        // C++17 filesystem
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (!entry.is_regular_file()) continue;
            auto p = entry.path();
            if (p.extension() == ".txt") {
                files.push_back(p.filename().string());
            }
        }
        sort(files.begin(), files.end());
    } catch (...) {
        // ignore
    }
    return files;
}

int main() {
    GUI gui(1200, 900);
    uiScreen currentScreen = START;

    MapData activeMap;

    int stepCount = 0;
    long double executionTime = 0.0f;
    long double accumulatedTimeNs = 0.0f;
    float timer = 0.0f;
    float speed = 1.0f;

    float saveNotifTimer = 0.0f;
    bool showSaveNotif = false;

    bool closeWindowFromMenu = false;
    PathFinder* activeAlgorithm = nullptr;

    // For LOAD screen
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

                int maxItemsToShow = 10; // simple cap for UI
                int count = min((int)inputFiles.size(), maxItemsToShow);

                for (int i = 0; i < count; i++) {
                    string label = TextFormat("[%d] %s", i + 1, inputFiles[i].c_str());
                    Color col = (i == selectedInputIndex) ? SKYBLUE : DARKGRAY;
                    DrawStyledBox(startX, startY + i * (boxHeight + spacingY), boxWidth, boxHeight, col, label.c_str(), 16);

                    // keyboard selection
                    // Raylib key mapping is platform specific; so only support up to 9 explicitly.
                    if (i == 0 && IsKeyPressed(KEY_ONE)) selectedInputIndex = 0;

                    else if (i == 1 && IsKeyPressed(KEY_TWO)) selectedInputIndex = 1;
                    else if (i == 2 && IsKeyPressed(KEY_THREE)) selectedInputIndex = 2;
                    else if (i == 3 && IsKeyPressed(KEY_FOUR)) selectedInputIndex = 3;
                    else if (i == 4 && IsKeyPressed(KEY_FIVE)) selectedInputIndex = 4;
                    else if (i == 5 && IsKeyPressed(KEY_SIX)) selectedInputIndex = 5;
                    else if (i == 6 && IsKeyPressed(KEY_SEVEN)) selectedInputIndex = 6;
                    else if (i == 7 && IsKeyPressed(KEY_EIGHT)) selectedInputIndex = 7;
                    else if (i == 8 && IsKeyPressed(KEY_NINE)) selectedInputIndex = 8;
                }

                DrawStyledBox(startX, startY + count * (boxHeight + spacingY) + 20, boxWidth, boxHeight, MAROON, "[B] BACK", 16);

                if (IsKeyPressed(KEY_B)) {
                    currentScreen = START;
                }

                // Confirm selection (enter)
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
                    currentScreen = SOLVING;
                    activeAlgorithm = new UCS_Solver(&activeMap);
                } else if (IsKeyPressed(KEY_FOUR)) {
                    gui.setAlgoName("Dijkstra Search");
                    activeAlgorithm = new Djikstra_Solver(&activeMap);
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
                DrawStyledBox(startX, startY + (boxHeight + spacing) * 3 + 20, 120, 35, MAROON, "[B] BACK", 16);

                auto initAlgo = [&](HeuristicType h) {
                    if (activeAlgorithm != nullptr) delete activeAlgorithm;

                    if (pendingAlgo == ALGO_ASTAR) {
                        activeAlgorithm = new A_Star_Solver(&activeMap, h);
                    } else {
                        activeAlgorithm = new GBFS_Solver(&activeMap, h);
                    }
                    currentScreen = SOLVING;
                };

                if (IsKeyPressed(KEY_ONE)) {
                    gui.setHeuristicName("Manhattan (H1)");
                    initAlgo(HEUR_MANHATTAN);
                } else if (IsKeyPressed(KEY_TWO)) {
                    gui.setHeuristicName("Euclidean (H2)");
                    initAlgo(HEUR_EUCLIDEAN);
                } else if (IsKeyPressed(KEY_THREE)) {
                    gui.setHeuristicName("Chebyshev (H3)");
                    initAlgo(HEUR_CHEBYSHEV);
                }

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
                    executionTime = 0;
                    accumulatedTimeNs = 0;
                    stepCount = 0;
                    gui.resetUIState();
                    currentScreen = SELECT;
                    break;
                } else if (IsKeyPressed(KEY_S)) {
                    if (activeAlgorithm != nullptr && activeAlgorithm->isFinished()) {
                        int finalCost = (int)activeAlgorithm->getTotalCost();
                        saveSolution(gui.getAlgoName(), gui.getHeuristicName(), finalCost, stepCount, executionTime, gui.getSolutionMoves());

                        showSaveNotif = true;
                        saveNotifTimer = 1.3f;
                    }
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
                    } else {
                        executionTime = accumulatedTimeNs / 1000000.0;
                    }

                    gui.render(activeAlgorithm, activeMap, executionTime, stepCount);
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
        }

        if (showSaveNotif) {
            int screenW = GetScreenWidth();
            int screenH = GetScreenHeight();

            int boxWidth = 320;
            int boxHeight = 40;
            int boxX = (screenW / 2) - (boxWidth / 2);
            int boxY = screenH - 80;

            DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(DARKGREEN, 0.85f));
            DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, LIME);
            DrawText("Solusi berhasil disimpan ke solution.txt", boxX + 15, boxY + 12, 14, RAYWHITE);
        }

        EndDrawing();
    }

    if (activeAlgorithm != nullptr) {
        delete activeAlgorithm;
    }

    return 0;
}

