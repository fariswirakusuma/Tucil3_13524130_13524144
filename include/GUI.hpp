#pragma once

#include <PathFinder.hpp>
#include <BGFS.hpp>
#include <Djikstra.hpp>
#include <A_Star.hpp>
#include <UCS.hpp>

class GUI {
private:
    int screen_width;
    int screen_height;

public:
    GUI(int width, int height) : screen_width(width), screen_height(height) {}
    void render(PathFinder* activeAlgorithm);
    ~GUI() = default;;
};