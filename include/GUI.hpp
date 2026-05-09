#pragma once

#include <PathFinder.hpp>
#include <BGFS.hpp>
#include <Djikstra.hpp>
#include <A_Star.hpp>
#include <UCS.hpp>
#include <raylib.h>
#include <raymath.h>


enum uiScreen { START, SELECT, HEURISTIC, SOLVING };
class GUI {
private:
    std::string currentAlgoName = "";    
    std::string currentHeuristicName = ""; 
    int screen_width;
    int screen_height;
    int tileSize;
    float speed = 1.0f;      
    float timer = 0.0f;
    int stepCount = 0;
    vector<cord> finalPath;

    int offsetX = 350; 
    int offsetY = 100;

    int totalPathSteps = 0;
    int totalCost = 0;
    long long executionTime = 0;
    string solutionMoves = "";

    bool isPlaybackActive = false; 
    int playbackIndex = 0;
    bool resultProcessed = false; 

    void drawBaseMap(const MapData& map, cord actorPos);
public:
    GUI(int width, int height) : screen_width(width), screen_height(height) {
        InitWindow(screen_width, screen_height, "Ice Sliding Puzzle Solver");
        SetTargetFPS(60);
        tileSize = 40;
        SetExitKey(KEY_Q);
    }
    void resetUIState() {
        isPlaybackActive = false;
        playbackIndex = 0;
        resultProcessed = false;
        stepCount = 0;
        totalCost = 0;
        finalPath.clear();
    }
    void drawAlgorithmOverlay(PathFinder* activeAlgorithm) ;
    void drawFinalPath(const std::vector<cord>& path);
    void render(PathFinder* activeAlgorithm, const MapData& map, long double execTime, int steps);
    string getDirectionString(const std::vector<cord>& path);
    ~GUI();
    void setTileSize(int size);
    void saveSolutionToTxt(const MapData&, const std::string& moves);
    void setAlgoName(const std::string& name) { currentAlgoName = name; }
    void setHeuristicName(const std::string& name) { currentHeuristicName = name; };
    int getTotalCost() const { return totalCost; };
    string getAlgoName() const { return currentAlgoName; };
    string getHeuristicName() const { return currentHeuristicName; };
    string getSolutionMoves() const { return solutionMoves; };
};

void DrawStyledBox(int x, int y, int width, int height, Color color, const char* text, int fontSize);
