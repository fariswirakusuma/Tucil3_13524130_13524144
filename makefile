# Deteksi OS
CXX := g++

ifeq ($(OS),Windows_NT)
    # Konfigurasi Windows
    TARGET_OS := WINDOWS
    RM := del /Q /F
    MKDIR := mkdir
    # Linker flags untuk Windows (MinGW)
    LDFLAGS := -lraylib -lgdi32 -lwinmm
    EXT := .exe
else
    # Konfigurasi Linux
    TARGET_OS := LINUX
    RM := rm -rf
    MKDIR := mkdir -p
    # Linker flags untuk Linux
    LDFLAGS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    EXT :=
endif

# Direktori
SRC_DIR     := src
INC_DIR     := include
OBJ_DIR     := obj
BIN_DIR     := bin

# Nama Executable
TARGET      := $(BIN_DIR)/game$(EXT)

# Source dan Objects (Mendeteksi semua file .cpp)
SRCS        := $(filter-out $(SRC_DIR)/BGFS.cpp,$(wildcard $(SRC_DIR)/*.cpp))
OBJS        := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Flag Kompilasi C++
CXXFLAGS    := -Wall -Wextra -std=c++17 -I$(INC_DIR)

# Rule Utama
all: prepare $(TARGET)

# Membuat direktori jika belum ada
prepare:
ifeq ($(TARGET_OS),WINDOWS)
	@if not exist $(OBJ_DIR) $(MKDIR) $(OBJ_DIR)
	@if not exist $(BIN_DIR) $(MKDIR) $(BIN_DIR)
else
	@$(MKDIR) $(OBJ_DIR) $(BIN_DIR)
endif

# Link executable (Menyatukan semua file .o menjadi target di folder bin)
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Kompilasi .cpp ke .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Membersihkan hasil build
clean:
ifeq ($(TARGET_OS),WINDOWS)
	@if exist $(OBJ_DIR) rmdir /S /Q $(OBJ_DIR)
	@if exist $(BIN_DIR) rmdir /S /Q $(BIN_DIR)
else
	@$(RM) $(OBJ_DIR) $(BIN_DIR)
endif

run: all
ifeq ($(TARGET_OS),WINDOWS)
	@$(subst /,\,$(TARGET))
else
	@./$(TARGET)
endif