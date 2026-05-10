#include <Parser.hpp>

MapData parseMapInput(istream& input) {
    MapData data;
    if (!(input >> data.rows >> data.cols)) return data;
    input >> ws; 

    data.grid.resize(data.rows);
    data.costMap.assign(data.rows, vector<int>(data.cols, 0));
    
    for (int i = 0; i < data.rows; i++) {
        getline(input, data.grid[i]);
        data.grid[i].erase(remove(data.grid[i].begin(), data.grid[i].end(), '\r'), data.grid[i].end());

        for (int j = 0; j < (int)data.grid[i].length() && j < data.cols; j++) {
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

void parsing(MapData& activeMap,bool &isMapLoaded){
    auto selection = pfd::open_file("Pilih File Map Teks", "test/input/", { "Text Files", "*.txt", "All Files", "*" }).result();

    if (!selection.empty()) {
        string filePath = selection[0];
        parsing(activeMap, isMapLoaded, filePath);
    }
}

void parsing(MapData& activeMap, bool& isMapLoaded, const std::string& filePath) {
    ifstream file(filePath);
    if (file.is_open()) {
        activeMap = parseMapInput(file);
        isMapLoaded = (activeMap.rows > 0);
        file.close();
    } else {
        isMapLoaded = false;
    }
}


void saveSolution(const string& algo, const string& heuristic, int cost, int iterations, long double time, const string& moves) {
    ofstream file("test/output/solusi.txt");
    file << "Algoritma: " << algo << "\n";
    file << "Heuristic: " << heuristic << "\n";
    file << "Solusi: " << moves << "\n";
    file << "Cost: " << cost << "\n";
    file << "Iterasi: " << iterations << " iterasi\n";
    file << "Waktu: " << time << " ms\n";
    file.close();
}
