#pragma once

#include <PathFinder.hpp>
#include <BGFS.hpp>
#include <Djikstra.hpp>
#include <A_Star.hpp>
#include <UCS.hpp>
#include <raylib.h>
#include <raymath.h>


enum GameScreen{START,SOLVING};
class GUI {
private:
    int screen_width;
    int screen_height;

public:
    GUI(int width, int height) : screen_width(width), screen_height(height) {}
    void render(PathFinder* activeAlgorithm);
    ~GUI() = default;;
};

void DrawStyledBox(int x, int y, int width, int height, Color color, const char* text, int fontSize);