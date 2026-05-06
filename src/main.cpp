#include <bits/stdc++.h>
#include <raylib.h>
#include <PathFinder.hpp>
#include <GUI.hpp>
#include <BGFS.hpp>
#include <Djikstra.hpp>
#include <A_Star.hpp>
#include <UCS.hpp>


using namespace std;

int main(){
    GUI gm(1200,900);
    GameScreen currentScreen = START;

    bool closeWindowFromMenu = false;
    while (!WindowShouldClose() && !closeWindowFromMenu){
        BeginDrawing();
        ClearBackground(WHITE);
        switch (currentScreen)
        {
        case START:{
            int centerX = GetScreenWidth() / 2;
            int centerY = GetScreenHeight() / 2;
            DrawText("Ice Sliding Puzzle Solver", centerX - (MeasureText("Ice Sliding Puzzle Solver", 60) / 2), 200, 60, DARKBLUE);
            DrawStyledBox(centerX - 150, 400, 300, 60, SKYBLUE, "[1] LOAD", 20);
            DrawStyledBox(centerX - 150, 560, 300, 60, MAROON, "[2] EXIT", 20);

            if (IsKeyPressed(KEY_ONE)){
                try {


                } catch (const exception& e) {
                    std::cout << ">> GAGAL LOAD: " << e.what() << "\n";
                } 

            }
            if (IsKeyPressed(KEY_TWO)){
                closeWindowFromMenu = true;
            }

            break;
        }
        case SOLVING:{
            break;
        }
        default:
            break;
        }
    }
}