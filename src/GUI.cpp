#include <GUI.hpp>
#include <PathFinder.hpp>
#include <BGFS.hpp>
#include <Djikstra.hpp>
#include <A_Star.hpp>
void GUI::render(PathFinder* activeAlgorithm){
    while(true){
        activeAlgorithm->step();
        if(activeAlgorithm->isFinished()){
            break;
        }
    }
}

void DrawStyledBox(int x, int y, int width, int height, Color color, const char* text, int fontSize){
    DrawRectangle(x, y, width, height, color);
    DrawText(text, x + width / 2 - MeasureText(text, fontSize) / 2, y + height / 2 - fontSize / 2, fontSize, BLACK);
}