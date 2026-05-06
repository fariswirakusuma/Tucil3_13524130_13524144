#include <Parser.hpp>

MapData parseMapInput(std::istream& input) {
    MapData data;
    std::string command;
    
    input >> command >> data.rows >> data.cols;
    
    data.grid.resize(data.rows);
    data.costMap.assign(data.rows, std::vector<int>(data.cols, 0));
    
    for (int i = 0; i < data.rows; i++) {
        input >> data.grid[i];
        for (int j = 0; j < data.cols; j++) {
            char cell = data.grid[i][j];
            if (cell == 'Z') data.start = {i, j};
            else if (cell == 'O') data.target = {i, j};
            else if (isdigit(cell)) data.waypoints[cell - '0'] = {i, j};
        }
    }
    
    for (int i = 0; i < data.rows; i++) {
        for (int j = 0; j < data.cols; j++) {
            input >> data.costMap[i][j];
        }
    }
    
    return data;
}

void parsing(MapData& activeMap,bool isMapLoaded){
    auto selection = pfd::open_file("Pilih File Map Teks", ".", 
                                            { "Text Files", "*.txt", "All Files", "*" }).result();

    if (!selection.empty()) {
        std::string filePath = selection[0];
        std::ifstream file(filePath);
        
        if (file.is_open()) {
            activeMap = parseMapInput(file);
            if (activeMap.rows > 0) {
                isMapLoaded = true;
            }
            file.close();
        }
    }
}